

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

#define ANIM_TOP_LOOP_ADVANCE_VALUE   1    // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
#define ANIM_TOP_LOOP_COLLECT_VALUE   2    // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
#define ANIM_BONUS_MULTIPLIER         3    // fast blink of the new value for 8 times.
#define ANIM_SPINNER_INCREASE_VALUE   4    // on the original game, there is no animation for increasing the value of the spinner
#define ANIM_SPINNER_COLLECT_VALUE    5    // moderate chase of one lamp each time, through the lamps that were already lit.
#define ANIM_BALLS_DIAMOND            6    // on the original game it seems odd/even blink pretty fast


// define lamp groups that work for animation
#define LAMP_GROUP_LOOP_TARGET_VALUE      0
#define LAMP_GROUP_SPINNER_TARGET_VALUE   1
#define LAMP_GROUP_BONUS_MULTIPLIER       2
#define LAMP_GROUP_OBJECTS_DIAMOND        3
#define LAMP_GROUP_OBJECTS_NUMBERS        4
#define LAMP_GROUP_OBJECTS_WOW            5
#define LAMP_GROUP_OBJECTS_SPECIAL        6
#define LAMP_GROUPS_COUNT                 7


extern CLampMatrix LampMatrix;


typedef byte (*get_bitmap_func_ptr)(byte current_step);
typedef void (*set_lamps_func_ptr)(byte bitmap);

class CAnimation
{
    byte active: 1;        // to quickly go through the list.
    byte step_ticks;       // in ticks of 10 msec. 100 = 1 second
    byte ticks_left;       // for the current step
    byte current_step: 5;  // 0..32
    byte steps_count: 5;   // 0..32
    get_bitmap_func_ptr bitmap_func;
    set_lamps_func_ptr set_lamps_func;
    byte final_bitmap;      // bitmap for lamps to display at the end
    
    void start(get_bitmap_func_ptr get_bitmap, byte steps, byte ticks_per_step, set_lamps_func_ptr set_lamps, byte final_bitmap)
    {
        this->active = 1;
        this->current_step = 0;
        this->steps_count = steps;
        this->step_ticks = ticks_per_step;
        this->ticks_left = step_ticks;
        this->bitmap_func = get_bitmap;
        this->set_lamps_func = set_lamps;
        this->final_bitmap = final_bitmap;

        // first presentation
        byte bitmap = (*this->bitmap_func)(current_step);
        (*this->set_lamps_func)(bitmap);
    }
    
    void tick()
    {
        if (!active)
            return;
        
        ticks_left -= 1;
        if (ticks_left > 0)
            return;

        current_step += 1;
        ticks_left = step_ticks;
         
        if (current_step < steps_count)
        {
            byte bitmap = (*bitmap_func)(current_step);
            (*set_lamps_func)(bitmap);
        }
        else
        {
            // restore the lamps to final values and be done.
            (*set_lamps_func)(final_bitmap);
            active = 0;
        }
    }
    
    void stop()
    {
        (*set_lamps_func)(0);
        active = 0;
    }
};


class CAnimator
{
public:
    CAnimation animations[LAMP_GROUPS_COUNT];

    void start(byte animation_no);
    void stop_all();
    
    void start_blinking(byte lamp_no);
    void stop_blinking();
    
    void ISR_every_100_milliseconds();
    
private:
    byte blinking_lamp: 6;
    byte blinking_time: 4;        // counts up to 10 calls of 10 msec each time
    byte blinking_status: 1;
};


void CAnimator::start(byte animation_no)
{
    switch (animation_no)
    {
        case ANIM_TOP_LOOP_ADVANCE_VALUE:      // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
        case ANIM_TOP_LOOP_COLLECT_VALUE:      // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
        case ANIM_BONUS_MULTIPLIER:            // fast blink of the new value for 8 times.
        case ANIM_SPINNER_INCREASE_VALUE:      // on the original game, there is no animation for increasing the value of the spinner
        case ANIM_SPINNER_COLLECT_VALUE:       // moderate chase of one lamp each time, through the lamps that were already lit.
        case ANIM_BALLS_DIAMOND:               // on the original game it seems odd/even blink pretty fast
            break;
    }
}

void CAnimator::stop_all()
{
    for (byte i = 0; i < LAMP_GROUPS_COUNT; i++)
        animations[i].stop();
    
    // stop all and...
    blinking_lamp = 0xFF;
}

void CAnimator::start_blinking(byte lamp_no)
{
    // extinguish previous lamp
    if (blinking_lamp != 0xFF && blinking_status)
    {
        LampMatrix::lamp_on(lamp_no);
    }
    
    blinking_lamp = lamp_no;
    blinking_time = 0;
    blinking_status = 1;
    LampMatrix::lamp_on(lamp_no);
}

void stop_blinking()
{
    blinking_lamp = 0xFF;
}

void CAnimator::ISR_every_10_milliseconds()
{
    // diminish counters, update lamps etc.
    for (byte i = 0; i < LAMP_GROUPS_COUNT; i++)
        animatins[i].tick();
    
    // from the videos I deducted that lamps that blinked on the original game, 
    // did so at maybe 5 Hz, so 100 msecs between toggles
    if (blinking_lamp != 0xFF)
    {
        blinking_time += 1; // cautious, this is 4 bits wide.
        if (blinking_time >= 9)
        {
            blinking_status = !blinking_status;
            LampMatrix.set_lamp(blinking_lamp, blinking_status);
            blinking_time = 0;
        }
    } 
}


byte CAnimator::loop_target_get_bitmap(byte step_no)
{
    // in this way (having switch statements in code), we use the program memory.
    // otherwise, we'd have to "bring" data from PROGMEM into ram to use it.
    
    // 3 times blink, 3 times chase
    switch (step_no)
    {
        case 0:
        case 2:
        case 4:
            return 0x1F;
        case 1:
        case 3:
        case 5:
            return 0x00;
        case 6:
        case 11:
        case 16:
            return 0x01;
        case 7:
        case 12:
        case 17:
            return 0x02;
        case 8:
        case 13:
        case 18:
            return 0x04;
        case 9:
        case 14:
        case 19:
            return 0x08;
        case 10:
        case 15:
        case 20:
            return 0x10;
    }
    
    return 0;
}

void CAnimator::loop_target_set_lamps(byte bitmap)
{
    LampMatrix.set_lamp(LAMP_LOOP_VALUE_173K, (bool)(bitmap & 0x10));
    LampMatrix.set_lamp(LAMP_LOOP_VALUE_40K, (bool)(bitmap & 0x08));
    LampMatrix.set_lamp(LAMP_LOOP_VALUE_30K, (bool)(bitmap & 0x04));
    LampMatrix.set_lamp(LAMP_LOOP_VALUE_20K, (bool)(bitmap & 0x02));
    LampMatrix.set_lamp(LAMP_LOOP_VALUE_10K, (bool)(bitmap & 0x01));
}


