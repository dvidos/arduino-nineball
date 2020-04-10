
void SpinnerClass::init()
{
    value = 0; // 100
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);
    LampMatrix.lamp_on(LAMP_SPINNER_TOP_VALUE);
}

void SpinnerClass::advance_to_top_value()
{
    LampMatrix.lamp_off(LAMP_SPINNER_TOP_VALUE);
    Gameplay.add_score_bcd(0x7000);
    value = 4; // 2500
    Animator.start(ANIM_SPINNER_INCREASE_VALUE, 0x0F);
    Audio.play(SOUND_FX_5);
}

void SpinnerClass::advance_value()
{
    if (value >= 4) // we don't reset like loop target
        return;

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    Animator.start(ANIM_SPINNER_INCREASE_VALUE, bitmap);

    Audio.play(SOUND_FX_5);
    LOGM(M_SPINNER_VALUE_IS_NOW_D, value);
}

void SpinnerClass::on_spinner_spun()
{
    dword score = get_spinner_score_bcd();
    Gameplay.add_score_bcd(score);

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    Animator.start(ANIM_SPINNER_COLLECT_VALUE, bitmap);

    Audio.play(SOUND_SPINNER);
    TimeKeeper.callback_later(reset_spinner_value, 2000);

    ThreeBankTargets.enable_or_flip_pink_target();
    LoopTarget.enable_or_flip_outlanes();
}

void SpinnerClass::reset_spinner_value()
{
    // remember this is a static function called by an interrupt
    Spinner.value = 0;
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_400);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_900);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_1600);
    LampMatrix.lamp_off(LAMP_SPINNER_VALUE_2500);

    LOGM(M_SPINNER_VALUE_RESET);
}

dword SpinnerClass::get_spinner_score_bcd()
{
    if      (value == 4) return 0x2500ul;
    else if (value == 3) return 0x1600ul;
    else if (value == 2) return 0x0900ul;
    else if (value == 1) return 0x0400ul;
    else                 return 0x0100ul;
}
