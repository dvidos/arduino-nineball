

void EightBankTargetsClass::init(bool super_bonus_made_previous_ball)
{
    spot_number_enabled = 0;
    left_inlane_enabled = 0;
    right_inlane_enabled = 0;
    object_made = 0;

    wow_number = 0;
    special_number = 0;
    special_made = 0;
    super_bonus_made = 0; // this is for next round
    bringing_targets_up_tries = 0;

    set_target_lamps(1);
    set_wow_lamps(0);
    set_special_lamps(0);
    set_bonus_lamps(0);

    LampMatrix.lamp_off(LAMP_OBJECT_9);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
    LampMatrix.set_lamp(LAMP_SUPER_BONUS_77K, super_bonus_made_previous_ball);
    LampMatrix.lamp_off(LAMP_ALL_TARGETS_DOWN_SPECIAL);

    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);

    // let the user know what to hit.
    Animator.start_blinking(objects[1].target_lamp_no);
}

byte EightBankTargetsClass::get_object_made()
{
    return object_made;
}

bool EightBankTargetsClass::get_super_bonus_made()
{
    return super_bonus_made;
}

void EightBankTargetsClass::on_target_hit(byte switch_no)
{
    // let's deduce the number hit, so we can programmatically
    // calculate things without a ton of conditions

    byte target_no = get_target_number(switch_no);
    if (target_no >= 1 && target_no <= 8)
    {
        if (target_no == special_number) {
            award_special(target_no);
        } else if (target_no == wow_number) {
            award_wow(target_no);
        } else if (target_no == number_nine_number) {
            award_number_nine(target_no);
        } else if (target_no == object_made + 1) {
            make_current_target_object(target_no);
        } else {
            // he just hit a drop target.
            Audio.play(SOUND_FX_6);
            Gameplay.add_score_bcd(0x1000);
        }

        // bring things up for the next number / sequence
        if (any_drop_target_down())
            bring_drop_targets_up();
    }
    else if ((switch_no == SW_LEFT_INLANE) ||
            (switch_no == SW_RIGHT_INLANE) ||
            (switch_no == SW_TOP_POP_BUMPER))
    {
        Audio.play(SOUND_FX_6);
        Gameplay.add_score_bcd(0x500);

        if (object_made < 8) {
            if ((switch_no == SW_LEFT_INLANE && left_inlane_enabled) ||
                (switch_no == SW_RIGHT_INLANE && right_inlane_enabled) ||
                (switch_no == SW_TOP_POP_BUMPER && spot_number_enabled)) {
                make_current_target_object();
            }
        }

        // don't turn off the "spot number" mode,
        // this allows more than one objects to be made quickly.

        // bring things up for the next number / sequence
        if (any_drop_target_down())
            bring_drop_targets_up();
    }
}

byte EightBankTargetsClass::get_target_number(byte switch_no) // return 0 if not found
{
    for (byte target_no = 1; target_no <= 8; target_no++) {
        if (objects[target_no].switch_no == switch_no)
            return target_no;
    }

    return 0;
}

inline void EightBankTargetsClass::set_target_lamps(bool value)
{
    for (byte i = 1; i <= 8; i++)
        LampMatrix.set_lamp(objects[i].target_lamp_no, value);
}

inline void EightBankTargetsClass::set_wow_lamps(bool value)
{
    for (byte i = 1; i <= 8; i++)
        LampMatrix.set_lamp(objects[i].wow_lamp_no, value);
}

inline void EightBankTargetsClass::set_special_lamps(bool value)
{
    // note, starting at 2.
    for (byte i = 2; i <= 8; i++)
        LampMatrix.set_lamp(objects[i].special_lamp_no, value);
}

inline void EightBankTargetsClass::set_bonus_lamps(bool value)
{
    for (byte i = 1; i <= 8; i++)
        LampMatrix.set_lamp(objects[i].bonus_lamp_no, value);
}

bool EightBankTargetsClass::all_drop_targets_down()
{
    return
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_1) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_2) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_3) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_4) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_5) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_6) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_7) &&
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_8);
}

bool EightBankTargetsClass::any_drop_target_down()
{
    return
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_1) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_2) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_3) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_4) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_5) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_6) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_7) ||
        SwitchMatrix.is_switch_closed(SW_LEFT_BANK_TARGET_8);
}

void EightBankTargetsClass::bring_drop_targets_up()
{
    bringing_targets_up_tries = 0;
    Coils.fire_left_bank_reset();
    TimeKeeper.callback_later(verify_drop_targets_are_up, 600);
}

void EightBankTargetsClass::verify_drop_targets_are_up()
{
    if (!EightBankTargets.any_drop_target_down())
        return; // we are successful

    if (EightBankTargets.bringing_targets_up_tries == 3) {
        LOGM(M_FAILED_BRINGING_EIGHT_BANK_TARGETS_UP);
        return; // give up
    }

    EightBankTargets.bringing_targets_up_tries += 1;
    Coils.fire_left_bank_reset();
    TimeKeeper.callback_later(verify_drop_targets_are_up, 600);
}

void EightBankTargetsClass::make_current_target_object(byte target_no)
{
    if (object_made >= 8)
        return;

    // award No target
    Gameplay.add_score_bcd(0x1000);
    Animator.stop_blinking();

    if (object_made > 0) {
        // blink a little, then turn off
        Animator.blink_a_little(objects[target_no].target_lamp_no, 0);
    }

    object_made += 1;
    LOGM(M_MADE_CURRENT_OBJECT_NO_D, object_made);

    // turn on bonus made
    LampMatrix.lamp_on(objects[object_made].bonus_lamp_no);

    // enable capturing
    if (object_made == 5) {
        BallKeeper.enable_ball_capturing();
    }

    // start blinking the next object
    if (object_made < 8) {
        Animator.start_blinking(objects[object_made + 1].target_lamp_no);
    }

    // if next is 9, launch what's needed
    if (object_made == 8) {
        Animator.start_blinking(LAMP_OBJECT_9);
        start_number_nine_sequence();

        if (GameSettings.eight_bank_wow_turn_on == 1) // start on 8
            start_wow_sequence();

        if (GameSettings.when_super_bonus_lights == 1) { // award on 8
            super_bonus_made = 1;
        }
    }
}

void EightBankTargetsClass::award_number_nine(byte target_no)
{
    Audio.play(SOUND_FX_6);
    object_made = 9;
    LOGM(M_MADE_OBJECT_NO_9);

    // turn off all drop target lamps
    set_target_lamps(0);
    Animator.blink_a_little(objects[target_no].target_lamp_no, 0);

    // turn on the Bonus lamp No 9
    Animator.stop_blinking();
    LampMatrix.lamp_on(LAMP_OBJECT_9);
    LampMatrix.lamp_on(LAMP_BONUS_MADE_9);

    if (GameSettings.eight_bank_wow_turn_on == 0) // start on 9
        start_wow_sequence();

    if (GameSettings.when_super_bonus_lights == 0) { // award on 9
        super_bonus_made = 1;
    }

    // this also starts multiball!!!
    BallKeeper.release_captured_balls();

    // special starts only when making WOW, not on 9.
}

void EightBankTargetsClass::award_wow(byte target_no)
{
    // award wow
    Audio.play(SOUND_FX_2);

    if (GameSettings.wow_award_type == 0) {
        Gameplay.add_score_bcd(0x70000);
    } else if (GameSettings.wow_award_type == 1) {
        BallKeeper.grant_shoot_again();
    }

    // turn off lamps (maybe blink the one for a while?)
    set_wow_lamps(0);

    // ok, let's blink a little, then turn off
    Animator.blink_a_little(objects[target_no].wow_lamp_no, 0);

    // special starts only when making WOW, not on 9.
    start_special_sequence();
}

void EightBankTargetsClass::award_special(byte target_no)
{
    // award special
    Audio.play(SOUND_FX_2);

    if (GameSettings.special_award_type == 0) {
        Gameplay.add_score_bcd(0x90000);
    } else if (GameSettings.special_award_type == 1) {
        Gameplay.add_score_bcd(0x130000);
    } else if (GameSettings.special_award_type == 2) {
        BallKeeper.grant_shoot_again();
    }

    special_made = 1;

    set_special_lamps(0);

    // ok, let's blink a little!
    Animator.blink_a_little(objects[target_no].special_lamp_no, 0);
    Animator.blink_a_little(LAMP_ALL_TARGETS_DOWN_SPECIAL, 1);

    // we need to see if special will launch again.
    if (GameSettings.unlimited_specials) {
        start_special_sequence();
    }
}

void EightBankTargetsClass::start_number_nine_sequence()
{
    LOGM(M_STARTING_NO_9_SEQUENCE);
    number_nine_number = get_next_target_number(0);

    LampMatrix.set_lamp(LAMP_OBJECT_1_DROP_TARGET, number_nine_number == 1);
    LampMatrix.set_lamp(LAMP_OBJECT_2_DROP_TARGET, number_nine_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_DROP_TARGET, number_nine_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_DROP_TARGET, number_nine_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_DROP_TARGET, number_nine_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_DROP_TARGET, number_nine_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_DROP_TARGET, number_nine_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_DROP_TARGET, number_nine_number == 8);

    TimeKeeper.callback_later(advance_number_nine_target, 1200);
}


void EightBankTargetsClass::start_wow_sequence()
{
    LOGM(M_STARTING_8_BANK_WOW_SEQUENCE);
    wow_number = get_next_target_number(0);

    LampMatrix.set_lamp(LAMP_OBJECT_1_WOW, wow_number == 1);
    LampMatrix.set_lamp(LAMP_OBJECT_2_WOW, wow_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_WOW, wow_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_WOW, wow_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_WOW, wow_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_WOW, wow_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_WOW, wow_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_WOW, wow_number == 8);

    TimeKeeper.callback_later(advance_wow_target, 1000);
}

void EightBankTargetsClass::start_special_sequence()
{
    LOGM(M_STARTING_8_BANK_SPECIAL_SEQUENCE);
    special_number = get_next_target_number(0);

    LampMatrix.set_lamp(LAMP_OBJECT_2_SPECIAL, special_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_SPECIAL, special_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_SPECIAL, special_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_SPECIAL, special_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_SPECIAL, special_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_SPECIAL, special_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_SPECIAL, special_number == 8);

    TimeKeeper.callback_later(advance_special_target, 800);
}

void EightBankTargetsClass::start_spot_number_timeout()
{
    LOGM(M_STARTING_SPOT_NUMBER_TIMEOUT);

    // "" slingshots, spinner and pop bumpers control
    // the percentage of time dead bumper, two return lanes, two outlanes
    // and 3 bank lites are on.

    // start of extend the spot number timeout.
    if (!spot_number_enabled) {
        // turn on both inlanes
        spot_number_enabled = true;
        left_inlane_enabled = true;
        right_inlane_enabled = true;
    } else {
        // flip inlances, even if both are on.
        if (left_inlane_enabled) {
            left_inlane_enabled = false;
            right_inlane_enabled = true;
        } else {
            left_inlane_enabled = true;
            right_inlane_enabled = false;
        }
    }

    LampMatrix.set_lamp(LAMP_LEFT_INLANE, left_inlane_enabled);
    LampMatrix.set_lamp(LAMP_RIGHT_INLANE, right_inlane_enabled);
    LampMatrix.lamp_on(LAMP_TOP_POP_BUMPER);

    TimeKeeper.callback_later(spot_number_timed_out,
        GameSettings.spot_light_strategy ? 12000 : 5000);
}

void EightBankTargetsClass::spot_number_timed_out()
{
    // static function to turn off pop bumper etc.
    EightBankTargets.spot_number_enabled = false;
    EightBankTargets.left_inlane_enabled = false;
    EightBankTargets.right_inlane_enabled = false;

    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
}

void EightBankTargetsClass::advance_number_nine_target()
{
    // static function to advance nine target
    if (EightBankTargets.number_nine_number == 0)
        return; // maybe it was made, while we were waiting.

    EightBankTargets.number_nine_number = EightBankTargets.get_next_target_number(EightBankTargets.number_nine_number);

    LampMatrix.set_lamp(LAMP_OBJECT_1_DROP_TARGET, EightBankTargets.number_nine_number == 1);
    LampMatrix.set_lamp(LAMP_OBJECT_2_DROP_TARGET, EightBankTargets.number_nine_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_DROP_TARGET, EightBankTargets.number_nine_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_DROP_TARGET, EightBankTargets.number_nine_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_DROP_TARGET, EightBankTargets.number_nine_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_DROP_TARGET, EightBankTargets.number_nine_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_DROP_TARGET, EightBankTargets.number_nine_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_DROP_TARGET, EightBankTargets.number_nine_number == 8);
}

void EightBankTargetsClass::advance_wow_target()
{
    // static function to advance wow target
    if (EightBankTargets.wow_number == 0)
        return; // maybe it was made, while we were waiting.

    EightBankTargets.wow_number = EightBankTargets.get_next_target_number(EightBankTargets.wow_number);

    LampMatrix.set_lamp(LAMP_OBJECT_1_WOW, EightBankTargets.wow_number == 1);
    LampMatrix.set_lamp(LAMP_OBJECT_2_WOW, EightBankTargets.wow_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_WOW, EightBankTargets.wow_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_WOW, EightBankTargets.wow_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_WOW, EightBankTargets.wow_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_WOW, EightBankTargets.wow_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_WOW, EightBankTargets.wow_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_WOW, EightBankTargets.wow_number == 8);

    TimeKeeper.callback_later(advance_wow_target, 1000);
}

void EightBankTargetsClass::advance_special_target()
{
    // static function to advance the special target
    if (EightBankTargets.special_number == 0)
        return; // maybe it was made, while we were waiting.

    EightBankTargets.special_number = EightBankTargets.get_next_target_number(EightBankTargets.special_number);

    LampMatrix.set_lamp(LAMP_OBJECT_2_SPECIAL, EightBankTargets.special_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_SPECIAL, EightBankTargets.special_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_SPECIAL, EightBankTargets.special_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_SPECIAL, EightBankTargets.special_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_SPECIAL, EightBankTargets.special_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_SPECIAL, EightBankTargets.special_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_SPECIAL, EightBankTargets.special_number == 8);

    TimeKeeper.callback_later(advance_special_target, 800);
}

byte EightBankTargetsClass::get_next_target_number(byte current)
{
    switch (current)
    {
        case 2: return 4;
        case 4: return 6;
        case 6: return 8;
        case 8: return 7;
        case 7: return 5;
        case 5: return 3;
        case 3: return 1;
        case 1: return 2;
    }

    // all others...
    return 2; // the first one.
}

