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


const char _eeprom_00[] PROGMEM = "String 0";
const char _eeprom_01[] PROGMEM = "*** FATAL %d ***";
const char _eeprom_02[] PROGMEM = "** Failed asserting that \"%s\"";
const char _eeprom_03[] PROGMEM = "String 3";
const char _eeprom_04[] PROGMEM = "int value: %d";
const char _eeprom_05[] PROGMEM = "setup() starting";
const char _eeprom_06[] PROGMEM = "setup() finished";
const char _eeprom_07[] PROGMEM = "Running Serial Emulator, send \"?\" for help";
const char _eeprom_08[] PROGMEM = "Attract starting idle mode";
const char _eeprom_09[] PROGMEM = "Attract starting game mode";
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

const char *const _eeprom_strings_table[] PROGMEM = {
    _eeprom_00,
    _eeprom_01,
    _eeprom_02,
    _eeprom_03,
    _eeprom_04,
    _eeprom_05,
    _eeprom_06,
    _eeprom_07,
    _eeprom_09,
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
};


void get_progmem_message(byte message_no, char *output, byte output_size)
{
    strncpy_P(output, (char *)pgm_read_word(&(_eeprom_strings_table[message_no])), output_size);
}




#endif // MESSAGES_H

