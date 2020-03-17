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

class LoopTargetClass
{
public:
    CGameplay *gameplay;
    byte value: 4; // 0=10k, 1=20k, 2=30k, 3=40k, 4=173k
    byte left_outlane: 1;  // 1=on, 0=off
    byte right_outlane: 1; // 1=on, 0=off
    void init();
    void advance_value();
    void collect_value();
    void flip_lit_outlane();
};

class SpinnerClass
{
public:
    CGameplay *gameplay;
    byte value: 4; // 0=100, 1=400, 2=900, 3=1600, 4=2500
    void init();
    void advance_value();
    void advance_to_top_value();
    void on_spinner_spun();
    void on_reset_timeout_expired();
};

class BonusMultiplerClass
{
public:
    CGameplay *gameplay;
    byte value:3; // 1=1x, 2=2x..7=7x
    void init();
    void increase_multiplier();
};

class ThreeBankTargetsClass
{
public:
    CGameplay *gameplay;
    byte lit_center_target: 1; // 0=top, 1=right.
    byte lit_wow_target: 3;    // 0=none, 1..4 specific target.
    void init();
    void on_target_hit(byte switch_no);
    void flip_lit_7k_target();
    void move_lit_wow_target();
    void start_wow();
};

class EightBankTargetsClass
{
public:
    CGameplay *gameplay;
    byte object_made: 4; // 0=none, 1..9=1..9
    void init();
    void on_target_hit(byte switch_no);
};


class CGameplay
{
public:
    CGameplay();
    void start(byte mode);
    void handle_event(Event& e);
    void every_100_msecs_interrupt();

    void add_score_bcd(dword bcd);
    void add_shoot_again();

    SpinnerClass Spinner;
    LoopTargetClass LoopTarget;
    BonusMultiplerClass BonusMultiplier;
    ThreeBankTargetsClass ThreeBankTargets;
    EightBankTargetsClass EightBankTargets;

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
    Spinner.gameplay = this;
    LoopTarget.gameplay = this;
    BonusMultiplier.gameplay = this;
    ThreeBankTargets.gameplay = this;
    EightBankTargets.gameplay = this;
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
    switch (switch_no) {
        case SW_LEFT_OUTLANE:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            if (left_outlane && next_object_to_make < 8)
                // spot objects 1-7
                LoopTarget.collect_value();
            break;

        case SW_RIGHT_OUTLANE:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            if (right_outlane && next_object_to_make <= 7)
                // spot objects 1-7
                LoopTarget.collect_value();
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
            Spinner.on_spinner_spun();
            ThreeBankTargets.flip_lit_7k_target();
            LoopTarget.flip_lit_outlane();
            break;

        case SW_TOP_LOOP_PASS:
            temp_score.add_bcd(0x3000);
            Audio.play(SOUND_FX_1);
            loop_target_value = ((loop_target_value + 1) % 5);
            Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, 0);
            break;

        case SW_TOP_LOOP_TARGET:
            LoopTarget.collect_value();
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

void CGameplay::make_current_target_object() {
    // update status, bonus, start animations, kick up the drop targets etc.

    // next_object_to_make: 3;  // 0-8 means 1-9. 9=wow, 10=special.
    // set the according target lamp to on.
    temp_score.add_bcd(0x1000);
    Audio.play(SOUND_FX_3);

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
    BonusMultiplier.init();
    ThreeBankTargets.init();
    EightBankTargets.init();
}



void LoopTargetClass::init()
{
    value = 1; // 10K
    LampMatrix.lamp_on(LAMP_LOOP_VALUE_10K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_20K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_30K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_40K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_173K);
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

    gameplay->add_score_bcd(0x3000);
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
    gameplay->add_score_bcd(score);

    Audio.play(SOUND_FX_5);

    value = 1;
    Animator.start(ANIM_TOP_LOOP_COLLECT_VALUE, 0x01);
}

void LoopTargetClass::flip_lit_outlane()
{
    //
}

void SpinnerClass::init()
{
    value = 0; // 100
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);
}

void SpinnerClass::advance_to_top_value()
{
    gameplay->add_score_bcd(0x7000);
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
    gameplay->add_score_bcd(score);

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    Animator.start(ANIM_SPINNER_COLLECT_VALUE, bitmap);

    Audio.play(SOUND_SPINNER);
    TimeKeeper.start_timeout(TIMEOUT_RESET_SPINNER_VALUE, 2000);
}

void SpinnerClass::on_reset_timeout_expired()
{
    // no frills this time.
    value = 0;
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);
}

void BonusMultiplerClass::init()
{
    value = 1; // 1x
    LampMatrix.lamp_on(LAMP_BONUS_MULTIPLIER_X1);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X2);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X4);
}

void BonusMultiplerClass::increase_multiplier()
{
    value = (value + 1) & 0x7;
    Animator.start(ANIM_BONUS_MULTIPLIER, value);
    Audio.play(SOUND_FX_3);

    if ((value == 6 && GameSettings.three_bank_wow_turn_on == 1) ||
        (value == 7))
        gameplay->ThreeBankTargets.start_wow();
}

void ThreeBankTargetsClass::init()
{
    lit_center_target = 0;

    // set lamps, status etc.
    LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, (lit_center_target == 0));
    LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);

    LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, (lit_center_target == 1));
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

void ThreeBankTargetsClass::flip_lit_7k_target()
{
    // called at least when spinner is spinning
    lit_center_target ^= 1;

    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, lit_center_target == 0);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, lit_center_target == 1);
}

void ThreeBankTargetsClass::move_lit_wow_target()
{
    // maybe on 1sec timer

    // moving to the next
    lit_wow_target += 1;
    if (lit_wow_target > 4)
        lit_wow_target = 1; // remember 0=off

    LampMatrix.set_lamp(LAMP_TOP_BANK_LEFT_TARGET, lit_wow_target == 1);
    LampMatrix.set_lamp(LAMP_TOP_BANK_RIGHT_TARGET, lit_wow_target == 2);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_LEFT_TARGET, lit_wow_target == 3);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_RIGHT_TARGET, lit_wow_target == 4);
}

void ThreeBankTargetsClass::on_target_hit(byte switch_no)
{
    // see if a wow is awarded
    if ((lit_wow_target == 1 && switch_no == SW_TOP_BANK_LEFT_TARGET) ||
        (lit_wow_target == 2 && switch_no == SW_TOP_BANK_RIGHT_TARGET) ||
        (lit_wow_target == 3 && switch_no == SW_RIGHT_BANK_LEFT_TARGET) ||
        (lit_wow_target == 4 && switch_no == SW_RIGHT_BANK_RIGHT_TARGET)) {
        if (GameSettings.wow_award_type == 0) {
            gameplay->add_score_bcd(0x7000);
        } else if (GameSettings.wow_award_type == 1) {
            gameplay->add_shoot_again();
        }

        // we could have an animation of it blinking for a while.

        Audio.play(SOUND_FX_4);

        lit_wow_target = 0;
        LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_RIGHT_TARGET);
    }

    // see if we have a 7K award
    if ((lit_center_target == 0 && switch_no == SW_TOP_BANK_CENTER_TARGET) ||
        (lit_center_target == 1 && switch_no == SW_RIGHT_BANK_CENTER_TARGET)) {
        gameplay->add_score_bcd(0x7000);
        // we could have an animation of it blinking for a while.
        Audio.play(SOUND_FAST_PHASERS);
    }

    if (GameSettings.spinner_value_advancement == 1) { // outer targets
        if (switch_no == SW_TOP_BANK_LEFT_TARGET ||
            switch_no == SW_TOP_BANK_RIGHT_TARGET ||
            switch_no == SW_RIGHT_BANK_LEFT_TARGET ||
            switch_no == SW_RIGHT_BANK_RIGHT_TARGET) {
            gameplay->Spinner.advance_value();
        }
    } else { // center targets
        if (switch_no == SW_TOP_BANK_CENTER_TARGET ||
            switch_no == SW_RIGHT_BANK_CENTER_TARGET) {
            gameplay->Spinner.advance_value();
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
            gameplay->BonusMultiplier.increase_multiplier();
        }
    }
    else if (GameSettings.multiplier_step_up == 1) { // both 3-banks
        if (top_bank_down || right_bank_down) {
            gameplay->BonusMultiplier.increase_multiplier();
        }
    }

    // see if we reset a bank

}



void EightBankTargetsClass::init()
{
    // set object to make, set lamps etc.
}

void EightBankTargetsClass::on_target_hit(byte switch_no)
{
    // if object made move to the next.
    // if 8/9 was made (depending on the settings) start WOWs and Specials.
    // if 5 was made, start the ball keeping feature
    // etc.
}

