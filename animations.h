

#include "constants.h"


// define lamp groups that work for animation
#define LAMP_GROUP_LOOP_TARGET_VALUE      0
#define LAMP_GROUP_SPINNER_TARGET_VALUE   1
#define LAMP_GROUP_BONUS_MULTIPLIER       2
#define LAMP_GROUPS_COUNT                 3


extern CLampMatrix LampMatrix;

// function pointers allow different animations in the same lamp group (e.g. increase & collect)
typedef byte (*get_bitmap_func_ptr)(byte current_step);
typedef void (*set_lamps_func_ptr)(byte bitmap);

// one class to implement the steps of each animation - one per lamp group.
class CAnimation
{
public:
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
        
        this->active = 1;
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

    CAnimator();
    void start(byte animation_no, byte final_bitmap);
    void stop_all();
    
    void start_blinking(byte lamp_no);
    void stop_blinking();
    
    void tick();
    
private:
    byte blinking_on: 1;          // whether a lamp should blink 
    byte blinking_lamp: 6;
    byte blinking_time: 4;        // counts up to 10 calls of 10 msec each time
    byte blinking_status: 1;
    
    // in this way (having switch statements in code), we use the program memory (which we have plenty)
    // otherwise, we'd have to "bring" data from PROGMEM into ram to use it.
    static byte loop_target_increase_get_bitmap(byte step);
    static byte loop_target_collect_get_bitmap(byte step);
    static byte spinner_value_increase_get_bitmap(byte step_no);
    static byte spinner_value_collect_get_bitmap(byte step_no);
    static byte bonus_multiplier_increase_get_bitmap(byte step);
    
    static void loop_target_set_lamps(byte bitmap);
    static void spinner_set_lamps(byte bitmap);
    static void bonus_multiplier_set_lamps(byte bitmap);
};


CAnimator::CAnimator()
{
    for (byte i = 0; i < LAMP_GROUPS_COUNT; i++)
        animations[i].active = 0;
    
    blinking_on = false; 
}

void CAnimator::start(byte animation_no, byte final_bitmap)
{
    switch (animation_no)
    {
        case ANIM_TOP_LOOP_ADVANCE_VALUE:      // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
            animations[LAMP_GROUP_LOOP_TARGET_VALUE].start(
                loop_target_increase_get_bitmap, 
                21, // steps
                1,  // ticks per step,
                loop_target_set_lamps,
                final_bitmap
            );
            break;
            
        case ANIM_TOP_LOOP_COLLECT_VALUE:      // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
            animations[LAMP_GROUP_LOOP_TARGET_VALUE].start(
                loop_target_collect_get_bitmap, 
                20, // steps
                1,  // ticks per step,
                loop_target_set_lamps,
                final_bitmap
            );
            break;
            
        case ANIM_SPINNER_INCREASE_VALUE:      // on the original game, there is no animation for increasing the value of the spinner
            animations[LAMP_GROUP_SPINNER_TARGET_VALUE].start(
                spinner_value_increase_get_bitmap, 
                18, // steps
                1,  // ticks per step,
                spinner_set_lamps,
                final_bitmap
            );
            break;
            
        case ANIM_SPINNER_COLLECT_VALUE:       // moderate chase of one lamp each time, through the lamps that were already lit.
            animations[LAMP_GROUP_SPINNER_TARGET_VALUE].start(
                spinner_value_collect_get_bitmap, 
                24, // steps
                1,  // ticks per step,
                spinner_set_lamps,
                final_bitmap
            );
            break;
            
        case ANIM_BONUS_MULTIPLIER:            // fast blink of the new value for 8 times.
            animations[LAMP_GROUP_BONUS_MULTIPLIER].start(
                bonus_multiplier_increase_get_bitmap, 
                12, // steps
                1,  // ticks per step,
                bonus_multiplier_set_lamps,
                final_bitmap
            );
            break;
    }
}

void CAnimator::stop_all()
{
    for (byte i = 0; i < LAMP_GROUPS_COUNT; i++)
        animations[i].stop();
    
    stop_blinking();
}

void CAnimator::start_blinking(byte lamp_no)
{
    // extinguish previous lamp
    if (blinking_on && blinking_status)
        LampMatrix.lamp_off(blinking_lamp);
    
    blinking_lamp = lamp_no;
    blinking_time = 0;
    blinking_status = 1;
    LampMatrix.lamp_on(lamp_no);
    blinking_on = true;
}

void CAnimator::stop_blinking()
{
    if (blinking_on && blinking_status)
        LampMatrix.lamp_off(blinking_lamp);
    
    blinking_on = false;
}

void CAnimator::tick()
{
    // diminish counters, update lamps etc.
    for (byte i = 0; i < LAMP_GROUPS_COUNT; i++)
        animations[i].tick();
    
    // from the videos I deducted that lamps that blinked on the original game, 
    // did so at maybe 5 Hz, so 100 msecs between toggles
    if (blinking_on)
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


byte CAnimator::loop_target_increase_get_bitmap(byte step_no)
{
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

byte CAnimator::loop_target_collect_get_bitmap(byte step_no)
{
    // 3 times blink, 3 times chase
    switch (step_no)
    {
        case 0:
        case 5:
        case 10:
        case 15:
            return 0x10;
        case 1:
        case 6:
        case 11:
        case 16:
            return 0x08;
        case 2:
        case 7:
        case 12:
        case 17:
            return 0x04;
        case 3:
        case 8:
        case 13:
        case 18:
            return 0x02;
        case 4:
        case 9:
        case 14:
        case 19:
            return 0x01;
    }
    return 0;
}

byte CAnimator::spinner_value_increase_get_bitmap(byte step_no)
{
    switch (step_no)
    {
        case 0:
        case 2:
        case 4:
            return 0x0F;
        case 1:
        case 3:
        case 5:
            return 0x00;
        case 6:
        case 10:
        case 14:
            return 0x01;
        case 7:
        case 11:
        case 15:
            return 0x02;
        case 8:
        case 12:
        case 16:
            return 0x04;
        case 9:
        case 13:
        case 17:
            return 0x08;
    }
    return 0;
}

byte CAnimator::spinner_value_collect_get_bitmap(byte step_no)
{
    switch (step_no)
    {
        case 0:
        case 8:
        case 16:
            return 0x01; // ___*
        case 1:
        case 9:
        case 17:
            return 0x03; // __**
        case 2:
        case 10:
        case 18:
            return 0x07; // _***
        case 3:
        case 11:
        case 19:
            return 0x0F; // ****
        case 4:
        case 12:
        case 20:
            return 0x0E; // ***_
        case 5:
        case 13:
        case 21:
            return 0x0C; // **__
        case 6:
        case 14:
        case 22:
            return 0x08; // *___
        case 7:
        case 15:
        case 23:
            return 0x00; // ____
    }
    return 0;
}

byte CAnimator::bonus_multiplier_increase_get_bitmap(byte step_no)
{
    switch (step_no)
    {
        case 0:
        case 3:
        case 6:
        case 9:
            return 0x04;
        case 1:
        case 4:
        case 7:
        case 10:
            return 0x02;
        case 2:
        case 5:
        case 8:
        case 11:
            return 0x01;
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

void CAnimator::spinner_set_lamps(byte bitmap)
{
    LampMatrix.set_lamp(LAMP_SPINNER_VALUE_2500, (bool)(bitmap & 0x08));
    LampMatrix.set_lamp(LAMP_SPINNER_VALUE_1600, (bool)(bitmap & 0x04));
    LampMatrix.set_lamp(LAMP_SPINNER_VALUE_900, (bool)(bitmap & 0x02));
    LampMatrix.set_lamp(LAMP_SPINNER_VALUE_400, (bool)(bitmap & 0x01));
}

void CAnimator::bonus_multiplier_set_lamps(byte bitmap)
{
    LampMatrix.set_lamp(LAMP_BONUS_MULTIPLIER_X4, (bool)(bitmap & 0x04));
    LampMatrix.set_lamp(LAMP_BONUS_MULTIPLIER_X2, (bool)(bitmap & 0x02));
    LampMatrix.set_lamp(LAMP_BONUS_MULTIPLIER_X1, (bool)(bitmap & 0x01));
}



