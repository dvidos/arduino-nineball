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

void start_animation(int animation_no);
void start_timeout(int timeout_no);
void start_coil(int coil_no);
void add_score(word bcd_amount_in_tens);

class Gameplay
{
public:
    
    void init(byte mode);
    void tick();

private:
            
    int loop_target_value: 3;      // 0..4 for 10k, 20k, 30k, 40k, 173k
    int spinner_value: 3;          // 0..4 for 100, 400, 900, 1600, 2500.
    int bonus_multiplier: 3;       // 1..7 for x1 .. x7 (minimum is 1, not zero)
    int next_object_to_make: 3;    // 0-8 means 1-9. 9=wow, 10=special.
    int current_moving_target: 3;  // moving target for "9", WOW, Special.
    int left_outlane: 1;           // collects loop pass value if lit
    int right_outlane: 1;          // collects loop pass value if lit
    int left_inlane: 1;            // spots current target when lit
    int right_inlane: 1;           // spots current target when lit
    int top_pop_bumper: 1;         // spots current target when lit
    
    int extra_balls_awarded: 2;    // up to 3 extra balls can be awarded. shoot again will lite for them.
    int one_special_achieved: 1;   // whether one special is achieved, to avoid giving more, if not allowed.
    
    void handle_timeout(char timeout_no);	
	void handle_animation_finished(char animation_no);
	void handle_switch_closed(char switch_no);
	void handle_switch_opened(char switch_no);
	word get_spinner_score_bcd(byte spinner_value);
	void collect_and_reset_loop_target_value();
	void make_current_target_object();
	void on_left_bank_drop_target_down(byte number);
};

void Gameplay::init(byte mode)
{
    // start gameplay, in a specific mode.
    // e.g. EVA_HAPPY_MODE (unlimited balls)
    // or   DIMITRIS_PRACTICE_MODE (if we have space for the code) 
}

void Gameplay::tick()
{
}

    

void Gameplay::handle_timeout(char timeout_no) {
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

void Gameplay::handle_animation_finished(char animation_no) {
    switch (animation_no) {
        case ANIM_TOP_LOOP_ADVANCE_VALUE:
            // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
            // set the lamps to the appropriate top loop score
        case ANIM_TOP_LOOP_COLLECT_VALUE:
           // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
            // set the lamps to the appropriate top loop score
        case ANIM_BONUS_MULTIPLIER:
            // fast blink of the new value for 8 times.
            // set the lamps to the appropriate bonus multiplier
        case ANIM_SPINNER_INCREASE_VALUE:
            // on the original game, there is no animation for increasing the value of the spinner
            // set the lamps to the approrpiate spinner value
        case ANIM_SPINNER_COLLECT_VALUE:
            // moderate chase of one lamp each time, through the lamps that were already lit.
            // set the lamps to the approrpiate spinner value
        case ANIM_BALLS_DIAMOND:
            // set the lamps to the appropriate objects made
            break;
    }
}

void Gameplay::handle_switch_closed(char switch_no) {
    switch (switch_no) {
        case SW_LEFT_OUTLANE:
            add_score(0x0300);
            Audio.play(SOUND_FX_1);
            if (left_outlane && next_object_to_make < 8)
                // spot objects 1-7
                collect_and_reset_loop_target_value();
            break;
       
        case SW_RIGHT_OUTLANE:
            add_score(0x0300);
            Audio.play(SOUND_FX_1);
            if (right_outlane && next_object_to_make <= 7)
                // spot objects 1-7
                collect_and_reset_loop_target_value();
            break;
       
        case SW_LEFT_INLANE:
            add_score(0x010); // 100
            Audio.play(SOUND_FX_1);
            if (left_inlane)
                make_current_target_object();
            break;
            
        case SW_RIGHT_INLANE:
            add_score(0x010); // 100
            Audio.play(SOUND_FX_1);
            if (right_inlane)
                make_current_target_object();
            break;
        
        case SW_MAIN_POP_BUMPER:
            add_score(0x0010); // 100
            Audio.play(SOUND_FX_1);
            break;
            
        case SW_TOP_POP_BUMPER:
            add_score(0x0010); // 100
            Audio.play(SOUND_FX_1);
            if (top_pop_bumper && next_object_to_make <= 7)
                // spot objects 1-7
                make_current_target_object();
            break;
    
        case SW_TOP_BANK_LEFT_TARGET:
        case SW_TOP_BANK_CENTER_TARGET:
        case SW_TOP_BANK_RIGHT_TARGET:
            // when all 3 drop targets down, increase bonus multiplier
        
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
            add_score(0x0300);
            Audio.play(SOUND_FX_1);
            loop_target_value = ((loop_target_value + 1) % 5);
            start_animation(ANIM_TOP_LOOP_ADVANCE_VALUE);
            break;
    
        case SW_SKILL_SHOT_TARGET:
            add_score(0x700);
            spinner_value = 4; // maximum
            // adjust lamps
            // should turn off the red arrow lamp? (in the original game it seems to be always on)
            break;
        
        case SW_SPINNER:
            add_score(get_spinner_score_bcd(spinner_value));
            start_animation(ANIM_SPINNER_COLLECT_VALUE);
            start_timeout(TIMEOUT_RESET_SPINNER_VALUE); // one second?
            Audio.play(SOUND_SPINNER);
            break;
        
        case SW_TOP_LOOP_PASS:
            add_score(0x0300);
            Audio.play(SOUND_FX_1);
            loop_target_value = ((loop_target_value + 1) % 5);
            start_animation(ANIM_TOP_LOOP_ADVANCE_VALUE);
            break;
        
        case SW_TOP_LOOP_TARGET:
            collect_and_reset_loop_target_value();
            break;
        
        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:
        
        case SW_DRAIN_HOLE:
            // play sad fx, 
            // fall into state of assigning bonus,
            // etc 
        
        case SW_START:
            // add user before ball leaves the shooter
            break;
    }
}

void Gameplay::handle_switch_opened(char switch_no) {
    switch (switch_no) {
        case SW_SHOOTING_LANE:
            // game on, no more players, start audio etc.
            break;
    }
}

word Gameplay::get_spinner_score_bcd(byte spinner_value) {
    if (spinner_value == 0) return 0x0010; //   100
    if (spinner_value == 1) return 0x0040; //   400
    if (spinner_value == 2) return 0x0090; //   900
    if (spinner_value == 3) return 0x0160; // 1,600
    if (spinner_value == 4) return 0x0250; // 2,500
    return 0x0010; // 100
}

void Gameplay::collect_and_reset_loop_target_value() {
    word score;
    
    if (loop_target_value == 4) {
        // as I want to keep the bcd to tens, 173,000 will be 90,000 + 83,000
        add_score(0x9000); // 90,000
        score = 0x8300; // 83,000
    }
    else if (loop_target_value == 3) score = 0x4000;   //  40,000
    else if (loop_target_value == 2) score = 0x3000;   //  30,000
    else if (loop_target_value == 1) score = 0x2000;   //  20,000
    else score = 0x1000; // 10,000    
    
    add_score(score);
    Audio.play(SOUND_FX_2);
    start_animation(ANIM_TOP_LOOP_COLLECT_VALUE);
    loop_target_value = 0;
}

void Gameplay::make_current_target_object() {
    // update status, bonus, start animations, kick up the drop targets etc.
    
    // next_object_to_make: 3;  // 0-8 means 1-9. 9=wow, 10=special.
    // set the according target lamp to on.
    add_score(0x0100); // 1K
    Audio.play(SOUND_FX_3);
    
}

void Gameplay::on_left_bank_drop_target_down(byte number) {
    // number is from 1..8, not 0-7
    // I think we must raise the 8 drop targets whenever target 8 is hit,
    // to avoid having a ball stuck on the gap that 8 leaves.
    // possibly make the current target object
    // if we are at 9, check if target is the same as the 9 moving target.
    // same for wow and special.
}
