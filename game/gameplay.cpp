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

extern CAudio Audio;
extern CAnimator Animator;
extern CSwitchMatrix SwitchMatrix;
extern CTimeKeeper TimeKeeper;
extern CCoils Coils;
extern CScoreDisplay ScoreDisplay;
extern CGameSettings GameSettings;
extern LoopTargetClass LoopTarget;
extern SpinnerClass Spinner;
extern BonusMultiplierClass BonusMultiplier;
extern ThreeBankTargetsClass ThreeBankTargets;
extern EightBankTargetsClass EightBankTargets;
extern BallKeeperClass BallKeeper;


CGameplay::CGameplay()
{
}

void CGameplay::start(byte mode)
{
    // start gameplay, in a specific mode.
    // e.g. EVA_HAPPY_MODE (unlimited balls)

    this->mode = mode;
    highest_player = 0;
    memset(player_info, 0, sizeof(player_info));

    this->prepare_game(0, 0);
    running = 1;

    // if not 3 balls in outhole, do not start

    LOG("Gameplay started in mode %d", mode);

    // we should derive the number of players and the number of balls.
    // then for each player/ball, we do
    BallKeeper.send_ball_to_shooting_lane();
    // when
    BallKeeper.is_ball_game_over();
    // we assign bonus
    // then move to next player/ball
    // when all exhausted, we go to idle
}

void CGameplay::handle_event(Event& e)
{
    if (e.type == switch_closed) {
        handle_switch_closed(e.number);
    }
}

void CGameplay::every_100_msecs_interrupt()
{
    if (!running)
        return;

    // remove one from each order of magnitude, add it to score
    BcdNum delta = BcdNum();

    bool found = false;
    for (byte nibble = 0; nibble < 8; nibble++)
    {
        if (temp_score.get_nibble(nibble)) {
            delta.zero();
            delta.set_nibble(nibble, 0x1);
            temp_score.subtract(delta);
            player_info[current_player].score.add(delta);
            found = true;
        }
    }

    if (found) {
        ScoreDisplay.show_bcd_num(1, player_info[current_player].score);
    }
}

void CGameplay::add_score_bcd(dword bcd)
{
    temp_score.add_bcd(bcd);
}

void CGameplay::handle_switch_closed(char switch_no) {
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


        // -------------------------------------------
        //  Things "written" above this line.
        // -------------------------------------------



        case SW_LEFT_LANE_CAPTURED_BALL: // fallthrough all of them
        case SW_LEFT_LANE_SECOND_BALL:
        case SW_LEFT_LANE_THIRD_BALL:
        case SW_OUTHOLE_RIGHT:
        case SW_OUTHOLE_MIDDLE:
        case SW_OUTHOLE_LEFT:
        case SW_SHOOTING_LANE:
            BallKeeper.on_switch_closed(switch_no);
            // we have to see if game is over,
            // or consume shoot again
            // then assign bonus (one-by-one),
            // move to next player or ball
            //if all finished, go back to attract mode.
            break;

        case SW_START:
            // add user before ball leaves the shooter
            // also, P2 could be used to denote profile No,
            // prior to pressing start...
            break;
    }
}


void CGameplay::prepare_game(byte player_no, byte ball_no)
{
    // set current player and ball no.
    current_player = player_no;
    player_info[current_player].ball_number = ball_no;
    temp_score.zero();

    // show player, ball, score
    ScoreDisplay.hide_display(0);
    ScoreDisplay.hide_display(1);
    ScoreDisplay.show_digit(2, current_player);
    ScoreDisplay.show_digit(6, player_info[current_player].ball_number);
    ScoreDisplay.show_bcd_num(1, player_info[current_player].score);

    Audio.play(SOUND_START);
    Coils.set_flippers_relay(1);

    // state
    collecting_bonuses = 0;

    Spinner.init();
    LoopTarget.init();
    BonusMultiplier.init(false);
    ThreeBankTargets.init();
    EightBankTargets.init(false);
}
