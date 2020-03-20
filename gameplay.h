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
extern CGameSettings GameSettings;

class CGameplay;
extern CGameplay Gameplay;


class LoopTargetClass
{
public:
    byte value: 4; // 0=10k, 1=20k, 2=30k, 3=40k, 4=173k
    byte left_outlane: 1;  // 1=on, 0=off
    byte right_outlane: 1; // 1=on, 0=off
    void init();
    void advance_value();
    void collect_value();
    void enable_or_flip_outlanes();
    void on_passed_outlane(byte switch_no);
    static void turn_off_outlanes();
};

class SpinnerClass
{
public:
    byte value: 4; // 0=100, 1=400, 2=900, 3=1600, 4=2500
    void init();
    void advance_value();
    void advance_to_top_value();
    void on_spinner_spun();
    static void reset_spinner_value();
};

class BonusMultiplierClass
{
public:
    byte value: 3; // 1=1x, 2=2x..7=7x
    void init(bool was_6_or_higher);
    void increase_multiplier();
};

class ThreeBankTargetsClass
{
public:
    byte lit_pink_target: 2; // 0=off, 1=top, 2=right.
    byte lit_wow_target: 3;    // 0=none, 1,2=top ones, 3,4 right ones
    void init();
    void on_target_hit(byte switch_no);
    void enable_or_flip_pink_target();
    static void turn_off_pink_target();
    void start_wow();
    static void move_lit_wow_target();
};

class EightBankTargetsClass
{
public:
    byte object_made: 4;          // 0=none, 1..9=1..9
    byte number_nine_number: 4;   // 0=none, 1..8=1..9
    byte wow_number: 4;           // 0=none, 1..8=1..8
    byte special_number: 4;       // 0=none, 2..8=2..8 (1 is missing)
    byte spot_number_enabled: 1;  // this is a timed mode
    byte special_made: 1;         // to allow at most one
    byte super_bonus_made: 1;
    byte sequence[8] = { 2, 4, 6, 8, 7, 5, 3, 1 };

    void init();
    void on_target_hit(byte switch_no);

    void award_current_number();
    void award_number_nine();
    void award_wow();
    void award_special();

    void start_number_nine_sequence();
    void start_wow_sequence();
    void start_special_sequence();
    void start_spot_number_timeout();

    static void spot_number_timed_out();
    static void advance_number_nine_target();
    static void advance_wow_target();
    static void advance_special_target();
};

class BallKeeperClass
{
public:
    byte shoot_agains: 2; // up to 3 shoot agains may be awarded
    byte ball_capture_enabled: 1; // the yellow arrow light

    void init();
    void enable_ball_capturing();
    void on_switch_closed(byte switch_no);
    bool has_captured_balls();
    bool are_all_balls_in_outhole();
    void grant_shoot_again();
    void consume_shoot_again();
    void eject_outhole_ball();       // get ball in shooting lane
    void release_captured_balls(); // eject all captured balls
    static void eject_outhole_timeout_expired();     // to see if successul
    static void release_captured_timeout_expired();  // to see if other balls still there
};



LoopTargetClass LoopTarget;
SpinnerClass Spinner;
BonusMultiplierClass BonusMultiplier;
ThreeBankTargetsClass ThreeBankTargets;
EightBankTargetsClass EightBankTargets;
BallKeeperClass BallKeeper;


class CGameplay
{
public:
    CGameplay();
    void start(byte mode);
    void handle_event(Event& e);
    void every_100_msecs_interrupt();

    void add_score_bcd(dword bcd);
    void add_shoot_again();

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
    void make_current_target_object();
};

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

void CGameplay::add_score_bcd(dword bcd)
{
    temp_score.add_bcd(bcd);
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

        // -------------------------------------------
        //  Things "written" above this line.
        // -------------------------------------------






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


        case SW_LEFT_BANK_TARGET_1:
            EightBankTargets.on_target_hit(1);
            break;

        case SW_LEFT_BANK_TARGET_2:
            EightBankTargets.on_target_hit(2);
            break;

        case SW_LEFT_BANK_TARGET_3:
            EightBankTargets.on_target_hit(3);
            break;

        case SW_LEFT_BANK_TARGET_4:
            EightBankTargets.on_target_hit(4);
            break;

        case SW_LEFT_BANK_TARGET_5:
            EightBankTargets.on_target_hit(5);
            break;

        case SW_LEFT_BANK_TARGET_6:
            EightBankTargets.on_target_hit(6);
            break;

        case SW_LEFT_BANK_TARGET_7:
            EightBankTargets.on_target_hit(7);
            break;

        case SW_LEFT_BANK_TARGET_8:
            EightBankTargets.on_target_hit(8);
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
    BonusMultiplier.init(false);
    ThreeBankTargets.init();
    EightBankTargets.init();
}

void LoopTargetClass::init()
{
    value = 1; // 10K
    left_outlane = 0;
    right_outlane = 0;

    LampMatrix.lamp_on(LAMP_LOOP_VALUE_10K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_20K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_30K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_40K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_173K);
    LampMatrix.lamp_off(LAMP_LEFT_OUTLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_OUTLANE);
}

void LoopTargetClass::advance_value()
{
    // increase value
    value += 1;
    if (value > 5)
        value = 1; // loop target increase wraps around

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    if (value >= 5) bitmap |= 0x10;
    Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, bitmap);

    Gameplay.add_score_bcd(0x3000);
    Audio.play(SOUND_FX_6);
}

void LoopTargetClass::collect_value()
{
    dword score = 0;
    if      (value == 5) score = 0x173000ul;
    else if (value == 4) score =  0x40000ul;
    else if (value == 3) score =  0x30000ul;
    else if (value == 2) score =  0x20000ul;
    else if (value == 1) score =  0x10000ul;
    Gameplay.add_score_bcd(score);

    Audio.play(SOUND_FX_5);

    value = 1;
    Animator.start(ANIM_TOP_LOOP_COLLECT_VALUE, 0x01);
}

void LoopTargetClass::enable_or_flip_outlanes()
{
    left_outlane ^= 1;
    right_outlane ^= 1;

    // if both on, select one
    if (left_outlane && right_outlane)
        right_outlane = false;

    LampMatrix.set_lamp(LAMP_LEFT_OUTLANE, left_outlane);
    LampMatrix.set_lamp(LAMP_RIGHT_OUTLANE, right_outlane);

    TimeKeeper.callback_later(turn_off_outlanes, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void LoopTargetClass::on_passed_outlane(byte switch_no)
{
    Gameplay.add_score_bcd(0x3000);

    if ((switch_no == SW_LEFT_OUTLANE && left_outlane) ||
        (switch_no == SW_RIGHT_OUTLANE && right_outlane))
        collect_value();
}

void LoopTargetClass::turn_off_outlanes()
{
    // remember this is a static function called by an interrupt
    LoopTarget.left_outlane = 0;
    LoopTarget.right_outlane = 0;

    LampMatrix.lamp_off(LAMP_LEFT_OUTLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_OUTLANE);
}

void SpinnerClass::init()
{
    value = 0; // 100
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);
    LampMatrix.lamp_on(LAMP_SPINNER_TOP_VALUE);
}

void SpinnerClass::advance_to_top_value()
{
    LampMatrix.lamp_off(LAMP_SPINNER_TOP_VALUE);
    Gameplay.add_score_bcd(0x7000);
    value = 4; // 2500
    Animator.start(ANIM_SPINNER_INCREASE_VALUE, 0x0F);
    Audio.play(SOUND_FX_5);
}

void SpinnerClass::advance_value()
{
    if (value >= 4) // we don't reset like loop target
        return;

    byte bitmap;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    Animator.start(ANIM_SPINNER_INCREASE_VALUE, bitmap);

    Audio.play(SOUND_FX_5);
}

void SpinnerClass::on_spinner_spun()
{
    dword score = 0x100;
    if      (value == 4) score = 0x2500ul;
    else if (value == 3) score = 0x1600ul;
    else if (value == 2) score = 0x0900ul;
    else if (value == 1) score = 0x0400ul;
    Gameplay.add_score_bcd(score);

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    Animator.start(ANIM_SPINNER_COLLECT_VALUE, bitmap);

    Audio.play(SOUND_SPINNER);
    TimeKeeper.callback_later(reset_spinner_value, 2000);

    ThreeBankTargets.enable_or_flip_pink_target();
    LoopTarget.enable_or_flip_outlanes();
}

void SpinnerClass::reset_spinner_value()
{
    // remember this is a static function called by an interrupt
    Spinner.value = 0;
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);
}

void BonusMultiplierClass::init(bool was_6_or_higher)
{
    value = was_6_or_higher ? 2 : 1; // 1x or 2x if previous was at least 6x.
    LampMatrix.lamp_on(LAMP_BONUS_MULTIPLIER_X1);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X2);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X4);
}

void BonusMultiplierClass::increase_multiplier()
{
    value = (value + 1) & 0x7;
    Animator.start(ANIM_BONUS_MULTIPLIER, value);
    Audio.play(SOUND_FX_3);

    if ((value == 6 && GameSettings.three_bank_wow_turn_on == 1) ||
        (value == 7))
        ThreeBankTargets.start_wow();
}

void ThreeBankTargetsClass::init()
{
    lit_pink_target = 0;
    lit_wow_target = 0;

    // set lamps, status etc.
    LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, (lit_pink_target == 1));
    LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);

    LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, (lit_pink_target == 2));
    LampMatrix.lamp_off(LAMP_RIGHT_BANK_RIGHT_TARGET);

    if (SwitchMatrix.is_switch_closed(SW_TOP_BANK_LEFT_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_CENTER_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_RIGHT_TARGET)) {
        // we hope we will not need retries
        Coils.fire_top_bank_reset();
    }

    if (SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_LEFT_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_CENTER_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_RIGHT_TARGET)) {
        // we hope we will not need retries
        Coils.fire_right_bank_reset();
    }
}

void ThreeBankTargetsClass::enable_or_flip_pink_target()
{
    // called at least when spinner is spinning
    lit_pink_target += 1;
    if (lit_pink_target >= 3)
        lit_pink_target = 1;

    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, lit_pink_target == 1);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, lit_pink_target == 2);

    TimeKeeper.callback_later(turn_off_pink_target, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void ThreeBankTargetsClass::turn_off_pink_target()
{
    // remember this is a static function
    ThreeBankTargets.lit_pink_target = 0;

    LampMatrix.lamp_off(LAMP_TOP_BANK_CENTER_TARGET);
    LampMatrix.lamp_off(LAMP_RIGHT_BANK_CENTER_TARGET);
}

void ThreeBankTargetsClass::start_wow()
{
    lit_wow_target = 1;

    LampMatrix.set_lamp(LAMP_TOP_BANK_LEFT_TARGET, lit_wow_target == 1);
    LampMatrix.set_lamp(LAMP_TOP_BANK_RIGHT_TARGET, lit_wow_target == 2);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_LEFT_TARGET, lit_wow_target == 3);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_RIGHT_TARGET, lit_wow_target == 4);

    TimeKeeper.callback_later(move_lit_wow_target, 1000);
}

void ThreeBankTargetsClass::move_lit_wow_target()
{
    // remember this is a static function
    if (ThreeBankTargets.lit_wow_target == 0) // maybe it was made while we were waiting.
        return;

    // moving to the next
    ThreeBankTargets.lit_wow_target += 1;
    if (ThreeBankTargets.lit_wow_target > 4)
        ThreeBankTargets.lit_wow_target = 1; // remember 0=off

    LampMatrix.set_lamp(LAMP_TOP_BANK_LEFT_TARGET, ThreeBankTargets.lit_wow_target == 1);
    LampMatrix.set_lamp(LAMP_TOP_BANK_RIGHT_TARGET, ThreeBankTargets.lit_wow_target == 2);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_LEFT_TARGET, ThreeBankTargets.lit_wow_target == 3);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_RIGHT_TARGET, ThreeBankTargets.lit_wow_target == 4);

    TimeKeeper.callback_later(move_lit_wow_target, 1000);
}

void ThreeBankTargetsClass::on_target_hit(byte switch_no)
{
    // see if a wow is awarded
    if ((lit_wow_target == 1 && switch_no == SW_TOP_BANK_LEFT_TARGET) ||
        (lit_wow_target == 2 && switch_no == SW_TOP_BANK_RIGHT_TARGET) ||
        (lit_wow_target == 3 && switch_no == SW_RIGHT_BANK_LEFT_TARGET) ||
        (lit_wow_target == 4 && switch_no == SW_RIGHT_BANK_RIGHT_TARGET)) {
        if (GameSettings.wow_award_type == 0) {
            Gameplay.add_score_bcd(0x7000);
        } else if (GameSettings.wow_award_type == 1) {
            Gameplay.add_shoot_again();
        }

        // we could have an animation of it blinking for a while.

        Audio.play(SOUND_FX_4);

        lit_wow_target = 0;
        LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_RIGHT_TARGET);
    }

    // "each target scores 1,000 or 7,000 when pink lite is lit"
    if ((lit_pink_target == 1 && switch_no == SW_TOP_BANK_CENTER_TARGET) ||
        (lit_pink_target == 2 && switch_no == SW_RIGHT_BANK_CENTER_TARGET)) {
        // we could have an animation of it blinking for a while.
        Gameplay.add_score_bcd(0x7000);
    } else {
        Gameplay.add_score_bcd(0x1000);
    }
    Audio.play(SOUND_FAST_PHASERS);


    if (GameSettings.spinner_value_advancement == 1) { // outer targets
        if (switch_no == SW_TOP_BANK_LEFT_TARGET ||
            switch_no == SW_TOP_BANK_RIGHT_TARGET ||
            switch_no == SW_RIGHT_BANK_LEFT_TARGET ||
            switch_no == SW_RIGHT_BANK_RIGHT_TARGET) {
            Spinner.advance_value();
        }
    } else { // center targets
        if (switch_no == SW_TOP_BANK_CENTER_TARGET ||
            switch_no == SW_RIGHT_BANK_CENTER_TARGET) {
            Spinner.advance_value();
        }
    }

    // see if we up bonus multiplier
    byte top_bank_down =
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_LEFT_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_CENTER_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_RIGHT_TARGET);

    byte right_bank_down =
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_LEFT_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_CENTER_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_RIGHT_TARGET);

    if (GameSettings.multiplier_step_up == 0) { // both 3-banks
        if (top_bank_down && right_bank_down) {
            BonusMultiplier.increase_multiplier();
        }
    }
    else if (GameSettings.multiplier_step_up == 1) { // both 3-banks
        if (top_bank_down || right_bank_down) {
            BonusMultiplier.increase_multiplier();
        }
    }

    // "" 3 banks reset only after all targets in both 3 banks have been hit.
    // when "multiplier_step_up" is 1, only one 3 bank resets at random.
    // when "multiplier_step_up" is 0, both 3 banks reset.

    if (top_bank_down && right_bank_down) {
        if (GameSettings.multiplier_step_up == 0) { // both 3-banks
            // if there is loss of power to reset both banks together,
            // we can change our strategy
            Coils.fire_top_bank_reset();
            Coils.fire_right_bank_reset();
        } else { // only one bank resets at random
            if (random(0, 100) >= 50)
                Coils.fire_top_bank_reset();
            else
                Coils.fire_right_bank_reset();
        }
    }
}

void EightBankTargetsClass::init()
{
    object_made = 0;
    wow_number = 0;
    special_number = 0;
    spot_number_enabled = 0;
    special_made = 0;
    super_bonus_made = 0;

    LampMatrix.lamp_off(LAMP_BONUS_MADE_1);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_2);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_3);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_4);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_5);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_6);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_7);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_8);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_9);

    LampMatrix.lamp_on(LAMP_OBJECT_1_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_2_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_3_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_4_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_5_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_6_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_7_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_8_DROP_TARGET);

    LampMatrix.lamp_off(LAMP_OBJECT_1_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_2_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_3_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_4_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_5_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_6_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_7_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_8_WOW);

    LampMatrix.lamp_off(LAMP_OBJECT_2_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_3_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_4_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_5_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_6_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_7_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_8_SPECIAL);

    LampMatrix.lamp_off(LAMP_OBJECT_9);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
    LampMatrix.lamp_off(LAMP_SUPER_BONUS_77K);
    LampMatrix.lamp_off(LAMP_ALL_TARGETS_DOWN_SPECIAL);

    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);
}

void EightBankTargetsClass::on_target_hit(byte switch_no)
{
    // let's deduce the number hit, so we can programmatically
    // calculate things without a ton of conditions
    byte target_no;
    byte target_switches[] = {
        SW_LEFT_BANK_TARGET_1,
        SW_LEFT_BANK_TARGET_2,
        SW_LEFT_BANK_TARGET_3,
        SW_LEFT_BANK_TARGET_4,
        SW_LEFT_BANK_TARGET_5,
        SW_LEFT_BANK_TARGET_6,
        SW_LEFT_BANK_TARGET_7,
        SW_LEFT_BANK_TARGET_8,
    };
    for (target_no = 0; target_no < 8; target_no++) {
        if (switch_no == target_switches[target_no])
            break;
    }
    target_no++; // convert from 0..7 into 1..8, if not found it'll be 9.

    if (target_no <= 8)
    {
        if (target_no == special_number) {
            // award special
            Audio.play(SOUND_FX_2);
            if (GameSettings.special_award_type == 0) {
                Gameplay.add_score_bcd(0x90000);
            } else if (GameSettings.special_award_type == 1) {
                Gameplay.add_score_bcd(0x130000);
            } else if (GameSettings.special_award_type == 2) {
                Gameplay.add_shoot_again();
            }
            special_made = 1;

            // we need to see if special will launch again.
            if (GameSettings.unlimited_specials) {
                // we launch again.
            }
        } else if (target_no == wow_number) {
            // award wow
            Audio.play(SOUND_FX_2);
            if (GameSettings.wow_award_type == 0) {
                Gameplay.add_score_bcd(0x70000);
            } else if (GameSettings.wow_award_type == 1) {
                Gameplay.add_shoot_again();
            }
        } else if (target_no == number_nine_number) {
            // award No 9
            // grant No 9.
            Audio.play(SOUND_FX_6);
            object_made = 9;
            // possibly kick off wow and special
            // possibly turn on the 77K bonus
        } else if (target_no == object_made + 1) {
            // award No target
            Gameplay.add_score_bcd(0x1000);
            object_made++;
            if (object_made == 8) {
                // kick off No 9,
                // possibly kick off wow and special
                // possibly turn on the 77K bonus
            }
        } else {
            // he just hit a drop target.
        }
    }
    else
    {
        // handle other switches, bumper, inlanes...
        // maybe make current number
    }

    // if object made move to the next.
    // if 8/9 was made (depending on the settings) start WOWs and Specials.
    // if 5 was made, start the ball keeping feature
    // etc.

    if (object_made >= 5)
        BallKeeper.enable_ball_capturing();

    // see if we need to reset the switches
    // if No 8 is dropped, we definitely need to .
    // if all are down we need as well.
    // when else? when the next number is not down?
}

void EightBankTargetsClass::award_current_number()
{
}

void EightBankTargetsClass::award_number_nine()
{
}

void EightBankTargetsClass::award_wow()
{
}

void EightBankTargetsClass::award_special()
{
}

void EightBankTargetsClass::start_number_nine_sequence()
{
}


void EightBankTargetsClass::start_wow_sequence()
{
}

void EightBankTargetsClass::start_special_sequence()
{
}

void EightBankTargetsClass::start_spot_number_timeout()
{
    // "" slingshots, spinner and pop bumpers control
    // the percentage of time dead bumper, two return lanes, two outlanes
    // and 3 bank lites are on.

    // start of extend the spot number timeout.
    spot_number_enabled = 1;

    LampMatrix.lamp_on(LAMP_LEFT_INLANE);
    LampMatrix.lamp_on(LAMP_RIGHT_INLANE);
    LampMatrix.lamp_on(LAMP_TOP_POP_BUMPER);

    TimeKeeper.callback_later(spot_number_timed_out, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void EightBankTargetsClass::spot_number_timed_out()
{
    // static function to turn off pop bumper etc.
    EightBankTargets.spot_number_enabled = false;
    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
}

void EightBankTargetsClass::advance_number_nine_target()
{
    // static function to advance nine target
}

void EightBankTargetsClass::advance_wow_target()
{
    // static function to advance wow target
}

void EightBankTargetsClass::advance_special_target()
{
    // static function to advance the special target
}


