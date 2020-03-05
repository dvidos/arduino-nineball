#ifndef CONSTANTS_H
#define CONSTANTS_H



#define MODE_ATTRACT           0
#define MODE_HAPPY_HOUR_GAME   1
#define MODE_PRACTICE_GAME     2
#define MODE_JUKE_BOX          3
#define MODE_NORMAL_GAME       4
#define MODE_SETTINGS          5
#define MODE_DIAGNOSTICS       6



#define ANIM_TOP_LOOP_ADVANCE_VALUE   1    // animation of lamps ramping up (1 time, upwards succession, then light the correct value, incrementatlly 1, 1+2, 1+2+3 etc)
#define ANIM_TOP_LOOP_COLLECT_VALUE   2    // animation of lamps ramping down (first blink 7 times all lit values, then 4 times quick succession downwards, single lamp chase)
#define ANIM_SPINNER_INCREASE_VALUE   3    // on the original game, there is no animation for increasing the value of the spinner
#define ANIM_SPINNER_COLLECT_VALUE    4    // moderate chase of one lamp each time, through the lamps that were already lit.
#define ANIM_BONUS_MULTIPLIER         5    // fast blink of the new value for 8 times.




#define GAME_MODE_BALL_EJECTING    0
#define GAME_MODE_BALL_SETTING_UP_NUMBER    0
#define GAME_MODE_BALL_WAITING_FOR_NUMBER_1    0
#define GAME_MODE_BALL_DONE_NUMBER_1           0


#define TIMEOUT_BALL_NINE_MOVING_TARGET  1    // move to the next moving target
#define TIMEOUT_WOW_MOVING_TARGET        2    // move to the next moving target, skip no 1.
#define TIMEOUT_RESET_SPINNER_VALUE      3    // spinner stopped spinning, reset its value
#define TIMEOUT_SPECIAL_MOVING_TARGET    4    // time to move to another target!




#define LAMP_SKILL_SHOT                1
#define LAMP_CAPTURE_BALL              2
#define LAMP_TOP_BANK_LEFT_TARGET      3
#define LAMP_TOP_BANK_CENTER_TARGET    4
#define LAMP_TOP_BANK_RIGHT_TARGET     5
#define LAMP_RIGHT_BANK_LEFT_TARGET    6
#define LAMP_RIGHT_BANK_CENTER_TARGET  7
#define LAMP_RIGHT_BANK_RIGHT_TARGET   8
#define LAMP_MAIN_POP_BUMPER           9
#define LAMP_TOP_POP_BUMPER           10
#define LAMP_LOOP_VALUE_10K           11
#define LAMP_LOOP_VALUE_20K           12
#define LAMP_LOOP_VALUE_30K           13
#define LAMP_LOOP_VALUE_40K           14
#define LAMP_LOOP_VALUE_173K          15
#define LAMP_OBJECT_1_DROP_TARGET     16
#define LAMP_OBJECT_2_DROP_TARGET     17
#define LAMP_OBJECT_3_DROP_TARGET     18
#define LAMP_OBJECT_4_DROP_TARGET     19
#define LAMP_OBJECT_5_DROP_TARGET     20
#define LAMP_OBJECT_6_DROP_TARGET     21
#define LAMP_OBJECT_7_DROP_TARGET     22
#define LAMP_OBJECT_8_DROP_TARGET     23
#define LAMP_OBJECT_1_WOW             24
#define LAMP_OBJECT_2_WOW             25
#define LAMP_OBJECT_3_WOW             26
#define LAMP_OBJECT_4_WOW             27
#define LAMP_OBJECT_5_WOW             28
#define LAMP_OBJECT_6_WOW             29
#define LAMP_OBJECT_7_WOW             30
#define LAMP_OBJECT_8_WOW             31
#define LAMP_OBJECT_2_SPECIAL         32
#define LAMP_OBJECT_3_SPECIAL         33
#define LAMP_OBJECT_4_SPECIAL         34
#define LAMP_OBJECT_5_SPECIAL         35
#define LAMP_OBJECT_6_SPECIAL         36
#define LAMP_OBJECT_7_SPECIAL         37
#define LAMP_OBJECT_8_SPECIAL         38
#define LAMP_OBJECT_9                 39
#define LAMP_ALL_TARGETS_DOWN_SPECIAL 40                  
#define LAMP_BONUS_MULTIPLIER_X1      41
#define LAMP_BONUS_MULTIPLIER_X2      42
#define LAMP_BONUS_MULTIPLIER_X4      43
#define LAMP_SPINNER_VALUE_400        44
#define LAMP_SPINNER_VALUE_900        45
#define LAMP_SPINNER_VALUE_1600       46
#define LAMP_SPINNER_VALUE_2500       47
#define LAMP_SUPER_BONUS_77K          48
#define LAMP_BONUS_MADE_1             49
#define LAMP_BONUS_MADE_2             50
#define LAMP_BONUS_MADE_3             51
#define LAMP_BONUS_MADE_4             52
#define LAMP_BONUS_MADE_5             53
#define LAMP_BONUS_MADE_6             54
#define LAMP_BONUS_MADE_7             55
#define LAMP_BONUS_MADE_8             56
#define LAMP_BONUS_MADE_9             57
#define LAMP_LEFT_OUTLANE             58 
#define LAMP_LEFT_INLANE              59 
#define LAMP_RIGHT_INLANE             60 
#define LAMP_RIGHT_OUTLANE            61 
#define LAMP_SHOOT_AGAIN              62
#define LAMP_START                    63


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
#define SW_LEFT_LANE_CAPTURED_BALL    21
#define SW_LEFT_LANE_SECOND_BALL      22
#define SW_LEFT_LANE_THIRD_BALL       23
#define SW_LEFT_LANE_EXIT             24
#define SW_SKILL_SHOT_TARGET          25
#define SW_SPINNER                    26
#define SW_TOP_LOOP_PASS              27
#define SW_TOP_LOOP_TARGET            28
#define SW_LEFT_SLINGSHOT             29
#define SW_RIGHT_SLINGSHOT            30
#define SW_DRAIN_HOLE                 31
#define SW_SHOOTING_LANE              32
#define SW_START                      33
#define SW_MENU_LEFT                  34
#define SW_MENU_RIGHT                 35
#define SW_TILT                       36


#define SOUND_FX_1             1
#define SOUND_FX_2             2
#define SOUND_FX_3             3
#define SOUND_FX_4             4
#define SOUND_FX_5             5
#define SOUND_FX_6             6
#define SOUND_FX_7             7
#define SOUND_SPINNER          8
#define SOUND_FEED_BALL        9
#define SOUND_SLINGSHOT       10
#define SOUND_INLANE          11
#define SOUND_OUTHOLE_DRAIN   12
#define SOUND_TILT            13
#define SOUND_COIN            15
#define SOUND_START           16
#define SOUND_BOOT            17
#define SOUND_PHASERS         18
#define SOUND_FOUR_FX_IN_ONE  19
#define SOUND_MACHINE_GUNS    20
#define SOUND_FAST_PHASERS    21
#define SOUND_ANTICIPATION    22
#define SOUND_EXPLOSION       23

#define SOUND_EFFECT_FIRST     1
#define SOUND_EFFECT_LAST     23
#define SOUND_SONG_FIRST     101
#define SOUND_SONG_LAST      110






#endif // CONSTANTS_H
