#ifndef MESSAGES_H
#define MESSAGES_H

#include <avr/pgmspace.h>



#define M_FATAL                               1
#define M_ASSERION_FAILED                     2
#define M_INT_VALUE_IS                        4
#define M_SETUP_STARTING                      5
#define M_SETUP_FINISHED                      6
#define M_SERIAL_EMULATOR_RUNNING             7
#define M_ATTRACT_STARTING_IDLE_MODE          8
#define M_ATTRACT_STARTING_GAME_MODE          9
#define M_ATTRACT_STARTING_RADIO_MODE        10
#define M_END_OF_TRACK_D_DETECTED            11
#define M_GOING_TO_NEW_RANDOM_SONG           12
#define M_GOING_TO_PREVIOUS_SONG             13
#define M_STOPPING_RADIO_MODE                14
#define M_ATTRACT_STARTING_SETTINGS_MODE     15
#define M_EXITING_WITHOUT_SAVING             16
#define M_ATTRACT_STARTING_DIAGNOSTICS_MODE  17
#define M_RELEASING_BALLS_AND_WAITING_FOR_DRAINING   18
#define M_RELEASING_BALL_AS_CAPTURING_IS_DISABLED    19
#define M_RELEASING_BALLS_AS_THEY_ARE_ALL_CAPTURED   20
#define M_BALL_CAPTURED_BUT_ANOTHER_STILL_IN_PLAYFIELD   21
#define M_BALL_CAPTURED_GIVING_NEW_ONE_TO_PLAYER         22
#define M_BALL_DRAINED_BUT_ANOTHER_STILL_IN_PLAYFIELD    23
#define M_RELEASING_ONE_CAPTURED_BALL_FOR_USER           24
#define M_GAME_OVER_FOR_THIS_BALL                        25
#define M_SHOOT_AGAIN_GRANTED                            26
#define M_BALL_CAPTURING_ENABLED                         27
#define M_RELEASING_CAPTURED_BALLS              28
#define M_SENDING_BALL_TO_SHOOTING_LANE         29
#define M_FAILED_RELEASING_CAPTURED_BALLS       30
#define M_FAILED_LAUNCHING_BALL_TO_SHOOTER_LANE 31
#define M_FAILED_BRINGING_EIGHT_BANK_TARGETS_UP 32
#define M_MADE_CURRENT_OBJECT_NO_D              33
#define M_MADE_OBJECT_NO_9                      34
#define M_STARTING_NO_9_SEQUENCE                35
#define M_STARTING_8_BANK_WOW_SEQUENCE          36
#define M_STARTING_8_BANK_SPECIAL_SEQUENCE      37
#define M_STARTING_SPOT_NUMBER_TIMEOUT          38
#define M_GAMEPLAY_STARTED_IN_MODE_D            39
#define M_COLLECTING_BONUSES                    40
#define M_PREPARING_GAME_FOR_PLAYER_D_BALL_D    41
#define M_SAVING_SETTINGS_TO_EEPROM             42
#define M_LOADING_SETTINGS_FROM_EEPROM          43
#define M_LOOP_TARGET_VALUE_IS_NOW_D            44
#define M_LOOP_TARGET_VALUE_COLLECTED_AND_RESET    45
#define M_OUTLANES_TURNED_ON_OR_FLIPPED_D_D     46
#define M_OUTLANES_TURNED_OFF                   47
#define M_SPINNER_VALUE_IS_NOW_D                48
#define M_SPINNER_VALUE_RESET                   49
#define M_WAVTRIGGER_VERSION_D_DETECTED         50
#define M_COULD_NOT_RETRIEVE_WAVTRIGGER_VERSION 51
#define M_WAVTRIGGER_REPORTS_D_TRACKS_ON_CARD   52
#define M_AUDIO_INITIALIZED                     53
#define M_AUDIO_TRACK_D_LAUNCHED                54
#define M_ALL_AUDIO_TRACKS_STOPPED              55
#define M_COILS_INITIATED                       56
#define M_TOP_BANK_COIL_FIRED                   57
#define M_RIGHT_BANK_COIL_FIRED                 58
#define M_LEFT_8_BANK_COIL_FIRED                59
#define M_OUTHOLE_EJECT_COIL_FIRED              60
#define M_CAPTURE_LANE_EJECT_COIL_FIRED         61
#define M_LEFT_SLINGSHOT_COIL_FIRED             62
#define M_RIGHT_SLINGSHOT_COIL_FIRED            63
#define M_KNOCKER_COIL_FIRED                    64
#define M_FLIPPERS_RELAY_SET_TO_D               65
#define M_LAMP_MATRIX_INITIALIZED               66
#define M_ALL_LAMPS_OFF                         67
#define M_LAMPS_COLUMN_D_ON                     68
#define M_LAMPS_ROW_D_ON                        69
#define M_SCORE_DISPLAY_INITIALIZED             70
#define M_SWITCH_MATRIX_INITIALIZED             71
#define M_EMULATOR_SCREEN_01                    72
#define M_EMULATOR_SCREEN_02                    73
#define M_EMULATOR_SCREEN_03                    74
#define M_EMULATOR_SCREEN_04                    75
#define M_EMULATOR_SCREEN_05                    76
#define M_EMULATOR_SCREEN_06                    77
#define M_EMULATOR_SCREEN_07                    78
#define M_EMULATOR_SCREEN_08                    79
#define M_EMULATOR_SCREEN_09                    80
#define M_EMULATOR_SCREEN_10                    81
#define M_EMULATOR_SCREEN_11                    82
#define M_EMULATOR_SCREEN_12                    83
#define M_EMULATOR_SCREEN_13                    84
#define M_EMULATOR_SCREEN_14                    85
#define M_EMULATOR_SCREEN_15                    86
#define M_EMULATOR_SCREEN_16                    87
#define M_EMULATOR_SCREEN_17                    88
#define M_EMULATOR_SCREEN_18                    89
#define M_EMULATOR_SCREEN_19                    90
#define M_EMULATOR_SCREEN_20                    91
#define M_EMULATOR_SCREEN_21                    92
#define M_EMULATOR_SCREEN_22                    93
#define M_EMULATOR_SCREEN_23                    94
#define M_EMULATOR_SCREEN_24                    95
#define M_EMULATOR_SCREEN_25                    96
#define M_EMULATOR_SCREEN_26                    97
#define M_SW_TOP_LOOP_PASS_NAME                 98
#define M_SW_TOP_LOOP_TARGET_NAME               99
#define M_SW_TOP_BANK_LEFT_TARGET_NAME          100
#define M_SW_TOP_BANK_CENTER_TARGET_NAME        101
#define M_SW_TOP_BANK_RIGHT_TARGET_NAME         102
#define M_SW_RIGHT_BANK_LEFT_TARGET_NAME        103
#define M_SW_RIGHT_BANK_CENTER_TARGET_NAME      104
#define M_SW_RIGHT_BANK_RIGHT_TARGET_NAME       105
#define M_SW_TOP_POP_BUMPER_NAME                106
#define M_SW_MAIN_POP_BUMPER_NAME               107
#define M_SW_SKILL_SHOT_TARGET_NAME             108
#define M_SW_LEFT_BANK_TARGET_1_NAME            109
#define M_SW_LEFT_BANK_TARGET_2_NAME            110
#define M_SW_LEFT_BANK_TARGET_3_NAME            111
#define M_SW_LEFT_BANK_TARGET_4_NAME            112
#define M_SW_LEFT_BANK_TARGET_5_NAME            113
#define M_SW_LEFT_BANK_TARGET_6_NAME            114
#define M_SW_LEFT_BANK_TARGET_7_NAME            115
#define M_SW_LEFT_BANK_TARGET_8_NAME            116
#define M_SW_LEFT_LANE_CAPTURED_BALL_NAME       117
#define M_SW_LEFT_LANE_SECOND_BALL_NAME         118
#define M_SW_LEFT_LANE_THIRD_BALL_NAME          119
#define M_SW_LEFT_LANE_EXIT_NAME                120
#define M_SW_SPINNER_NAME                       121
#define M_SW_LEFT_SLINGSHOT_NAME                122
#define M_SW_RIGHT_SLINGSHOT_NAME               123
#define M_SW_LEFT_OUTLANE_NAME                  124
#define M_SW_RIGHT_OUTLANE_NAME                 125
#define M_SW_LEFT_INLANE_NAME                   126
#define M_SW_RIGHT_INLANE_NAME                  127
#define M_SW_OUTHOLE_LEFT_NAME                  128
#define M_SW_OUTHOLE_MIDDLE_NAME                129
#define M_SW_OUTHOLE_RIGHT_NAME                 130
#define M_SW_SHOOTING_LANE_NAME                 131
#define M_SW_START_NAME                         132
#define M_SW_MENU_LEFT_NAME                     133
#define M_SW_MENU_RIGHT_NAME                    134
#define M_SWITCH_CLOSED                         135
#define M_SWITCH_OPENED                         136
#define M_SWITCH_CLICKED                        137



const char _eeprom_0[] PROGMEM = "String 0";
const char _eeprom_1[] PROGMEM = "*** FATAL %d ***";
const char _eeprom_2[] PROGMEM = "** Failed asserting that \"%s\"";
const char _eeprom_3[] PROGMEM = "String 3";
const char _eeprom_4[] PROGMEM = "int value: %d";
const char _eeprom_5[] PROGMEM = "setup() starting";
const char _eeprom_6[] PROGMEM = "setup() finished";
const char _eeprom_7[] PROGMEM = "Running Serial Emulator, send \"?\" for help";
const char _eeprom_8[] PROGMEM = "Attract starting idle mode";
const char _eeprom_9[] PROGMEM = "Attract starting game mode";
const char _eeprom_10[] PROGMEM = "Attract starting radio mode";
const char _eeprom_11[] PROGMEM = "End of track %d detected";
const char _eeprom_12[] PROGMEM = "Going to a new random song";
const char _eeprom_13[] PROGMEM = "Going to previous song";
const char _eeprom_14[] PROGMEM = "Stopping radio mode";
const char _eeprom_15[] PROGMEM = "Attract starting settings mode";
const char _eeprom_16[] PROGMEM = "Exiting without saving";
const char _eeprom_17[] PROGMEM = "Attract starting diagnostics mode";
const char _eeprom_18[] PROGMEM = "Releasing balls and waiting for draining";
const char _eeprom_19[] PROGMEM = "Releasing ball as capturing is disabled";
const char _eeprom_20[] PROGMEM = "Releasing balls as they're all captured";
const char _eeprom_21[] PROGMEM = "Ball captured, but another ball still somewhere in playfield";
const char _eeprom_22[] PROGMEM = "Ball captured, giving new one to player";
const char _eeprom_23[] PROGMEM = "Ball drained, but another ball still somewhere in playfield";
const char _eeprom_24[] PROGMEM = "Releasing one captured ball for user";
const char _eeprom_25[] PROGMEM = "Game over for this ball";
const char _eeprom_26[] PROGMEM = "Shoot again granted";
const char _eeprom_27[] PROGMEM = "Ball capturing enabled";
const char _eeprom_28[] PROGMEM = "Releasing captured balls";
const char _eeprom_29[] PROGMEM = "Sending to shooting lane";
const char _eeprom_30[] PROGMEM = "Warning: failed releasing captured balls";
const char _eeprom_31[] PROGMEM = "Warning: failed launching ball in shooting lane";
const char _eeprom_32[] PROGMEM = "Warning: exhausted tries to bring drop targets up";
const char _eeprom_33[] PROGMEM = "Made current object (%d)";
const char _eeprom_34[] PROGMEM = "Made object No 9";
const char _eeprom_35[] PROGMEM = "Starting No 9 sequence";
const char _eeprom_36[] PROGMEM = "Starting 8-bank WOW sequence";
const char _eeprom_37[] PROGMEM = "Starting 8-bank Special sequence";
const char _eeprom_38[] PROGMEM = "Starting Spot Number timeout";
const char _eeprom_39[] PROGMEM = "Gameplay started in mode %d";
const char _eeprom_40[] PROGMEM = "Collecting bonuses";
const char _eeprom_41[] PROGMEM = "Preparing gameplay for player %d, ball %d";
const char _eeprom_42[] PROGMEM = "Saving settings to EEPROM";
const char _eeprom_43[] PROGMEM = "Loading settings from EEPROM";
const char _eeprom_44[] PROGMEM = "Loop target value is now %d";
const char _eeprom_45[] PROGMEM = "Loop target value collected and reset";
const char _eeprom_46[] PROGMEM = "Outlanes lights turned on or flipped (%d, %d)";
const char _eeprom_47[] PROGMEM = "Outlanes lights turned off";
const char _eeprom_48[] PROGMEM = "Spinner value is now %d";
const char _eeprom_49[] PROGMEM = "Spinner value reset to 100";
const char _eeprom_50[] PROGMEM = "WavTrigger version %s detected";
const char _eeprom_51[] PROGMEM = "Could not retrieve wavTrigger version";
const char _eeprom_52[] PROGMEM = "WavTrigger reports %d tracks on SD card";
const char _eeprom_53[] PROGMEM = "Audio initialized";
const char _eeprom_54[] PROGMEM = "Audio track %d launched";
const char _eeprom_55[] PROGMEM = "All audio tracks stopped";
const char _eeprom_56[] PROGMEM = "Coils initialized";
const char _eeprom_57[] PROGMEM = "Top Bank coil fired";
const char _eeprom_58[] PROGMEM = "Right Bank coil fired";
const char _eeprom_59[] PROGMEM = "Left (8) Bank coil fired";
const char _eeprom_60[] PROGMEM = "Outhole Eject coil fired";
const char _eeprom_61[] PROGMEM = "Capture Lane Eject coil fired";
const char _eeprom_62[] PROGMEM = "Left Slingshot coil fired";
const char _eeprom_63[] PROGMEM = "Right Slingshot coil fired";
const char _eeprom_64[] PROGMEM = "Knocker coil fired";
const char _eeprom_65[] PROGMEM = "Flippers relay set to %d";
const char _eeprom_66[] PROGMEM = "Lamp Matrix initialized";
const char _eeprom_67[] PROGMEM = "Lamps all off";
const char _eeprom_68[] PROGMEM = "Lamps column %d ON";
const char _eeprom_69[] PROGMEM = "Lamps row %d ON";
const char _eeprom_70[] PROGMEM = "Score Display initialized";
const char _eeprom_71[] PROGMEM = "Switch Matrix initialized";
const char _eeprom_72[] PROGMEM = "+----------------------------+";
const char _eeprom_73[] PROGMEM = "| Q           _ _ _ Y<*      |";
const char _eeprom_74[] PROGMEM = "|  W * 173    * * *     \\    |";
const char _eeprom_75[] PROGMEM = "|     * 40    E R T  (U*)\\   |";
const char _eeprom_76[] PROGMEM = "|      * 30               \\  |";
const char _eeprom_77[] PROGMEM = "|       * 20          I *_ | |";
const char _eeprom_78[] PROGMEM = "|  <*    * 10   ( )   O *_ | |";
const char _eeprom_79[] PROGMEM = "|A                    P *_ | |";
const char _eeprom_80[] PROGMEM = "|S _2 * * *       * 4X     | |";
const char _eeprom_81[] PROGMEM = "|D _4 * * *       * 2X     | |";
const char _eeprom_82[] PROGMEM = "|  _6 * * *       * 1X     | |";
const char _eeprom_83[] PROGMEM = "|  _8 * * *                | |";
const char _eeprom_84[] PROGMEM = "|  9 *     * Sp     --[M]--| |";
const char _eeprom_85[] PROGMEM = "|  _7 * * *         * 2500 | |";
const char _eeprom_86[] PROGMEM = "|  _5 * * *         * 1600 | |";
const char _eeprom_87[] PROGMEM = "|F _3 * * *        * 900   | |";
const char _eeprom_88[] PROGMEM = "|  _1 * *         * 400    | |";
const char _eeprom_89[] PROGMEM = "|     *77K   *8            | |";
const char _eeprom_90[] PROGMEM = "|  *  \\    *6  *7    /  *  | |";
const char _eeprom_91[] PROGMEM = "| |H| J\\ *4  *9  *5 /K |L| | |";
const char _eeprom_92[] PROGMEM = "|G| |   \\  *2  *3  /   | |;| |";
const char _eeprom_93[] PROGMEM = "|*           *1           *| |";
const char _eeprom_94[] PROGMEM = "|          * ShAgn         |V|";
const char _eeprom_95[] PROGMEM = "+------------------Z-X-C---+-+";
const char _eeprom_96[] PROGMEM = "\\:Start  [:P1  ]:P2  ?:Help  /:Status  ~:Keep next switch down";
const char _eeprom_97[] PROGMEM = "Tiddle keeps next switch closed (no opening event is generated)";
const char _eeprom_98[] PROGMEM = "SW_TOP_LOOP_PASS";
const char _eeprom_99[] PROGMEM = "SW_TOP_LOOP_TARGET";
const char _eeprom_100[] PROGMEM = "SW_TOP_BANK_LEFT_TARGET";
const char _eeprom_101[] PROGMEM = "SW_TOP_BANK_CENTER_TARGET";
const char _eeprom_102[] PROGMEM = "SW_TOP_BANK_RIGHT_TARGET";
const char _eeprom_103[] PROGMEM = "SW_RIGHT_BANK_LEFT_TARGET";
const char _eeprom_104[] PROGMEM = "SW_RIGHT_BANK_CENTER_TARGET";
const char _eeprom_105[] PROGMEM = "SW_RIGHT_BANK_RIGHT_TARGET";
const char _eeprom_106[] PROGMEM = "SW_TOP_POP_BUMPER";
const char _eeprom_107[] PROGMEM = "SW_MAIN_POP_BUMPER";
const char _eeprom_108[] PROGMEM = "SW_SKILL_SHOT_TARGET";
const char _eeprom_109[] PROGMEM = "SW_LEFT_BANK_TARGET_1";
const char _eeprom_110[] PROGMEM = "SW_LEFT_BANK_TARGET_2";
const char _eeprom_111[] PROGMEM = "SW_LEFT_BANK_TARGET_3";
const char _eeprom_112[] PROGMEM = "SW_LEFT_BANK_TARGET_4";
const char _eeprom_113[] PROGMEM = "SW_LEFT_BANK_TARGET_5";
const char _eeprom_114[] PROGMEM = "SW_LEFT_BANK_TARGET_6";
const char _eeprom_115[] PROGMEM = "SW_LEFT_BANK_TARGET_7";
const char _eeprom_116[] PROGMEM = "SW_LEFT_BANK_TARGET_8";
const char _eeprom_117[] PROGMEM = "SW_LEFT_LANE_CAPTURED_BALL";
const char _eeprom_118[] PROGMEM = "SW_LEFT_LANE_SECOND_BALL";
const char _eeprom_119[] PROGMEM = "SW_LEFT_LANE_THIRD_BALL";
const char _eeprom_120[] PROGMEM = "SW_LEFT_LANE_EXIT";
const char _eeprom_121[] PROGMEM = "SW_SPINNER";
const char _eeprom_122[] PROGMEM = "SW_LEFT_SLINGSHOT";
const char _eeprom_123[] PROGMEM = "SW_RIGHT_SLINGSHOT";
const char _eeprom_124[] PROGMEM = "SW_LEFT_OUTLANE";
const char _eeprom_125[] PROGMEM = "SW_RIGHT_OUTLANE";
const char _eeprom_126[] PROGMEM = "SW_LEFT_INLANE";
const char _eeprom_127[] PROGMEM = "SW_RIGHT_INLANE";
const char _eeprom_128[] PROGMEM = "SW_OUTHOLE_LEFT";
const char _eeprom_129[] PROGMEM = "SW_OUTHOLE_MIDDLE";
const char _eeprom_130[] PROGMEM = "SW_OUTHOLE_RIGHT";
const char _eeprom_131[] PROGMEM = "SW_SHOOTING_LANE";
const char _eeprom_132[] PROGMEM = "SW_START";
const char _eeprom_133[] PROGMEM = "SW_MENU_LEFT";
const char _eeprom_134[] PROGMEM = "SW_MENU_RIGHT";
const char _eeprom_135[] PROGMEM = "Switch closed";
const char _eeprom_136[] PROGMEM = "Switch opened";
const char _eeprom_137[] PROGMEM = "Switch clicked";







const char *const _eeprom_strings_table[] PROGMEM = {
    _eeprom_0,
    _eeprom_1,
    _eeprom_2,
    _eeprom_3,
    _eeprom_4,
    _eeprom_5,
    _eeprom_6,
    _eeprom_7,
    _eeprom_8,
    _eeprom_9,
    _eeprom_10,
    _eeprom_11,
    _eeprom_12,
    _eeprom_13,
    _eeprom_14,
    _eeprom_15,
    _eeprom_16,
    _eeprom_17,
    _eeprom_18,
    _eeprom_19,
    _eeprom_20,
    _eeprom_21,
    _eeprom_22,
    _eeprom_23,
    _eeprom_24,
    _eeprom_25,
    _eeprom_26,
    _eeprom_27,
    _eeprom_28,
    _eeprom_29,
    _eeprom_30,
    _eeprom_31,
    _eeprom_32,
    _eeprom_33,
    _eeprom_34,
    _eeprom_35,
    _eeprom_36,
    _eeprom_37,
    _eeprom_38,
    _eeprom_39,
    _eeprom_40,
    _eeprom_41,
    _eeprom_42,
    _eeprom_43,
    _eeprom_44,
    _eeprom_45,
    _eeprom_46,
    _eeprom_47,
    _eeprom_48,
    _eeprom_49,
    _eeprom_50,
    _eeprom_51,
    _eeprom_52,
    _eeprom_53,
    _eeprom_54,
    _eeprom_55,
    _eeprom_56,
    _eeprom_57,
    _eeprom_58,
    _eeprom_59,
    _eeprom_60,
    _eeprom_61,
    _eeprom_62,
    _eeprom_63,
    _eeprom_64,
    _eeprom_65,
    _eeprom_66,
    _eeprom_67,
    _eeprom_68,
    _eeprom_69,
    _eeprom_70,
    _eeprom_71,
    _eeprom_72,
    _eeprom_73,
    _eeprom_74,
    _eeprom_75,
    _eeprom_76,
    _eeprom_77,
    _eeprom_78,
    _eeprom_79,
    _eeprom_80,
    _eeprom_81,
    _eeprom_82,
    _eeprom_83,
    _eeprom_84,
    _eeprom_85,
    _eeprom_86,
    _eeprom_87,
    _eeprom_88,
    _eeprom_89,
    _eeprom_90,
    _eeprom_91,
    _eeprom_92,
    _eeprom_93,
    _eeprom_94,
    _eeprom_95,
    _eeprom_96,
    _eeprom_97,
    _eeprom_98,
    _eeprom_99,
    _eeprom_100,
    _eeprom_101,
    _eeprom_102,
    _eeprom_103,
    _eeprom_104,
    _eeprom_105,
    _eeprom_106,
    _eeprom_107,
    _eeprom_108,
    _eeprom_109,
    _eeprom_110,
    _eeprom_111,
    _eeprom_112,
    _eeprom_113,
    _eeprom_114,
    _eeprom_115,
    _eeprom_116,
    _eeprom_117,
    _eeprom_118,
    _eeprom_119,
    _eeprom_120,
    _eeprom_121,
    _eeprom_122,
    _eeprom_123,
    _eeprom_124,
    _eeprom_125,
    _eeprom_126,
    _eeprom_127,
    _eeprom_128,
    _eeprom_129,
    _eeprom_130,
    _eeprom_131,
    _eeprom_132,
    _eeprom_133,
    _eeprom_134,
};


void get_progmem_message(byte message_no, char *output, byte output_size)
{
    strncpy_P(output, (char *)pgm_read_word(&(_eeprom_strings_table[message_no])), output_size);
}




#endif // MESSAGES_H
