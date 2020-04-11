#ifdef RUN_SERIAL_EMULATOR


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
    // "#  Profile   CurScore    Ball   Made6X   SprBns    HiScr1   HiScr2   HiScr3"
    // "1     0      00112233     3       Y        Y         Y        Y        Y"

    LOG("#  Profile   CurScore    Ball   Made6X   SprBns    HiScr1   HiScr2   HiScr3");
    for (byte i = 0; i < 3; i++) {
        LOG("%d     %d      %02X%02X%02X%02X     %d       %c        %c         %c        %c        %c",
            i + 1,
            Gameplay.player_info[i].profile_number,
            Gameplay.player_info[i].score.bcd[0],
            Gameplay.player_info[i].score.bcd[1],
            Gameplay.player_info[i].score.bcd[2],
            Gameplay.player_info[i].score.bcd[3],
            Gameplay.player_info[i].ball_number,
            Gameplay.player_info[i].achieved_6x_bonus_multiplier ? 'Y' : 'N',
            Gameplay.player_info[i].super_bonus_made_prev_ball ? 'Y' : 'N',
            Gameplay.player_info[i].achieved_l1_highscore ? 'Y' : 'N',
            Gameplay.player_info[i].achieved_l2_highscore ? 'Y' : 'N',
            Gameplay.player_info[i].achieved_l3_highscore ? 'Y' : 'N'
        );
    }

    LOG("--------------------------------------");
    LOG("Gameplay mode                   : %d", Gameplay.mode);
    LOG("Gameplay current_player         : %d (0 - 3)", Gameplay.current_player);
    LOG("Gameplay num_players            : %d", Gameplay.num_players);
    LOG("--------------------------------------");
    LOG("EightBank.object_made           : %d (0=none, 1-9)", EightBankTargets.object_made);
    LOG("EightBank.spot_number_enabled   : %c", EightBankTargets.spot_number_enabled ? 'Y' : 'N');
    LOG("EightBank.left_inlane_enabled   : %c", EightBankTargets.left_inlane_enabled ? 'Y' : 'N');
    LOG("EightBank.right_inlane_enabled  : %c", EightBankTargets.right_inlane_enabled ? 'Y' : 'N');
    LOG("EightBank.special_made          : %c", EightBankTargets.special_made ? 'Y' : 'N');
    LOG("EightBank.super_bonus_made      : %c", EightBankTargets.super_bonus_made ? 'Y' : 'N');
    LOG("BonusMultipler.value            : %d", BonusMultiplier.get_multiplier());
    LOG("--------------------------------------");
    LOG("ThreeBank.lit_pink_target       : %d (0=off, 1, 2)", ThreeBankTargets.lit_pink_target);
    LOG("ThreeBank.lit_wow_target        : %d (0=off, 1 - 4)", ThreeBankTargets.lit_wow_target);
    LOG("LoopTarget.value                : %d (0 - 4)", LoopTarget.value);
    LOG("LoopTarget.left_outlane         : %c", LoopTarget.left_outlane ? 'Y' : 'N');
    LOG("LoopTarget.right_outlane        : %c", LoopTarget.right_outlane ? 'Y' : 'N');
    LOG("Spinner.value                   : %d (0 - 4)", Spinner.value);
    LOG("BallKeeper ball_capture_enabled : %c", BallKeeper.ball_capture_enabled ? 'Y' : 'N');
    LOG("BallKeeper shoot_agains         : %d", BallKeeper.shoot_agains);
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
