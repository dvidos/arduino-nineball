#ifdef RUN_SERIAL_EMULATOR



class EmulatorClass
{
public:
    struct char_switch_association {
        char chr;
        byte switch_no;
        const char *name;
    } keymap[40] = {
        {'q', SW_LEFT_LANE_THIRD_BALL, "SW_LEFT_LANE_THIRD_BALL"},
        {'a', SW_LEFT_LANE_SECOND_BALL, "SW_LEFT_LANE_SECOND_BALL"},
        {'z', SW_LEFT_LANE_THIRD_BALL, "SW_LEFT_LANE_THIRD_BALL"},
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
    LOG("--------------");
    // lets do 4 columns, gives us 10 lines, each column could be up to 25 characters
    char line[128];
    for (int i = 0; i < 10; i++) {
        sprintf(line, "%c %-22s| %c %-22s| %c %-22s| %c %-22s",
            keymap[ 0 + i].chr, keymap[ 0 + i].name,
            keymap[10 + i].chr, keymap[10 + i].name,
            keymap[20 + i].chr, keymap[20 + i].name,
            keymap[30 + i].chr, keymap[30 + i].name);
        LOG(line);
    }
    LOG("Tiddle keeps next switch closed (no opening event is generated)");
}

void EmulatorClass::show_status() {
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
        gotten_tiddle = 1;
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
