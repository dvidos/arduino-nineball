

void BallKeeperClass::init()
{
    ball_game_over = false;
    shoot_agains = 0;
    ball_capture_enabled = false;
    ball_release_in_progress = false;
    release_all_balls = false;
    ball_serving_in_progress = false;
    eject_tries = 0;

    LampMatrix.lamp_off(LAMP_CAPTURE_BALL);
    LampMatrix.lamp_off(LAMP_SHOOT_AGAIN);
}

/**
 * Called at machine boot time, way before attract
 * It's called from setup(), so can delay
 */
void BallKeeperClass::drain_any_captured_balls()
{
    byte count = count_captured_balls();
    if (count == 0)
        return;

    LOGM(M_RELEASING_BALLS_AND_WAITING_FOR_DRAINING);
    release_captured_balls();

    while (1) {
        count = count_drained_balls();
        if (count == 3)
            break;
        delay(1000);
    };
}

void BallKeeperClass::on_switch_closed(byte switch_no)
{
    // we will ignore events if we are in progress of ejecting
    // from capture lane or outhole (as other balls settle into place)

    if (!ball_release_in_progress) {
        if (
            (switch_no == SW_LEFT_LANE_CAPTURED_BALL) ||
            (
                switch_no == SW_LEFT_LANE_SECOND_BALL &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL)
            ) ||
            (
                switch_no == SW_LEFT_LANE_THIRD_BALL &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL) &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL)
            )
        )
            on_ball_captured();
    }

    if (!ball_serving_in_progress) {
        if (
            (switch_no == SW_OUTHOLE_RIGHT) ||
            (
                switch_no == SW_OUTHOLE_MIDDLE &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_RIGHT)
            ) ||
            (
                switch_no == SW_OUTHOLE_LEFT &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_MIDDLE) &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_RIGHT)
            )
        )
            on_ball_drained();
    }
}

bool BallKeeperClass::is_ball_game_over() {
    return ball_game_over;
}

void BallKeeperClass::on_ball_captured()
{
    if (!ball_capture_enabled) {
        // not allowed yet.
        LOGM(M_RELEASING_BALL_AS_CAPTURING_IS_DISABLED);
        release_captured_balls();
        return;
    }

    // we allow capturing. make current tartget!
    EightBankTargets.make_current_target_object(0);

    // also, if all balls captured, start multi ball.
    byte captured = count_captured_balls();
    byte drained = count_drained_balls();

    if (captured == 3) {
        // no point, we cannot serve more balls
        LOGM(M_RELEASING_BALLS_AS_THEY_ARE_ALL_CAPTURED);
        release_captured_balls();
        return;
    }

    if (captured + drained < 3) {
        // so there seems to be a ball in the playfield,
        // e.g. we had multiball and a ball is captured again.
        // not doing anything.
        LOGM(M_BALL_CAPTURED_BUT_ANOTHER_STILL_IN_PLAYFIELD);
        return;
    }

    // there is no ball in the playfield, give one to player
    LOGM(M_BALL_CAPTURED_GIVING_NEW_ONE_TO_PLAYER);
    send_ball_to_shooting_lane();
    return;
}

void BallKeeperClass::on_ball_drained()
{
    Audio.play(SOUND_OUTHOLE_DRAIN);
    // maybe we are in multiball, so no need to worry
    // see if there is a ball captured and release it

    byte captured = count_captured_balls();
    byte drained = count_drained_balls();

    if (captured + drained < 3) {
        // it seems a ball is still in play, so don't worry
        LOGM(M_BALL_DRAINED_BUT_ANOTHER_STILL_IN_PLAYFIELD);
        return;
    }

    // so all balls either captured or drained.
    // if captured, release so that he can play.
    if (captured > 0) {
        // it seems a ball is still in play, so don't worry
        LOGM(M_RELEASING_ONE_CAPTURED_BALL_FOR_USER);
        release_captured_balls(false);
        return;
    }

    // no captured balls, all balls are drained.
    if (shoot_agains > 0) {
        shoot_agains -= 1;
        Animator.blink_a_little(LAMP_SHOOT_AGAIN, shoot_agains > 0);
        send_ball_to_shooting_lane();
    }

    // if no captured, this could be the end, unless he has shoot again
    LOGM(M_GAME_OVER_FOR_THIS_BALL);
    ball_game_over = true;
}

void BallKeeperClass::grant_shoot_again() {
    if (shoot_agains == 3)
        return;

    LOGM(M_SHOOT_AGAIN_GRANTED);
    shoot_agains += 1;

    // bells and whistles
    Audio.play(SOUND_EXPLOSION);
    Animator.blink_a_little(LAMP_SHOOT_AGAIN, 1);
}

void BallKeeperClass::enable_ball_capturing()
{
    ball_capture_enabled = true;
    LOGM(M_BALL_CAPTURING_ENABLED);

    // bells and whistles
    Audio.play(SOUND_FAST_PHASERS);
    Animator.blink_a_little(LAMP_CAPTURE_BALL, 1);
}

void BallKeeperClass::release_captured_balls(bool all_balls)
{
    LOGM(M_RELEASING_CAPTURED_BALLS);
    eject_tries = 0;
    ball_release_in_progress = true;
    release_all_balls = all_balls;

    Coils.fire_capture_lane_eject();
    TimeKeeper.callback_later(check_release_captured_balls, 1000);
}

void BallKeeperClass::send_ball_to_shooting_lane()
{
    LOGM(M_SENDING_BALL_TO_SHOOTING_LANE);
    GameSettings.balls_served += 1;

    eject_tries = 0;
    ball_serving_in_progress = true;
    Coils.fire_outhole_eject();
    TimeKeeper.callback_later(check_ball_got_to_eject_lane, 3000);
}


byte BallKeeperClass::count_captured_balls()
{
    byte balls = 0;

    if (SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL))
        balls += 1;
    if (SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL))
        balls += 1;
    if (SwitchMatrix.is_switch_closed(SW_LEFT_LANE_THIRD_BALL))
        balls += 1;

    return balls;
}

byte BallKeeperClass::count_drained_balls()
{
    byte balls = 0;

    if (SwitchMatrix.is_switch_closed(SW_OUTHOLE_RIGHT))
        balls += 1;
    if (SwitchMatrix.is_switch_closed(SW_OUTHOLE_MIDDLE))
        balls += 1;
    if (SwitchMatrix.is_switch_closed(SW_OUTHOLE_LEFT))
        balls += 1;

    return balls;
}

void BallKeeperClass::check_release_captured_balls()
{
    if (!BallKeeper.release_all_balls) {
        // if we don't care for all balls, we are good.
        BallKeeper.ball_release_in_progress = false;
        return;
    }

    // if all balls are out, we are good.
    if (!SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL) &&
        !SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL) &&
        !SwitchMatrix.is_switch_closed(SW_LEFT_LANE_THIRD_BALL))
        BallKeeper.ball_release_in_progress = false;
        return;

    // try again, up to some efforts
    BallKeeper.eject_tries++;
    if (BallKeeper.eject_tries == 7) {
        LOGM(M_FAILED_RELEASING_CAPTURED_BALLS);
        BallKeeper.ball_release_in_progress = false;
        return;
    }

    BallKeeper.ball_release_in_progress = true;
    Coils.fire_capture_lane_eject();
    TimeKeeper.callback_later(BallKeeperClass::check_release_captured_balls, 1000);
}

void BallKeeperClass::check_ball_got_to_eject_lane()
{
    // if ball made it to eject lane, we are good.
    if (SwitchMatrix.is_switch_closed(SW_SHOOTING_LANE)) {
        BallKeeper.ball_serving_in_progress = false;
        return;
    }

    // try again, up to some efforts
    BallKeeper.eject_tries++;
    if (BallKeeper.eject_tries == 5) {
        LOGM(M_FAILED_LAUNCHING_BALL_TO_SHOOTER_LANE);
        BallKeeper.ball_serving_in_progress = false;
        return;
    }

    BallKeeper.ball_serving_in_progress = true;
    Coils.fire_outhole_eject();
    TimeKeeper.callback_later(BallKeeperClass::check_ball_got_to_eject_lane, 3000);
}
