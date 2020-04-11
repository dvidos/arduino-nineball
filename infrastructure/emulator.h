#ifdef RUN_SERIAL_EMULATOR
#ifndef EMULATOR_H
#define EMULATOR_H


class EmulatorClass
{
public:
    struct char_switch_association {
        char chr;
        byte switch_no;
        byte eeprom_message_no;
    } keymap[40] = {
        {'q', SW_TOP_LOOP_PASS, M_SW_TOP_LOOP_PASS_NAME},
        {'w', SW_TOP_LOOP_TARGET, M_SW_TOP_LOOP_TARGET_NAME},
        {'e', SW_TOP_BANK_LEFT_TARGET, M_SW_TOP_BANK_LEFT_TARGET_NAME},
        {'r', SW_TOP_BANK_CENTER_TARGET, M_SW_TOP_BANK_CENTER_TARGET_NAME},
        {'t', SW_TOP_BANK_RIGHT_TARGET, M_SW_TOP_BANK_RIGHT_TARGET_NAME},
        {'i', SW_RIGHT_BANK_LEFT_TARGET, M_SW_RIGHT_BANK_LEFT_TARGET_NAME},
        {'o', SW_RIGHT_BANK_CENTER_TARGET, M_SW_RIGHT_BANK_CENTER_TARGET_NAME},
        {'p', SW_RIGHT_BANK_RIGHT_TARGET, M_SW_RIGHT_BANK_RIGHT_TARGET_NAME},
        {'u', SW_TOP_POP_BUMPER, M_SW_TOP_POP_BUMPER_NAME},
        {'b', SW_MAIN_POP_BUMPER, M_SW_MAIN_POP_BUMPER_NAME},
        {'y', SW_SKILL_SHOT_TARGET, M_SW_SKILL_SHOT_TARGET_NAME},

        {'1', SW_LEFT_BANK_TARGET_1, M_SW_LEFT_BANK_TARGET_1_NAME},
        {'2', SW_LEFT_BANK_TARGET_2, M_SW_LEFT_BANK_TARGET_2_NAME},
        {'3', SW_LEFT_BANK_TARGET_3, M_SW_LEFT_BANK_TARGET_3_NAME},
        {'4', SW_LEFT_BANK_TARGET_4, M_SW_LEFT_BANK_TARGET_4_NAME},
        {'5', SW_LEFT_BANK_TARGET_5, M_SW_LEFT_BANK_TARGET_5_NAME},
        {'6', SW_LEFT_BANK_TARGET_6, M_SW_LEFT_BANK_TARGET_6_NAME},
        {'7', SW_LEFT_BANK_TARGET_7, M_SW_LEFT_BANK_TARGET_7_NAME},
        {'8', SW_LEFT_BANK_TARGET_8, M_SW_LEFT_BANK_TARGET_8_NAME},

        {'d', SW_LEFT_LANE_CAPTURED_BALL, M_SW_LEFT_LANE_CAPTURED_BALL_NAME},
        {'s', SW_LEFT_LANE_SECOND_BALL, M_SW_LEFT_LANE_SECOND_BALL_NAME},
        {'a', SW_LEFT_LANE_THIRD_BALL, M_SW_LEFT_LANE_THIRD_BALL_NAME},
        {'f', SW_LEFT_LANE_EXIT, M_SW_LEFT_LANE_EXIT_NAME},
        {'m', SW_SPINNER, M_SW_SPINNER_NAME},
        {'j', SW_LEFT_SLINGSHOT, M_SW_LEFT_SLINGSHOT_NAME},
        {'k', SW_RIGHT_SLINGSHOT, M_SW_RIGHT_SLINGSHOT_NAME},
        {'g', SW_LEFT_OUTLANE, M_SW_LEFT_OUTLANE_NAME},
        {';', SW_RIGHT_OUTLANE, M_SW_LEFT_INLANE_NAME},
        {'h', SW_LEFT_INLANE, M_SW_RIGHT_INLANE_NAME},
        {'k', SW_RIGHT_INLANE, M_SW_RIGHT_INLANE_NAME},
        {'z', SW_OUTHOLE_LEFT, M_SW_OUTHOLE_LEFT_NAME},
        {'x', SW_OUTHOLE_MIDDLE, M_SW_OUTHOLE_MIDDLE_NAME},
        {'c', SW_OUTHOLE_RIGHT, M_SW_OUTHOLE_RIGHT_NAME},
        {'v', SW_SHOOTING_LANE, M_SW_SHOOTING_LANE_NAME},
        {'\\', SW_START, M_SW_START_NAME},
        {'[', SW_MENU_LEFT, M_SW_MENU_LEFT_NAME},
        {']', SW_MENU_RIGHT, M_SW_MENU_RIGHT_NAME},
    };

    byte gotten_tiddle: 1;
    byte switch_pending_opening;
    volatile byte *switches_buffers;

    EmulatorClass();
    void set_switches_buffers(byte *buffers);
    void keys_help();
    void show_status();
    byte get_mapping(char chr);
    bool get_next_switch_event(byte *p_switch_no, byte *p_is_closed);
};




#endif // EMULATOR_H
#endif // RUN_SERIAL_EMULATOR
