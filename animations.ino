

#include "constants.h"

/*
#define ANIM_TOP_LOOP_ADVANCE_VALUE   1    // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
#define ANIM_TOP_LOOP_COLLECT_VALUE   2    // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
#define ANIM_BONUS_MULTIPLIER         3    // fast blink of the new value for 8 times.
#define ANIM_SPINNER_INCREASE_VALUE   4    // on the original game, there is no animation for increasing the value of the spinner
#define ANIM_SPINNER_COLLECT_VALUE    5    // moderate chase of one lamp each time, through the lamps that were already lit.
#define ANIM_BALLS_DIAMOND            6    // on the original game it seems odd/even blink pretty fast
*/

/**
 * Let's find a way, where we can configure the animations as data
 * (number of steps, which lamps participate, which are on on each step, step duration)
 * And store all that in program memory.
 * definition is:
 *   const dataType varName[] PROGMEM = { data0, data1, data2, ... } 
 */

// define lamp groups that work for animation
#define LAMP_GROUP_LOOP_TARGET_VALUE      0
#define LAMP_GROUP_SPINNER_TARGET_VALUE   1
#define LAMP_GROUP_BONUS_MULTIPLIER       2
#define LAMP_GROUP_OBJECTS_DIAMOND        3

const byte lampGroups[4][10] PROGMEM = {
    { 0, 1, 2, 3, 4 },
    { 32, 33, 34, 35, 36 },
    { 6, 4, 23, 21, 19 },
    { 41, 42, 43, 44, 45, 46 }
};

#define ANIMATION_TYPE_CHASE_UP     0
#define ANIMATION_TYPE_CHASE_DOWN   1
#define ANIMATION_TYPE_FILL         2
#define ANIMATION_TYPE_EMPTY        3
#define ANIMATION_TYPE_BLINK        4

struct Animation
{
    byte lampGroup: 3;
    byte type: 3; // chase up/down, fill/empty, blink etc
    byte step_duration: 4; // in 100 msec units (0..15 => 0 .. 1.5 seconds)
    byte exec_times: 2; // up to 4 times
    
    // maybe which animation to play next???   
};

const struct Animation animations[10] PROGMEM = {
    { LAMP_GROUP_LOOP_TARGET_VALUE, ANIMATION_TYPE_CHASE_UP, 3, 2 },
    { LAMP_GROUP_SPINNER_TARGET_VALUE, ANIMATION_TYPE_FILL, 3, 2 }
};

struct AnimationExecution
{
    byte animation_no: 3;
    byte current_step_no;
    byte current_step_time_left;
};

void chase_up(byte lamps[], byte lamps_count, char &step_no)
{
    step_no += 1;
    if (step_no >= lamps_count)
        step_no = 0;
        
    for (char i = 0; i < lamps_count; i++) {
        if (i == step_no) {
            LampMatrix.lamp_on(lamps[(byte)i]);
        } else {
            LampMatrix.lamp_off(lamps[(byte)i]);
        }
    }
}

void chase_down(byte lamps[], byte lamps_count, char &step_no)
{
    step_no -= 1;
    if (step_no < 0)
        step_no = lamps_count - 1;
        
    for (char i = lamps_count - 1; i >= 0; i--) {
        if (i == step_no) {
            LampMatrix.lamp_on(lamps[(byte)i]);
        } else {
            LampMatrix.lamp_off(lamps[(byte)i]);
        }
    }
}

void blink(byte lamps[], byte lamps_count, byte &step_no)
{
    step_no = !step_no;
        
    for (byte i = lamps_count - 1; i >= 0; i--) {
        if (step_no) {
            LampMatrix.lamp_on(lamps[i]);
        } else {
            LampMatrix.lamp_off(lamps[i]);
        }
    }
}

void fill_in(byte lamps[], byte lamps_count, byte &step_no)
{
    step_no += 1;
    if (step_no >= lamps_count)
        step_no = 0;
        
    for (byte i = 0; i < lamps_count; i++) {
        if (i <= step_no) {
            LampMatrix.lamp_on(lamps[i]);
        } else {
            LampMatrix.lamp_off(lamps[i]);
        }
    }
}

void empty(byte lamps[], byte lamps_count, byte &step_no)
{
    step_no -= 1;
    if (step_no < 0)
        step_no = lamps_count - 1;
        
    for (char i = lamps_count - 1; i >= 0; i--) {
        if (i <= step_no) {
            LampMatrix.lamp_on(lamps[(byte)i]);
        } else {
            LampMatrix.lamp_off(lamps[(byte)i]);
        }
    }
}


    
class CAnimator
{
public:

    void start(byte animation_no);
    void stop_all();
    
    void ISR_every_100_milliseconds();
};


void CAnimator::start(byte animation_no)
{
}

void CAnimator::stop_all()
{
}

void CAnimator::ISR_every_100_milliseconds()
{
    // diminish counters, update lamps etc.
}
