
extern CAnimator Animator;
extern CAudio Audio;
extern CGameSettings GameSettings;
extern ThreeBankTargetsClass ThreeBankTargets;


void BonusMultiplierClass::init(bool was_6_or_higher)
{
    value = was_6_or_higher ? 2 : 1; // 1x or 2x if previous was at least 6x.
    LampMatrix.lamp_on(LAMP_BONUS_MULTIPLIER_X1);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X2);
    LampMatrix.lamp_off(LAMP_BONUS_MULTIPLIER_X4);
}

void BonusMultiplierClass::increase_multiplier()
{
    value = (value + 1) & 0x7;
    Animator.start(ANIM_BONUS_MULTIPLIER, value);
    Audio.play(SOUND_FX_3);

    if ((value == 6 && GameSettings.three_bank_wow_turn_on == 1) ||
        (value == 7))
        ThreeBankTargets.start_wow();

    if (value >= 6)
        Gameplay.multipler_6x_achieved();
}

byte BonusMultiplierClass::get_multiplier()
{
    return value;
}
