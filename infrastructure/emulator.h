#ifdef RUN_SERIAL_EMULATOR



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


EmulatorClass::EmulatorClass() {
    gotten_tiddle = false;
    switch_pending_opening = 0xFF; // none
}

void EmulatorClass::set_switches_buffers(byte *buffers) {
    // this function allows switch_matrix to depend on emulator
    // and frees emulator from depending on switch_matrix.
    // aiding with the #include sequence
    switches_buffers = buffers;
}

void EmulatorClass::keys_help() {
    LOGM(M_EMULATOR_SCREEN_01);
    LOGM(M_EMULATOR_SCREEN_02);
    LOGM(M_EMULATOR_SCREEN_03);
    LOGM(M_EMULATOR_SCREEN_04);
    LOGM(M_EMULATOR_SCREEN_05);
    LOGM(M_EMULATOR_SCREEN_06);
    LOGM(M_EMULATOR_SCREEN_07);
    LOGM(M_EMULATOR_SCREEN_08);
    LOGM(M_EMULATOR_SCREEN_09);
    LOGM(M_EMULATOR_SCREEN_10);
    LOGM(M_EMULATOR_SCREEN_11);
    LOGM(M_EMULATOR_SCREEN_12);
    LOGM(M_EMULATOR_SCREEN_13);
    LOGM(M_EMULATOR_SCREEN_14);
    LOGM(M_EMULATOR_SCREEN_15);
    LOGM(M_EMULATOR_SCREEN_16);
    LOGM(M_EMULATOR_SCREEN_17);
    LOGM(M_EMULATOR_SCREEN_18);
    LOGM(M_EMULATOR_SCREEN_19);
    LOGM(M_EMULATOR_SCREEN_20);
    LOGM(M_EMULATOR_SCREEN_21);
    LOGM(M_EMULATOR_SCREEN_22);
    LOGM(M_EMULATOR_SCREEN_23);
    LOGM(M_EMULATOR_SCREEN_24);
    LOGM(M_EMULATOR_SCREEN_25);
}

void EmulatorClass::show_status() {

}

byte EmulatorClass::get_mapping(char chr) {
    for (byte i = 0; i < (sizeof(keymap) / sizeof(keymap[0])); i++) {
        if (keymap[i].chr == chr)
            return i;
    }
    return 0xFF;
}

bool EmulatorClass::get_next_switch_event(byte *p_switch_no, byte *p_is_closed) {

    // see if we have a switch opening pending
    if (switch_pending_opening != 0xFF) {
        // don't log here, to avoid excess noise
        *p_switch_no = switch_pending_opening;
        *p_is_closed = false;
        switch_pending_opening = 0xFF;
        return true;
    }

    // then see if user sent something
    if (Serial.available() == 0)
        return false;

    byte received = Serial.read();
    char c = (char)received;
    if (c == '?') {
        keys_help();
        return false;
    } else if (c == '/') {
        show_status();
        return false;
    } else if (c == '~') {
        gotten_tiddle = true;
        return false;
    }

    byte mapping = get_mapping(c);
    if (mapping == 0xFF)
        return false; // invalid keys ignored

    register byte buffer_index = keymap[mapping].switch_no >> 3;
    register byte bit_offset = keymap[mapping].switch_no & 0x7;
    bool already_closed = (switches_buffers[buffer_index] >> bit_offset) & 0x01;
    if (already_closed) {
        // emulate switch opening
        LOGM(M_SWITCH_OPENED);
        LOGM(keymap[mapping].eeprom_message_no);
        *p_switch_no = keymap[mapping].switch_no;
        *p_is_closed = false;
    } else {
        // we will either keep it closed or just emulate a "click" (down
        if (gotten_tiddle) {
            // a tiddle keeps the switch closed.
            LOGM(M_SWITCH_CLOSED);
            LOGM(keymap[mapping].eeprom_message_no);
            *p_switch_no = keymap[mapping].switch_no;
            *p_is_closed = true;
            gotten_tiddle = false;
        } else {
            // the usual case is to close and open right after, emulating a "click"
            LOGM(M_SWITCH_CLICKED);
            LOGM(keymap[mapping].eeprom_message_no);
            *p_switch_no = keymap[mapping].switch_no;
            *p_is_closed = true;
            switch_pending_opening = keymap[mapping].switch_no;
        }
    }

    return true;
}


#endif // RUN_SERIAL_EMULATOR
