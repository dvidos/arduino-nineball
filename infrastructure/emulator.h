#ifdef RUN_SERIAL_EMULATOR



class EmulatorClass
{
public:
    struct char_switch_association {
        char chr;
        byte switch_no;
        const char *name;
    } keymap[40] = {
        {'q', SW_TOP_LOOP_PASS, "SW_TOP_LOOP_PASS"},
        {'w', SW_TOP_LOOP_TARGET, "SW_TOP_LOOP_TARGET"},
        {'e', SW_TOP_BANK_LEFT_TARGET, "SW_TOP_BANK_LEFT_TARGET"},
        {'r', SW_TOP_BANK_CENTER_TARGET, "SW_TOP_BANK_CENTER_TARGET"},
        {'t', SW_TOP_BANK_RIGHT_TARGET, "SW_TOP_BANK_RIGHT_TARGET"},
        {'i', SW_RIGHT_BANK_LEFT_TARGET, "SW_RIGHT_BANK_LEFT_TARGET"},
        {'o', SW_RIGHT_BANK_CENTER_TARGET, "SW_RIGHT_BANK_CENTER_TARGET"},
        {'p', SW_RIGHT_BANK_RIGHT_TARGET, "SW_RIGHT_BANK_RIGHT_TARGET"},
        {'u', SW_TOP_POP_BUMPER, "SW_TOP_POP_BUMPER"},
        {'b', SW_MAIN_POP_BUMPER, "SW_MAIN_POP_BUMPER"},
        {'y', SW_SKILL_SHOT_TARGET, "SW_SKILL_SHOT_TARGET"},

        {'1', SW_LEFT_BANK_TARGET_1, "SW_LEFT_BANK_TARGET_1"},
        {'2', SW_LEFT_BANK_TARGET_2, "SW_LEFT_BANK_TARGET_2"},
        {'3', SW_LEFT_BANK_TARGET_3, "SW_LEFT_BANK_TARGET_3"},
        {'4', SW_LEFT_BANK_TARGET_4, "SW_LEFT_BANK_TARGET_4"},
        {'5', SW_LEFT_BANK_TARGET_5, "SW_LEFT_BANK_TARGET_5"},
        {'6', SW_LEFT_BANK_TARGET_6, "SW_LEFT_BANK_TARGET_6"},
        {'7', SW_LEFT_BANK_TARGET_7, "SW_LEFT_BANK_TARGET_7"},
        {'8', SW_LEFT_BANK_TARGET_8, "SW_LEFT_BANK_TARGET_8"},

        {'d', SW_LEFT_LANE_CAPTURED_BALL, "SW_LEFT_LANE_CAPTURED_BALL"},
        {'s', SW_LEFT_LANE_SECOND_BALL, "SW_LEFT_LANE_SECOND_BALL"},
        {'a', SW_LEFT_LANE_THIRD_BALL, "SW_LEFT_LANE_THIRD_BALL"},
        {'f', SW_LEFT_LANE_EXIT, "SW_LEFT_LANE_EXIT"},
        {'m', SW_SPINNER, "SW_SPINNER"},
        {'j', SW_LEFT_SLINGSHOT, "SW_LEFT_SLINGSHOT"},
        {'k', SW_RIGHT_SLINGSHOT, "SW_RIGHT_SLINGSHOT"},
        {'g', SW_LEFT_OUTLANE, "SW_LEFT_OUTLANE"},
        {';', SW_RIGHT_OUTLANE, "SW_LEFT_INLANE"},
        {'h', SW_LEFT_INLANE, "SW_RIGHT_INLANE"},
        {'k', SW_RIGHT_INLANE, "SW_RIGHT_INLANE"},
        {'z', SW_OUTHOLE_LEFT, "SW_OUTHOLE_LEFT"},
        {'x', SW_OUTHOLE_MIDDLE, "SW_OUTHOLE_MIDDLE"},
        {'c', SW_OUTHOLE_RIGHT, "SW_OUTHOLE_RIGHT"},
        {'v', SW_SHOOTING_LANE, "SW_SHOOTING_LANE"},
        {'\\', SW_START, "SW_START"},
        {'[', SW_MENU_LEFT, "SW_MENU_LEFT"},
        {']', SW_MENU_RIGHT, "SW_MENU_RIGHT"},
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
    LOG("Available keys");
    // lets do 3 columns, gives us 10 lines, each column could be up to 25 characters
    char line[128];
    byte array_size = sizeof(keymap) / sizeof(keymap[0]);
    LOG("+-------------------------------+-------------------------------+-------------------------------");
    for (int i = 0; i < 13; i++) {
        sprintf(line, "| %c %-28s| %c %-28s| %c %-28s |",
            ( 0 + i < array_size) ? keymap[ 0 + i].chr : ' ',
            ( 0 + i < array_size) ? keymap[ 0 + i].name : "",
            (13 + i < array_size) ? keymap[13 + i].chr : ' ',
            (13 + i < array_size) ? keymap[13 + i].name : "",
            (26 + i < array_size) ? keymap[26 + i].chr : ' ',
            (26 + i < array_size) ? keymap[26 + i].name : "");
        LOG(line);
    }
    LOG("+-------------------------------+-------------------------------+-------------------------------");
    LOG("Tiddle keeps next switch closed (no opening event is generated)");
}

void EmulatorClass::show_status() {

    #define DROP_TARGET(no)     (SwitchMatrix.is_switch_closed(no) ? '_' : '#')
    #define LAMP(no)            (LampMatrix.is_on(no) ? '@' : '.')

    LOG("+----------------------------+");
    LOG("| Q           _ _ _ Y<*      |");
    LOG("|  W * 173    * * *     \\    |");
    LOG("|     * 40    E R T  (U*)\\   |");
    LOG("|      * 30               \\  |");
    LOG("|       * 20          I *_ | |");
    LOG("|  <*    * 10   ( )   O *_ | |");
    LOG("|A                    P *_ | |");
    LOG("|S _2 * * *       * 4X     | |");
    LOG("|D _4 * * *       * 2X     | |");
    LOG("|  _6 * * *       * 1X     | |");
    LOG("|  _8 * * *                | |");
    LOG("|  9 *     * Sp     --[M]--| |");
    LOG("|  _7 * * *         * 2500 | |");
    LOG("|  _5 * * *         * 1600 | |");
    LOG("|F _3 * * *        * 900   | |");
    LOG("|  _1 * *         * 400    | |");
    LOG("|     *77K   *8            | |");
    LOG("|  *  \\    *6  *7    /  *  | |");
    LOG("| |H| J\\ *4  *9  *5 /K |L| | |");
    LOG("|G| |   \\  *2  *3  /   | |;| |");
    LOG("|*           *1           *| |");
    LOG("|          * ShAgn         |V|");
    LOG("+------------------Z-X-C---+-+");
    LOG("\\:Start  [:P1  ]:P2  ?:Help  /:Status  ~:Keep next switch down");
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

    //LOG("Emulator, switches bytes are 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
    //        switches_buffers[0], switches_buffers[1], switches_buffers[2], switches_buffers[3], switches_buffers[4]);

    register byte buffer_index = keymap[mapping].switch_no >> 3;
    register byte bit_offset = keymap[mapping].switch_no & 0x7;
    bool already_closed = (switches_buffers[buffer_index] >> bit_offset) & 0x01;
    if (already_closed) {
        // emulate switch opening
        LOG("%s opened", keymap[mapping].name);
        *p_switch_no = keymap[mapping].switch_no;
        *p_is_closed = false;
    } else {
        // we will either keep it closed or just emulate a "click" (down
        if (gotten_tiddle) {
            // a tiddle keeps the switch closed.
            LOG("%s closed", keymap[mapping].name);
            *p_switch_no = keymap[mapping].switch_no;
            *p_is_closed = true;
            gotten_tiddle = false;
        } else {
            // the usual case is to close and open right after, emulating a "click"
            LOG("%s clicked", keymap[mapping].name);
            *p_switch_no = keymap[mapping].switch_no;
            *p_is_closed = true;
            switch_pending_opening = keymap[mapping].switch_no;
        }
    }

    return true;
}


#endif // RUN_SERIAL_EMULATOR
