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


#define GAMEPLAY_MODE_NORMAL       0
#define GAMEPLAY_MODE_EVA_HAPPY    1



/**
 * firing eject coil, waiting for ball to enter in shooting lane and stay there.
 * start is allowed to be pressed here, to add a player.
 * this state ends when ball is shot through the lane.
 */
#define STATE_EJECTING_TO_SHOOTING_LANE  0
/**
 * happens as soon as the ball is leaving the base of the shooting lane.
 * stops only when (the last) ball enters outhole
 */   
#define STATE_GAME_RUNNING               1
/**
 * happens when the ball (last if multiball) enters outhole
 * if we have shoot-agains, we skip assigning bonus and go to shoot-again (preserving objects made)
 * otherwise, we collect any bonus, wait for score to be assigned,
 * then we clear objects made and go to next palyer and/or ball. 
 */
#define STATE_COLLECTING_BONUS           2


class CGameplay
{
public:
    
    void start(byte mode);
    void handle_event(Event& e);
    void every_100_msecs_interrupt();

private:
    int running: 1;               // whether we are running (or merely existing in memory)
    int mode: 2;                  // mode under which we are running
    int highest_player: 2;        // highest player number (0..3) (essentially num_players - 1)
    int current_player: 2;        // current player no
    int state: 6;
    
    struct player_info
    {
        int ball_number: 3;       // 1..5 shoot-agains don't increase this.
        BcdNum score;
    } player[4];             
    
    BcdNum temp_score;             // a place to add score that is slowly moved to player score and display.
    
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
	void handle_switch_closed(char switch_no);
	void handle_switch_opened(char switch_no);
	dword get_spinner_score_bcd(byte spinner_value);
	void collect_and_reset_loop_target_value();
	void make_current_target_object();
	void on_left_bank_drop_target_down(byte number);
};

void CGameplay::start(byte mode)
{
    // start gameplay, in a specific mode.
    // e.g. EVA_HAPPY_MODE (unlimited balls)
    // or   DIMITRIS_PRACTICE_MODE (if we have space for the code) 
}

void CGameplay::handle_event(Event& e)
{
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
                player[current_player].score.add(delta);
            }            
        }
        
        // show on display
        ScoreDisplay.display_bcd_num(1, player[current_player].score);
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

void CGameplay::handle_switch_opened(char switch_no) {
    switch (switch_no) {
        case SW_SHOOTING_LANE:
            // game on, no more players, start audio etc.
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

