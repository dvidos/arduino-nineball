

void BallKeeperClass::init()
{
    ball_game_over = false;
    shoot_agains = 0;
    ball_capture_enabled = false;
    releasing_balls = 0;
    serving_shooting_ball = 0;
    eject_tries = 0;

    LampMatrix.lamp_off(LAMP_CAPTURE_BALL);
    LampMatrix.lamp_off(LAMP_SHOOT_AGAIN);
}

void BallKeeperClass::on_switch_closed(byte switch_no)
{
    // we will ignore events if we are in progress of ejecting
    // from capture lane or outhole
    // (as other balls settle into place)
    switch (switch_no)
    {
        case SW_LEFT_LANE_CAPTURED_BALL:
            if (!releasing_balls)
                on_ball_captured();
            break;

        case SW_LEFT_LANE_SECOND_BALL:
            if (!releasing_balls &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL))
                on_ball_captured();
            break;

        case SW_LEFT_LANE_THIRD_BALL:
            if (!releasing_balls &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL) &&
                SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL))
                on_ball_captured();
            break;

        case SW_OUTHOLE_RIGHT:
            if (!serving_shooting_ball)
                on_ball_drained();
            break;

        case SW_OUTHOLE_MIDDLE:
            if (!serving_shooting_ball &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_RIGHT))
                on_ball_drained();
            break;

        case SW_OUTHOLE_LEFT:
            if (!serving_shooting_ball &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_MIDDLE) &&
                SwitchMatrix.is_switch_closed(SW_OUTHOLE_RIGHT))
                on_ball_drained();
            break;

        case SW_SHOOTING_LANE:
            // flag that we landed -- do we care?
    }
}

void BallKeeperClass::is_ball_game_over() {
    return ball_game_over;
}

void BallKeeperClass::on_ball_captured()
{
    if (!ball_capture_enabled) {
        // not allowed yet.
        LOG("Releasing ball as capturing is disabled");
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
        LOG("Releasing balls as they're all captured");
        release_captured_balls();
        return;
    }

    if (captured + drained < 3) {
        // so there seems to be a ball in the playfield,
        // e.g. we had multiball and a ball is captured again.
        // not doing anything.
        LOG("Ball captured, but another ball still somewhere in playfield");
        return;
    }

    // there is no ball in the playfield, give one to player
    LOG("No ball in playfield, giving one to player");
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
        LOG("Ball drained, but another ball still somewhere in playfield");
        return;
    }

    // so all balls either captured or drained.
    // if captured, release so that he can play.
    if (captured > 0) {
        // it seems a ball is still in play, so don't worry
        LOG("Releasing one captured ball for user");
        releasing_balls = true;
        TimeKeeper.callback_later(turn_off_releasing_balls, 1000);
        Coils.fire_capture_lane_eject();
        return;
    }

    // no captured balls, all balls are drained.
    if (shoot_agains > 0) {
        shoot_agains -= 1;
        Animator.blink_a_little(LAMP_SHOOT_AGAIN, shoot_agains > 0);
        send_ball_to_shooting_lane();
    }

    // if no captured, this could be the end, unless he has shoot again
    LOG("Game over for this ball No");
    ball_game_over = true;
}

void BallKeeperClass::grant_shoot_again() {
    if (shoot_agains == 3)
        return;

    LOG("Shoot again granted");
    shoot_agains += 1;

    // bells and whistles
    Audio.play(SOUND_EXPLOSION);
    Animator.blink_a_little(LAMP_SHOOT_AGAIN, 1);
}

void BallKeeperClass::enable_ball_capturing()
{
    ball_capture_enabled = true;
    LOG("Ball capturing enabled");

    // bells and whistles
    Audio.play(SOUND_FAST_PHASERS);
    Animator.blink_a_little(LAMP_CAPTURE_BALL, 1);
}

void BallKeeperClass::release_captured_balls()
{
    LOG("Releasing captured balls");
    eject_tries = 0;
    releasing_balls = true;
    Coils.fire_capture_lane_eject();
    TimeKeeper.callback_later(check_all_captured_balls_released, 1000);
}

void BallKeeperClass::send_ball_to_shooting_lane()
{
    LOG("Sending to shooting lane");
    eject_tries = 0;
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

btye BallKeeperClass::count_drained_balls()
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

void BallKeeperClass::check_all_captured_balls_released()
{
    // if all balls are out, we are good.
    if (!SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL) &&
        !SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL) &&
        !SwitchMatrix.is_switch_closed(SW_LEFT_LANE_THIRD_BALL))
        releasing_balls = false;
        return;

    // try again, up to some efforts
    BallKeeper.eject_tries++;
    if (BallKeeper.eject_tries == 7) {
        LOG("Warning: failed releasing captured balls");
        releasing_balls = false;
        return;
    }

    releasing_balls = true;
    Coils.fire_capture_lane_eject();
    TimeKeeper.callback_later(BallKeeperClass::check_all_captured_balls_released, 1000);
}

void BallKeeperClass::check_ball_got_to_eject_lane()
{
    // if ball made it to eject lane, we are good.
    if (SwitchMatrix.is_switch_closed(SW_SHOOTING_LANE))
        return;

    // try again, up to some efforts
    BallKeeper.eject_tries++;
    if (BallKeeper.eject_tries == 5) {
        LOG("Warning: failed launching ball in shooting lane");
        return;
    }

    Coils.fire_outhole_eject();
    TimeKeeper.callback_later(BallKeeperClass::check_eject_lane, 3000);
}

void BallKeeperClass::turn_off_releasing_balls() {
    BallKeeper.releasing_balls = false;
}

void BallKeeperClass::turn_off_serving_shooting_ball() {
    BallKeeper.serving_shooting_ball = false;
}
