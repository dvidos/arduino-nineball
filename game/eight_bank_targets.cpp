

void EightBankTargetsClass::init()
{
    object_made = 0;
    wow_number = 0;
    special_number = 0;
    spot_number_enabled = 0;
    special_made = 0;
    super_bonus_made = 0;
    bringing_targets_up_tries = 0;

    LampMatrix.lamp_off(LAMP_BONUS_MADE_1);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_2);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_3);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_4);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_5);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_6);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_7);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_8);
    LampMatrix.lamp_off(LAMP_BONUS_MADE_9);

    LampMatrix.lamp_on(LAMP_OBJECT_1_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_2_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_3_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_4_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_5_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_6_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_7_DROP_TARGET);
    LampMatrix.lamp_on(LAMP_OBJECT_8_DROP_TARGET);

    LampMatrix.lamp_off(LAMP_OBJECT_1_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_2_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_3_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_4_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_5_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_6_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_7_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_8_WOW);

    LampMatrix.lamp_off(LAMP_OBJECT_2_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_3_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_4_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_5_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_6_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_7_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_8_SPECIAL);

    LampMatrix.lamp_off(LAMP_OBJECT_9);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
    LampMatrix.lamp_off(LAMP_SUPER_BONUS_77K);
    LampMatrix.lamp_off(LAMP_ALL_TARGETS_DOWN_SPECIAL);

    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);
}

void EightBankTargetsClass::on_target_hit(byte switch_no)
{
    // let's deduce the number hit, so we can programmatically
    // calculate things without a ton of conditions
    byte target_no;
    byte target_switches[] = {
        SW_LEFT_BANK_TARGET_1,
        SW_LEFT_BANK_TARGET_2,
        SW_LEFT_BANK_TARGET_3,
        SW_LEFT_BANK_TARGET_4,
        SW_LEFT_BANK_TARGET_5,
        SW_LEFT_BANK_TARGET_6,
        SW_LEFT_BANK_TARGET_7,
        SW_LEFT_BANK_TARGET_8,
    };
    for (target_no = 0; target_no < 8; target_no++) {
        if (switch_no == target_switches[target_no])
            break;
    }
    target_no++; // convert from 0..7 into 1..8, if not found it'll be 9.

    if (target_no <= 8)
    {
        if (target_no == special_number) {
            award_special();
        } else if (target_no == wow_number) {
            award_wow();
        } else if (target_no == number_nine_number) {
            award_number_nine();
        } else if (target_no == object_made + 1) {
            award_current_number();
        } else {
            // he just hit a drop target.
            Audio.play(SOUND_FX_6);
            Gameplay.add_score_bcd(0x1000);
        }

        if (all_drop_targets_down())
            bring_drop_targets_up();
    }
    else
    {
        // handle other switches, bumper, inlanes...
        // maybe make current number
    }

    // if object made move to the next.
    // if 8/9 was made (depending on the settings) start WOWs and Specials.
    // if 5 was made, start the ball keeping feature
    // etc.

    if (object_made >= 5)
        BallKeeper.enable_ball_capturing();

    // see if we need to reset the switches
    // if No 8 is dropped, we definitely need to .
    // if all are down we need as well.
    // when else? when the next number is not down?
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

    if (EightBankTargets.bringing_targets_up_tries == 3)
        return; // give up

    EightBankTargets.bringing_targets_up_tries += 1;
    Coils.fire_left_bank_reset();
    TimeKeeper.callback_later(verify_drop_targets_are_up, 600);
}

void EightBankTargetsClass::award_current_number()
{
    // award No target
    Gameplay.add_score_bcd(0x1000);
    object_made++;
    if (object_made == 8) {
        // kick off No 9,
        // possibly kick off wow and special
        // possibly turn on the 77K bonus
        // add to the BONUS_MADE diamond lights and have them blink?

        start_number_nine_sequence();

        if (GameSettings.eight_bank_wow_turn_on == 1) // start on 8
            start_wow_sequence();
    }
}

void EightBankTargetsClass::award_number_nine()
{
    // award No 9
    // grant No 9.
    Audio.play(SOUND_FX_6);
    object_made = 9;
    // possibly kick off wow and special
    // possibly turn on the 77K bonus

    if (GameSettings.eight_bank_wow_turn_on == 0) // start on 9
        start_wow_sequence();
}

void EightBankTargetsClass::award_wow()
{
    // award wow
    Audio.play(SOUND_FX_2);

    if (GameSettings.wow_award_type == 0) {
        Gameplay.add_score_bcd(0x70000);
    } else if (GameSettings.wow_award_type == 1) {
        BallKeeper.grant_shoot_again();
    }

    // turn off lamps (maybe blink the one for a while?)
    LampMatrix.lamp_off(LAMP_OBJECT_1_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_2_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_3_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_4_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_5_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_6_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_7_WOW);
    LampMatrix.lamp_off(LAMP_OBJECT_8_WOW);

    start_special_sequence();
}

void EightBankTargetsClass::award_special()
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
    LampMatrix.lamp_on(LAMP_ALL_TARGETS_DOWN_SPECIAL);

    LampMatrix.lamp_off(LAMP_OBJECT_2_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_3_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_4_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_5_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_6_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_7_SPECIAL);
    LampMatrix.lamp_off(LAMP_OBJECT_8_SPECIAL);

    // we need to see if special will launch again.
    if (GameSettings.unlimited_specials) {
        start_special_sequence();
    }
}

void EightBankTargetsClass::start_number_nine_sequence()
{
    number_nine_number = 1;

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
    wow_number = 1;

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
    special_number = 2;

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
    // "" slingshots, spinner and pop bumpers control
    // the percentage of time dead bumper, two return lanes, two outlanes
    // and 3 bank lites are on.

    // start of extend the spot number timeout.
    spot_number_enabled = 1;

    LampMatrix.lamp_on(LAMP_LEFT_INLANE);
    LampMatrix.lamp_on(LAMP_RIGHT_INLANE);
    LampMatrix.lamp_on(LAMP_TOP_POP_BUMPER);

    TimeKeeper.callback_later(spot_number_timed_out, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void EightBankTargetsClass::spot_number_timed_out()
{
    // static function to turn off pop bumper etc.
    EightBankTargets.spot_number_enabled = false;

    LampMatrix.lamp_off(LAMP_LEFT_INLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_INLANE);
    LampMatrix.lamp_off(LAMP_TOP_POP_BUMPER);
}

void EightBankTargetsClass::advance_number_nine_target()
{
    // static function to advance nine target
    if (EightBankTargets.number_nine_number == 0)
        return; // maybe it was made, while we were waiting.

    EightBankTargets.number_nine_number += 1;
    if (EightBankTargets.number_nine_number > 8)
        EightBankTargets.number_nine_number = 1;

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

    EightBankTargets.wow_number += 1;
    if (EightBankTargets.wow_number > 8)
        EightBankTargets.wow_number = 1;

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

    EightBankTargets.special_number += 1;
    if (EightBankTargets.special_number > 8)
        EightBankTargets.special_number = 2;

    LampMatrix.set_lamp(LAMP_OBJECT_2_SPECIAL, EightBankTargets.special_number == 2);
    LampMatrix.set_lamp(LAMP_OBJECT_3_SPECIAL, EightBankTargets.special_number == 3);
    LampMatrix.set_lamp(LAMP_OBJECT_4_SPECIAL, EightBankTargets.special_number == 4);
    LampMatrix.set_lamp(LAMP_OBJECT_5_SPECIAL, EightBankTargets.special_number == 5);
    LampMatrix.set_lamp(LAMP_OBJECT_6_SPECIAL, EightBankTargets.special_number == 6);
    LampMatrix.set_lamp(LAMP_OBJECT_7_SPECIAL, EightBankTargets.special_number == 7);
    LampMatrix.set_lamp(LAMP_OBJECT_8_SPECIAL, EightBankTargets.special_number == 8);

    TimeKeeper.callback_later(advance_special_target, 800);
}



