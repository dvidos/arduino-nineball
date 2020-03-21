


void ThreeBankTargetsClass::init()
{
    lit_pink_target = 0;
    lit_wow_target = 0;

    // set lamps, status etc.
    LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, (lit_pink_target == 1));
    LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);

    LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, (lit_pink_target == 2));
    LampMatrix.lamp_off(LAMP_RIGHT_BANK_RIGHT_TARGET);

    if (SwitchMatrix.is_switch_closed(SW_TOP_BANK_LEFT_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_CENTER_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_RIGHT_TARGET)) {
        // we hope we will not need retries
        Coils.fire_top_bank_reset();
    }

    if (SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_LEFT_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_CENTER_TARGET) ||
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_RIGHT_TARGET)) {
        // we hope we will not need retries
        Coils.fire_right_bank_reset();
    }
}

void ThreeBankTargetsClass::enable_or_flip_pink_target()
{
    // called at least when spinner is spinning
    lit_pink_target += 1;
    if (lit_pink_target >= 3)
        lit_pink_target = 1;

    LampMatrix.set_lamp(LAMP_TOP_BANK_CENTER_TARGET, lit_pink_target == 1);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_CENTER_TARGET, lit_pink_target == 2);

    TimeKeeper.callback_later(turn_off_pink_target, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void ThreeBankTargetsClass::turn_off_pink_target()
{
    // remember this is a static function
    ThreeBankTargets.lit_pink_target = 0;

    LampMatrix.lamp_off(LAMP_TOP_BANK_CENTER_TARGET);
    LampMatrix.lamp_off(LAMP_RIGHT_BANK_CENTER_TARGET);
}

void ThreeBankTargetsClass::start_wow()
{
    lit_wow_target = 1;

    LampMatrix.set_lamp(LAMP_TOP_BANK_LEFT_TARGET, lit_wow_target == 1);
    LampMatrix.set_lamp(LAMP_TOP_BANK_RIGHT_TARGET, lit_wow_target == 2);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_LEFT_TARGET, lit_wow_target == 3);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_RIGHT_TARGET, lit_wow_target == 4);

    TimeKeeper.callback_later(move_lit_wow_target, 1000);
}

void ThreeBankTargetsClass::move_lit_wow_target()
{
    // remember this is a static function
    if (ThreeBankTargets.lit_wow_target == 0) // maybe it was made while we were waiting.
        return;

    // moving to the next
    ThreeBankTargets.lit_wow_target += 1;
    if (ThreeBankTargets.lit_wow_target > 4)
        ThreeBankTargets.lit_wow_target = 1; // remember 0=off

    LampMatrix.set_lamp(LAMP_TOP_BANK_LEFT_TARGET, ThreeBankTargets.lit_wow_target == 1);
    LampMatrix.set_lamp(LAMP_TOP_BANK_RIGHT_TARGET, ThreeBankTargets.lit_wow_target == 2);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_LEFT_TARGET, ThreeBankTargets.lit_wow_target == 3);
    LampMatrix.set_lamp(LAMP_RIGHT_BANK_RIGHT_TARGET, ThreeBankTargets.lit_wow_target == 4);

    TimeKeeper.callback_later(move_lit_wow_target, 1000);
}

void ThreeBankTargetsClass::on_target_hit(byte switch_no)
{
    // see if a wow is awarded
    if ((lit_wow_target == 1 && switch_no == SW_TOP_BANK_LEFT_TARGET) ||
        (lit_wow_target == 2 && switch_no == SW_TOP_BANK_RIGHT_TARGET) ||
        (lit_wow_target == 3 && switch_no == SW_RIGHT_BANK_LEFT_TARGET) ||
        (lit_wow_target == 4 && switch_no == SW_RIGHT_BANK_RIGHT_TARGET)) {
        if (GameSettings.wow_award_type == 0) {
            Gameplay.add_score_bcd(0x7000);
        } else if (GameSettings.wow_award_type == 1) {
            BallKeeper.grant_shoot_again();
        }

        // we could have an animation of it blinking for a while.

        Audio.play(SOUND_FX_4);

        lit_wow_target = 0;
        LampMatrix.lamp_off(LAMP_TOP_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_TOP_BANK_RIGHT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_LEFT_TARGET);
        LampMatrix.lamp_off(LAMP_RIGHT_BANK_RIGHT_TARGET);
    }

    // "each target scores 1,000 or 7,000 when pink lite is lit"
    if ((lit_pink_target == 1 && switch_no == SW_TOP_BANK_CENTER_TARGET) ||
        (lit_pink_target == 2 && switch_no == SW_RIGHT_BANK_CENTER_TARGET)) {
        // we could have an animation of it blinking for a while.
        Gameplay.add_score_bcd(0x7000);
    } else {
        Gameplay.add_score_bcd(0x1000);
    }
    Audio.play(SOUND_FAST_PHASERS);


    if (GameSettings.spinner_value_advancement == 1) { // outer targets
        if (switch_no == SW_TOP_BANK_LEFT_TARGET ||
            switch_no == SW_TOP_BANK_RIGHT_TARGET ||
            switch_no == SW_RIGHT_BANK_LEFT_TARGET ||
            switch_no == SW_RIGHT_BANK_RIGHT_TARGET) {
            Spinner.advance_value();
        }
    } else { // center targets
        if (switch_no == SW_TOP_BANK_CENTER_TARGET ||
            switch_no == SW_RIGHT_BANK_CENTER_TARGET) {
            Spinner.advance_value();
        }
    }

    // see if we up bonus multiplier
    byte top_bank_down =
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_LEFT_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_CENTER_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_TOP_BANK_RIGHT_TARGET);

    byte right_bank_down =
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_LEFT_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_CENTER_TARGET) &&
        SwitchMatrix.is_switch_closed(SW_RIGHT_BANK_RIGHT_TARGET);

    if (GameSettings.multiplier_step_up == 0) { // both 3-banks
        if (top_bank_down && right_bank_down) {
            BonusMultiplier.increase_multiplier();
        }
    }
    else if (GameSettings.multiplier_step_up == 1) { // both 3-banks
        if (top_bank_down || right_bank_down) {
            BonusMultiplier.increase_multiplier();
        }
    }

    // "" 3 banks reset only after all targets in both 3 banks have been hit.
    // when "multiplier_step_up" is 1, only one 3 bank resets at random.
    // when "multiplier_step_up" is 0, both 3 banks reset.

    if (top_bank_down && right_bank_down) {
        if (GameSettings.multiplier_step_up == 0) { // both 3-banks
            // if there is loss of power to reset both banks together,
            // we can change our strategy
            Coils.fire_top_bank_reset();
            Coils.fire_right_bank_reset();
        } else { // only one bank resets at random
            if (random(0, 100) >= 50)
                Coils.fire_top_bank_reset();
            else
                Coils.fire_right_bank_reset();
        }
    }
}
