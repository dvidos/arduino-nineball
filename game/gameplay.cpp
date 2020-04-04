/*
    the initial idea was to have a state machine,
    switching actions based on context and events that happened.
    the various timed actions (timeouts, switches detection) are performed in interrupts, so they have to be fast.
    therefore we will set flags from the interrupts and handle the events from the main loop.

    the main logic is to respond to events and kick off coils, sounds, lamps, scores, etc.

    adding score is done with an unsigned int, in BCD, in tens. That means that 3,000 is encoded in 0x0300.
    also, adding score is done on a temporary buffer and a background process adds it to the real score, in thousands
    this is to simulate slowly giving the user points and make it more enjoyable.

    slingshots, spinner and pop bumpers control the percentage of the time that the dead bumper, the two return lanes
    and the two outlanes and the two 3-bank lites are on. it can be setup to liberal or conservative.

*/


void CGameplay::start(byte mode)
{
    // if not 3 balls in outhole, do not start
    byte count = BallKeeper.count_drained_balls();
    if (count != 3) {
        Animator.blink_a_little(LAMP_START, 0);
        Animator.blink_a_little(LAMP_SHOOT_AGAIN, 0);
        return;
    }

    // start gameplay, in a specific mode.
    // e.g. EVA_HAPPY_MODE (unlimited balls)
    this->mode = mode;
    num_players = 1;
    memset(player_info, 0, sizeof(player_info));
    current_player = 0;

    prepare_player_ball_game();

    GameSettings.games_played += 1;

    LOG("Gameplay started in mode %d", mode);
}

void CGameplay::prepare_player_ball_game()
{
    temp_score.zero();

    game_running = true;
    game_collecting_bonuses = false;

    // show player, ball, score
    ScoreDisplay.hide_display(0);
    ScoreDisplay.hide_display(1);
    ScoreDisplay.show_digit(2, (current_player + 1));
    ScoreDisplay.show_digit(6, (player_info[current_player].ball_number + 1));
    ScoreDisplay.show_bcd_num(1, player_info[current_player].score);

    Audio.play(SOUND_START);
    Coils.set_flippers_relay(1);

    Spinner.init();
    LoopTarget.init();
    BonusMultiplier.init(player_info[current_player].achieved_6x_bonus_multiplier);
    ThreeBankTargets.init();
    EightBankTargets.init(player_info[current_player].super_bonus_made_prev_ball);
    BallKeeper.init();

    // reset housekeeping for this round
    player_info[current_player].achieved_l1_highscore = false;
    player_info[current_player].achieved_l2_highscore = false;
    player_info[current_player].achieved_l3_highscore = false;

    // finally...
    BallKeeper.send_ball_to_shooting_lane();
}

void CGameplay::every_100_msecs_interrupt()
{
    if (!game_running || temp_score.is_zero())
        return;

    // start at 10Ks, 1Ks, 100s 10s.
    BcdNum n = BcdNum();
    n.from_bcd(0x10000);
    if (temp_score > n) {
        temp_score.subtract(n);
        player_info[current_player].score.add(n);
    }
    n.from_bcd(0x1000);
    if (temp_score > n) {
        temp_score.subtract(n);
        player_info[current_player].score.add(n);
    }
    n.from_bcd(0x100);
    if (temp_score > n) {
        temp_score.subtract(n);
        player_info[current_player].score.add(n);
    }
    n.from_bcd(0x10);
    if (temp_score > n) {
        temp_score.subtract(n);
        player_info[current_player].score.add(n);
    }

    if (!game_collecting_bonuses) {
        // see if we passed any high score
        if (!player_info[current_player].achieved_l1_highscore &&
            player_info[current_player].score > GameSettings.awards_threshold[0]) {
            BallKeeper.grant_shoot_again();
            player_info[current_player].achieved_l1_highscore = true;
        }

        if (!player_info[current_player].achieved_l2_highscore &&
            player_info[current_player].score > GameSettings.awards_threshold[1]) {
            BallKeeper.grant_shoot_again();
            player_info[current_player].achieved_l2_highscore = true;
        }

        if (!player_info[current_player].achieved_l3_highscore &&
            player_info[current_player].score > GameSettings.awards_threshold[2]) {
            BallKeeper.grant_shoot_again();
            player_info[current_player].achieved_l3_highscore = true;
        }
    }

    ScoreDisplay.show_bcd_num(1, player_info[current_player].score);
}

void CGameplay::handle_switch(byte switch_no) {
    switch (switch_no)
    {
        case SW_TOP_LOOP_PASS:  // fallthrough
        case SW_LEFT_LANE_EXIT:
            LoopTarget.advance_value();
            break;
        case SW_TOP_LOOP_TARGET:
            LoopTarget.collect_value();
            break;
        case SW_LEFT_OUTLANE:  // fallthrough
        case SW_RIGHT_OUTLANE:
            LoopTarget.on_passed_outlane(switch_no);
            break;

        case SW_TOP_BANK_LEFT_TARGET:   // fallthrough, all of them
        case SW_TOP_BANK_CENTER_TARGET:
        case SW_TOP_BANK_RIGHT_TARGET:
        case SW_RIGHT_BANK_LEFT_TARGET:
        case SW_RIGHT_BANK_CENTER_TARGET:
        case SW_RIGHT_BANK_RIGHT_TARGET:
            ThreeBankTargets.on_target_hit(switch_no);
            break;

        case SW_SPINNER:
            Spinner.on_spinner_spun();
            break;

        case SW_SKILL_SHOT_TARGET:
            Spinner.advance_to_top_value();
            break;

        case SW_LEFT_BANK_TARGET_1: // fallthrough, all of them.
        case SW_LEFT_BANK_TARGET_2:
        case SW_LEFT_BANK_TARGET_3:
        case SW_LEFT_BANK_TARGET_4:
        case SW_LEFT_BANK_TARGET_5:
        case SW_LEFT_BANK_TARGET_6:
        case SW_LEFT_BANK_TARGET_7:
        case SW_LEFT_BANK_TARGET_8:
        case SW_LEFT_INLANE:
        case SW_RIGHT_INLANE:
        case SW_TOP_POP_BUMPER:
            EightBankTargets.on_target_hit(switch_no);
            break;

        case SW_MAIN_POP_BUMPER:
            // scores 100 or highest value on spinner (?!?!?!?)
            add_score_bcd(Spinner.get_spinner_score_bcd());
            EightBankTargets.start_spot_number_timeout();
            break;

        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:
            add_score_bcd(0x10);
            EightBankTargets.start_spot_number_timeout();
            break;

        case SW_LEFT_LANE_CAPTURED_BALL: // fallthrough all of them
        case SW_LEFT_LANE_SECOND_BALL:
        case SW_LEFT_LANE_THIRD_BALL:
        case SW_OUTHOLE_RIGHT:
        case SW_OUTHOLE_MIDDLE:
        case SW_OUTHOLE_LEFT:
        case SW_SHOOTING_LANE:
            BallKeeper.on_switch_closed(switch_no);
            if (BallKeeper.is_ball_game_over())
                on_this_player_ball_game_finished();
            break;

        case SW_START:
            // also, P2 could be used to denote profile No,
            // prior to pressing start...
            if (current_player == 0 &&
                player_info[current_player].ball_number == 0 &&
                num_players < 4) {
                num_players += 1;
                Audio.play(SOUND_COIN);
                Animator.blink_a_little(LAMP_START, num_players < 4);
            }
            break;
    }
}

void CGameplay::add_score_bcd(dword bcd)
{
    temp_score.add_bcd(bcd);
}

void CGameplay::collect_bonus() {
    LOG("Collecting bonuses");
    game_collecting_bonuses = true;

    byte multiplier = BonusMultiplier.get_multiplier();
    byte times;
    byte object_no;


    // collect super bonus if achieved in previous ball
    if (player_info[current_player].super_bonus_made_prev_ball) {
        Audio.play(SOUND_FAST_PHASERS);
        Animator.start_blinking(LAMP_SUPER_BONUS_77K);
        add_score_bcd(0x77000ul);
        while (!temp_score.is_zero())
            delay(100); // just wait
        Animator.stop_blinking();
    }

    for (object_no = 9; object_no >= 1; object_no--) {
        if (object_no > EightBankTargets.get_object_made())
            continue;

        Audio.play(SOUND_FAST_PHASERS);
        Animator.start_blinking(get_object_bonus_lamp(object_no));
        for (times = 0; times < multiplier; times++)
            add_score_bcd(((dword)object_no) << 16);
        while (!temp_score.is_zero())
            delay(100); // just wait
        Animator.stop_blinking();
    }

    if (player_info[current_player].score > GameSettings.highest_score_to_date) {
        GameSettings.highest_score_to_date = player_info[current_player].score;
    }

    // let's marvel this for a second.
    object_no = 0; // use this for now.
    for (times = 0; times < 6; times++) {
        if (object_no)
            ScoreDisplay.hide_display(0);
        else
            ScoreDisplay.show_bcd_num(1, player_info[current_player].score);
        object_no = !object_no;
        delay(400);
    }
    delay(2000);
}

inline byte CGameplay::get_object_bonus_lamp(byte object_no)
{
    // this is a way to store the lamps in code,
    // instead of a local variable in RAM
    switch (object_no)
    {
        case 1: return LAMP_BONUS_MADE_1;
        case 2: return LAMP_BONUS_MADE_2;
        case 3: return LAMP_BONUS_MADE_3;
        case 4: return LAMP_BONUS_MADE_4;
        case 5: return LAMP_BONUS_MADE_5;
        case 6: return LAMP_BONUS_MADE_6;
        case 7: return LAMP_BONUS_MADE_7;
        case 8: return LAMP_BONUS_MADE_8;
        case 9: return LAMP_BONUS_MADE_9;
    }

    return 0;
}

void CGameplay::on_this_player_ball_game_finished()
{
    Coils.set_flippers_relay(0);

    // we need to wait for the score to catch up
    // do this before carrying over the super bonus
    collect_bonus();

    // save super bonus for next ball,
    // AFTER we collect bonuses
    // this does sound like a bug in the original game software
    player_info[current_player].super_bonus_made_prev_ball = EightBankTargets.get_super_bonus_made();
    player_info[current_player].achieved_6x_bonus_multiplier = BonusMultiplier.get_multiplier() >= 6;

    // move to next player/ball etc.
    current_player++;
    if (current_player >= num_players) {
        current_player = 0;
        player_info[current_player].ball_number += 1;
    }
    register byte total_balls = GameSettings.five_balls_per_game ? 5 : 3;
    if (player_info[current_player].ball_number >= total_balls) {
        // we finished for good
        // for high scores, balls served and games played.
        GameSettings.save_to_eeprom();
        Attract.start();
    } else {
        // next player/ball
        prepare_player_ball_game();
    }
}

