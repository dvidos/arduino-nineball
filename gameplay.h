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

#include "constants.h"


extern CAudio Audio;
extern CAnimator Animator;
extern CSwitchMatrix SwitchMatrix;
extern CTimeKeeper TimeKeeper;
extern CCoils Coils;
extern CScoreDisplay ScoreDisplay;




class CGameplay
{
public:

    void start(byte mode);
    void handle_event(Event& e);
    void every_100_msecs_interrupt();

private:
    word mode: 2;                  // mode under which we are running
    word highest_player: 2;        // highest player number (0..3) (essentially num_players - 1)
    word current_player: 2;        // current player no

    struct player_info
    {
        word ball_number: 3;       // 1..5 shoot-agains don't increase this.
        BcdNum score;
    } player_info[4];
    BcdNum temp_score;             // a place to add score that is slowly moved to player score and display.

    word running: 1;               // whether we are running (or merely existing in memory)
    word waiting_for_ball_in_eject_lane: 1;
    word collecting_bonuses: 1;

    word loop_target_value: 3;      // 0..4 for 10k, 20k, 30k, 40k, 173k
    word spinner_value: 3;          // 0..4 for 100, 400, 900, 1600, 2500.
    word bonus_multiplier: 3;       // 1..7 for x1 .. x7 (minimum is 1, not zero)
    word left_outlane: 1;           // collects loop pass value if lit
    word right_outlane: 1;          // collects loop pass value if lit
    word left_inlane: 1;            // spots current target when lit
    word right_inlane: 1;           // spots current target when lit
    word top_pop_bumper: 1;         // spots current target when lit
    word extra_balls_awarded: 2;    // up to 3 extra balls can be awarded. shoot again will lite for them.
    word one_special_achieved: 1;   // whether one special is achieved, to avoid giving more, if not allowed.

    word next_object_to_make: 4;            // 1-8 and 9 means number 9.
    word current_number_nine_target: 3;     // moving target for "9"
    word current_8_bank_wow_target: 3;      // moving target for the 8-bank wow
    word current_3_banks_wow_target: 2;     // moving target for the 3-bank wow
    word current_8_bank_special_target: 3;  // moving target for 8 bank special.

    void handle_timeout(char timeout_no);
    void handle_switch_closed(char switch_no);

    void prepare_game(byte player_no, byte ball_no);
    dword get_spinner_score_bcd(byte spinner_value);
    void collect_and_reset_loop_target_value();
    void make_current_target_object();
    void on_left_bank_drop_target_down(byte number);

    class LoopTargetClass
    {
    public:
        byte value: 4; // 0=10k, 1=20k, 2=30k, 3=40k, 4=173k
        void init();
        void on_passed_loop();
        void on_target_hit();
    } LoopTarget;

    class SpinnerClass
    {
    public:
        byte value: 4; // 0=100, 1=400, 2=900, 3=1600, 4=2500
        void init();
        void on_value_increased();
        void on_spinner_spun();
        void on_reset_timeout_expired();
    } Spinner;

    class BonusMultiplerClass
    {
    public:
        byte value:3; // 1=1x, 2=2x..7=7x
        void init();
        void on_multipler_increased();
    } BonusMultipler;

    class ThreeBankTargetsClass
    {
    public:
        void init();
        void on_target_hit(byte switch_no);

    } ThreeBankTargets;

    class EightBankTargetsClass
    {
    public:
        byte object_made: 4; // 0=none, 1..9=1..9
        void init();
        void on_target_hit(byte switch_no);
    } EightBankTargets;
};

void CGameplay::start(byte mode)
{
    // start gameplay, in a specific mode.
    // e.g. EVA_HAPPY_MODE (unlimited balls)

    this->mode = mode;
    highest_player = 0;
    memset(player_info, 0, sizeof(player_info));

    this->prepare_game(0, 0);
    running = 1;

    LOG("Gameplay started in mode %d", mode);
}

void CGameplay::handle_event(Event& e)
{
    if (e.type == timeout_expired) {
        handle_timeout(e.number);
    } else if (e.type == switch_closed) {
        handle_switch_closed(e.number);
    }
}

void CGameplay::every_100_msecs_interrupt()
{
    if (running)
    {
        // remove one from each order of magnitude, add it to score
        BcdNum delta = BcdNum();
        for (byte nibble = 0; nibble < 8; nibble++)
        {
            if (temp_score.get_nibble(0)) {
                delta.zero();
                delta.set_nibble(nibble, 0x1);
                temp_score.subtract(delta);
                player_info[current_player].score.add(delta);
            }
        }

        // show on display
        ScoreDisplay.show_bcd_num(1, player_info[current_player].score);
    }
}

void CGameplay::handle_timeout(char timeout_no) {
    switch (timeout_no) {
        case TIMEOUT_BALL_NINE_MOVING_TARGET:
            // move to the next moving target
            // adjust lamps accordingly
            break;

        case TIMEOUT_WOW_MOVING_TARGET:
            // move to the next wow target
            // adjust lamps accordingly
            break;

        case TIMEOUT_SPECIAL_MOVING_TARGET:
            // move to the next moving special, skip no 1.
            // adjust lamps accordingly
            break;

        case TIMEOUT_RESET_SPINNER_VALUE:
            // make spinner_value zero,
            // adjust lamps accordingly
            break;

    }
}

void CGameplay::handle_switch_closed(char switch_no) {
    switch (switch_no) {
        case SW_LEFT_OUTLANE:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            if (left_outlane && next_object_to_make < 8)
                // spot objects 1-7
                collect_and_reset_loop_target_value();
            break;

        case SW_RIGHT_OUTLANE:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            if (right_outlane && next_object_to_make <= 7)
                // spot objects 1-7
                collect_and_reset_loop_target_value();
            break;

        case SW_LEFT_INLANE:
            temp_score.add_bcd(0x100);
            Audio.play(SOUND_FX_1);
            if (left_inlane)
                make_current_target_object();
            break;

        case SW_RIGHT_INLANE:
            temp_score.add_bcd(0x100);
            Audio.play(SOUND_FX_1);
            if (right_inlane)
                make_current_target_object();
            break;

        case SW_MAIN_POP_BUMPER:
            temp_score.add_bcd(0x100);
            Audio.play(SOUND_FX_1);
            break;

        case SW_TOP_POP_BUMPER:
            temp_score.add_bcd(0x100);
            Audio.play(SOUND_FX_1);
            if (top_pop_bumper && next_object_to_make <= 7)
                // spot objects 1-7
                make_current_target_object();
            break;

        case SW_TOP_BANK_LEFT_TARGET:
        case SW_TOP_BANK_CENTER_TARGET:
        case SW_TOP_BANK_RIGHT_TARGET:
            // depending on settings, outside or center target advances spinner.
            // if it's the middle target and wow is lit, collect it
            // when all 3 drop targets down, increase bonus multiplier
            // if multiplier entered 6x or 7x (depending on setting) start top banks WOW
            //

        case SW_RIGHT_BANK_LEFT_TARGET:
        case SW_RIGHT_BANK_CENTER_TARGET:
        case SW_RIGHT_BANK_RIGHT_TARGET:
            // when all 3 drop targets down, increase bonus multiplier

        case SW_LEFT_BANK_TARGET_1:
            on_left_bank_drop_target_down(1);
            break;

        case SW_LEFT_BANK_TARGET_2:
            on_left_bank_drop_target_down(2);
            break;

        case SW_LEFT_BANK_TARGET_3:
            on_left_bank_drop_target_down(3);
            break;

        case SW_LEFT_BANK_TARGET_4:
            on_left_bank_drop_target_down(4);
            break;

        case SW_LEFT_BANK_TARGET_5:
            on_left_bank_drop_target_down(5);
            break;

        case SW_LEFT_BANK_TARGET_6:
            on_left_bank_drop_target_down(6);
            break;

        case SW_LEFT_BANK_TARGET_7:
            on_left_bank_drop_target_down(7);
            break;

        case SW_LEFT_BANK_TARGET_8:
            on_left_bank_drop_target_down(8);
            break;

        case SW_LEFT_LANE_THIRD_BALL:
            if (SwitchMatrix.is_switch_closed(SW_LEFT_LANE_SECOND_BALL)) {
                if (next_object_to_make >= 6 && next_object_to_make <= 8)  // spot objects 6-8
                    make_current_target_object();
                // we should also release all balls for multiplay
            }
            break;

        case SW_LEFT_LANE_SECOND_BALL:
            if (SwitchMatrix.is_switch_closed(SW_LEFT_LANE_CAPTURED_BALL)) {
                if (next_object_to_make >= 6 && next_object_to_make <= 8) // spot objects 6-8
                    make_current_target_object();
            }
            break;

        case SW_LEFT_LANE_CAPTURED_BALL:
            if (next_object_to_make < 9) // spot objects 1-8
                make_current_target_object();
            // if not object 5 is made, we should start kicking this out...
            break;

        case SW_LEFT_LANE_EXIT:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            loop_target_value = ((loop_target_value + 1) % 5);
            Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, 0);
            break;

        case SW_SKILL_SHOT_TARGET:
            temp_score.add_bcd(0x7000);
            spinner_value = 4; // maximum
            // adjust lamps
            // should turn off the red arrow lamp? (in the original game it seems to be always on)
            break;

        case SW_SPINNER:
            temp_score.add_bcd(get_spinner_score_bcd(spinner_value));
            Animator.start(ANIM_SPINNER_COLLECT_VALUE, 0);
            TimeKeeper.start_timeout(TIMEOUT_RESET_SPINNER_VALUE, 1000);
            Audio.play(SOUND_SPINNER);
            break;

        case SW_TOP_LOOP_PASS:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            loop_target_value = ((loop_target_value + 1) % 5);
            Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, 0);
            break;

        case SW_TOP_LOOP_TARGET:
            collect_and_reset_loop_target_value();
            break;

        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:

        case SW_OUTHOLE_RIGHT:
            // play sad fx,
            // fall into state of assigning bonus,
            // etc

        case SW_START:
            // add user before ball leaves the shooter
            break;
    }
}

dword CGameplay::get_spinner_score_bcd(byte spinner_value) {
    if (spinner_value == 0) return 0x0100;
    if (spinner_value == 1) return 0x0400;
    if (spinner_value == 2) return 0x0900;
    if (spinner_value == 3) return 0x1600;
    if (spinner_value == 4) return 0x2500;
    return 0x100;
}

void CGameplay::collect_and_reset_loop_target_value() {
    dword score;

    if      (loop_target_value == 4) score = 0x173000;
    else if (loop_target_value == 3) score =  0x40000;
    else if (loop_target_value == 2) score =  0x30000;
    else if (loop_target_value == 1) score =  0x20000;
    else                             score =  0x10000;

    temp_score.add_bcd(score);
    Audio.play(SOUND_FX_2);
    Animator.start(ANIM_TOP_LOOP_COLLECT_VALUE, 0);
    loop_target_value = 0;
}

void CGameplay::make_current_target_object() {
    // update status, bonus, start animations, kick up the drop targets etc.

    // next_object_to_make: 3;  // 0-8 means 1-9. 9=wow, 10=special.
    // set the according target lamp to on.
    temp_score.add_bcd(0x1000);
    Audio.play(SOUND_FX_3);

}

void CGameplay::on_left_bank_drop_target_down(byte number) {
    // number is from 1..8, not 0-7
    // I think we must raise the 8 drop targets whenever target 8 is hit,
    // to avoid having a ball stuck on the gap that 8 leaves.
    // possibly make the current target object
    // if we are at 9, check if target is the same as the 9 moving target.
    // same for wow and special.
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
    waiting_for_ball_in_eject_lane = 1;
    collecting_bonuses = 0;

    Spinner.init();
    LoopTarget.init();
    BonusMultipler.init();
    ThreeBankTargets.init();
    EightBankTargets.init();
}



void CGameplay::LoopTargetClass::init()
{
    value = 1; // 10K

    LampMatrix.lamp_on(LAMP_LOOP_VALUE_10K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_20K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_30K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_40K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_173K);
}

void CGameplay::LoopTargetClass::on_passed_loop()
{
    // increase value
    value += 1;
    if (value > 5)
        value = 1;

    // start animation with the bitmap to display afterwards
    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    if (value >= 5) bitmap |= 0x10;
    Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, bitmap);

    // assign 3k
    temp_score.add_bcd(0x3000);

    // play an FX
    Audio.play(SOUND_FX_6);
}

void CGameplay::LoopTargetClass::on_target_hit()
{

}

void CGameplay::SpinnerClass::init()
{
}

void CGameplay::SpinnerClass::on_value_increased()
{
}

void CGameplay::SpinnerClass::on_spinner_spun()
{
}

void CGameplay::SpinnerClass::on_reset_timeout_expired()
{
}

void CGameplay::BonusMultiplerClass::init()
{
    value = 1; // 1x
    LampMatrix.lamp_on(LAMP_BONUS_MULTIPLIER_X1);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X2);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X4);
}

void CGameplay::BonusMultiplerClass::on_multipler_increased()
{
    value = (value + 1) & 0x7;
    // start animation
    Animator.start(ANIM_BONUS_MULTIPLIER, value);
}

void CGameplay::ThreeBankTargetsClass::init()
{
    // set lamps, status etc.
}

void CGameplay::ThreeBankTargetsClass::on_target_hit(byte switch_no)
{
    // if a whole bank is down (and settings allow it) increase bonus multiplier
    // if outside or inside targets (depending on settings) increase spinner value
}

void CGameplay::EightBankTargetsClass::init()
{
    // set object to make, set lamps etc.
}

void CGameplay::EightBankTargetsClass::on_target_hit(byte switch_no)
{
    // if object made move to the next.
    // if 8/9 was made (depending on the settings) start WOWs and Specials.
    // if 5 was made, start the ball keeping feature
    // etc.
}

