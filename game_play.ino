
/*
    the initial idea was to have a state machine,
    switching actions based on context and events that happened.
    the various timed actions (timeouts, switches detection) are performed in interrupts, so they have to be fast.
    therefore we will set flags from the interrupts and handle the events from the main loop.
    
    the main logic is to respond to events and kick off coils, sounds, lamps, scores, etc. 
    
    adding score is done with an unsigned int, in BCD, in tens. That means that 3,000 is encoded in 0x0300.
    also, adding score is done on a temporary buffer and a background process adds it to the real score, in thousands
    this is to simulate slowly giving the user points and make it more enjoyable.
    
*/



#define TIMEOUT_BALL_NINE_MOVING_TARGET  1    // move to the next moving target
#define TIMEOUT_WOW_MOVING_TARGET        2    // move to the next moving target, skip no 1.
#define TIMEOUT_RESET_SPINNER_VALUE      3    // spinner stopped spinning, reset its value

#define ANIM_TOP_LOOP_ADVANCE_VALUE   1    // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
#define ANIM_TOP_LOOP_COLLECT_VALUE   2    // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
#define ANIM_BONUS_MULTIPLIER         3    // fast blink of the new value for 8 times.
#define ANIM_SPINNER_INCREASE_VALUE   4    // on the original game, there is no animation for increasing the value of the spinner
#define ANIM_SPINNER_COLLECT_VALUE    5    // moderate chase of one lamp each time, through the lamps that were already lit.
#define ANIM_BALLS_DIAMOND            6    // on the original game it seems odd/even blink pretty fast


#define SW_LEFT_OUTLANE                1
#define SW_RIGHT_OUTLANE               2
#define SW_LEFT_INLANE                 3
#define SW_RIGHT_INLANE                4
#define SW_MAIN_POP_BUMPER             5
#define SW_TOP_POP_BUMPER              6
#define SW_TOP_BANK_LEFT_TARGET        7
#define SW_TOP_BANK_CENTER_TARGET      8
#define SW_TOP_BANK_RIGHT_TARGET       9
#define SW_RIGHT_BANK_LEFT_TARGET     10
#define SW_RIGHT_BANK_CENTER_TARGET   11
#define SW_RIGHT_BANK_RIGHT_TARGET    12
#define SW_LEFT_BANK_TARGET_1         13
#define SW_LEFT_BANK_TARGET_2         14
#define SW_LEFT_BANK_TARGET_3         15
#define SW_LEFT_BANK_TARGET_4         16
#define SW_LEFT_BANK_TARGET_5         17
#define SW_LEFT_BANK_TARGET_6         18
#define SW_LEFT_BANK_TARGET_7         19
#define SW_LEFT_BANK_TARGET_8         20
#define SW_LEFT_LANE_CAPTURE_FIRST_BALL    21
#define SW_LEFT_LANE_CAPTURE_SECOND_BALL   22
#define SW_LEFT_LANE_CAPTURE_THIRD_BALL    23
#define SW_LEFT_LANE_EXIT             24
#define SW_SKILL_SHOT_TARGET          25
#define SW_SPINNER                    26
#define SW_TOP_LOOP_PASS              27
#define SW_TOP_LOOP_TARGET            28
#define SW_DRAIN_HOLE                 29
#define SW_SHOOTING_LANE              30
#define SW_START                      31
#define SW_MENU_LEFT                  32
#define SW_MENU_RIGHT                 33
#define SW_TILT                       34


#define SOUND_FX_1                     1
#define SOUND_FX_SPINNER               2
#define SOUND_BALL_DRAIN               3



void game_play_tick() {
    // act on switches
    // act on timeouts
    // act on animations finished
}

void start_animation(int animation_no);
void start_timeout(int timeout_no);
void start_sound(int sound_no);
void start_coil(int coil_no);
void add_score(word bcd_amount_in_tens);

struct game_runtime_t {
    int loop_target_value: 3;    // 0..4 for 10k, 20k, 30k, 40k, 173k
    int spinner_value: 3;        // 0..4 for 100, 400, 900, 1600, 2500.
    int bonus_multiplier: 3;     // 1..7 for x1 .. x7 (minimum is 1, not zero)
    int next_object_to_make: 3;  // 0-8 means 1-9. 9=wow, 10=special.
    int left_outlane: 1;
    int right_outlane: 1;
    int left_inlane: 1;
    int right_inlane: 1;
    int shoot_again: 1;    
} game;
struct game_settings_t {
    
} settings;

void handle_timeout(char timeout_no) {
    switch (timeout_no) {
        case TIMEOUT_BALL_NINE_MOVING_TARGET:
            // move to the next moving target
            // adjust lamps accordingly
            break;
            
        case TIMEOUT_WOW_MOVING_TARGET:
            // move to the next moving target, skip no 1.
            // adjust lamps accordingly
            break;
            
        case TIMEOUT_RESET_SPINNER_VALUE:
            // make spinner_value zero, 
            // adjust lamps accordingly
            break;
    }
}

void handle_animation_finished(char animation_no) {
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

void handle_switch_closed(char switch_no) {
    switch (switch_no) {
        case SW_LEFT_OUTLANE:
        case SW_RIGHT_OUTLANE:
        case SW_LEFT_INLANE:
        case SW_RIGHT_INLANE:
        
        case SW_MAIN_POP_BUMPER:
        case SW_TOP_POP_BUMPER:
    
        case SW_TOP_BANK_LEFT_TARGET:
        case SW_TOP_BANK_CENTER_TARGET:
        case SW_TOP_BANK_RIGHT_TARGET:
        
        case SW_RIGHT_BANK_LEFT_TARGET:
        case SW_RIGHT_BANK_CENTER_TARGET:
        case SW_RIGHT_BANK_RIGHT_TARGET:
        
        case SW_LEFT_BANK_TARGET_1:
        case SW_LEFT_BANK_TARGET_2:
        case SW_LEFT_BANK_TARGET_3:
        case SW_LEFT_BANK_TARGET_4:
        case SW_LEFT_BANK_TARGET_5:
        case SW_LEFT_BANK_TARGET_6:
        case SW_LEFT_BANK_TARGET_7:
        case SW_LEFT_BANK_TARGET_8:
            // I think we must raise the 8 drop targets whenever target 8 is hit,
            // to avoid having a ball stuck on the gap that 8 leaves.
        
        case SW_LEFT_LANE_CAPTURE_FIRST_BALL:
        case SW_LEFT_LANE_CAPTURE_SECOND_BALL:
        case SW_LEFT_LANE_CAPTURE_THIRD_BALL:
        case SW_LEFT_LANE_EXIT:
            add_score(0x0300);
            start_sound(SOUND_FX_1);
            game.loop_target_value = ((game.loop_target_value + 1) % 5);
            start_animation(ANIM_TOP_LOOP_ADVANCE_VALUE);
            break;
    
        case SW_SKILL_SHOT_TARGET:
            add_score(0x700);
            game.spinner_value = 4; // maximum
            // adjust lamps
            // should turn off the red arrow lamp? (in the original game it seems to be always on)
            break;
        
        case SW_SPINNER:
            add_score(get_spinner_score_bcd(game.spinner_value));
            start_animation(ANIM_SPINNER_COLLECT_VALUE);
            start_timeout(TIMEOUT_RESET_SPINNER_VALUE); // one second?
            start_sound(SOUND_FX_SPINNER);
            break;
        
        case SW_TOP_LOOP_PASS:
            add_score(0x0300);
            start_sound(SOUND_FX_1);
            game.loop_target_value = ((game.loop_target_value + 1) % 5);
            start_animation(ANIM_TOP_LOOP_ADVANCE_VALUE);
            break;
        
        case SW_TOP_LOOP_TARGET:
            // collect the score, animation, sound, etc.
        
        case SW_DRAIN_HOLE:
            // play sad fx, 
            // fall into state of assigning bonus,
            // etc 
        
        case SW_START:
            // add user before ball leaves the shooter
            break;
    }
}

void handle_switch_opened(char switch_no) {
    switch (switch_no) {
        case SW_SHOOTING_LANE:
            // game on, no more players, start audio etc.
            break;
    }
}

word get_spinner_score_bcd(byte spinner_value) {
    if (spinner_value == 0) return 0x0010; //   100
    if (spinner_value == 1) return 0x0040; //   400
    if (spinner_value == 2) return 0x0090; //   900
    if (spinner_value == 3) return 0x0160; // 1,600
    if (spinner_value == 4) return 0x0250; // 2,500
    return 0x0010; // 100
}

word get_loop_score_bcd(byte loop_target_value) {
    if (loop_target_value == 4) return (word)0x173000; // 173,000  oopsy! dword!!!
    if (loop_target_value == 3) return 0x4000;   //  40,000
    if (loop_target_value == 2) return 0x3000;   //  30,000
    if (loop_target_value == 1) return 0x2000;   //  20,000
    return 0x1000; // 10,000    
}
 