

void LoopTargetClass::init()
{
    value = 1; // 10K
    left_outlane = 0;
    right_outlane = 0;

    LampMatrix.lamp_on(LAMP_LOOP_VALUE_10K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_20K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_30K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_40K);
    LampMatrix.lamp_off(LAMP_LOOP_VALUE_173K);
    LampMatrix.lamp_off(LAMP_LEFT_OUTLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_OUTLANE);
}

void LoopTargetClass::advance_value()
{
    // increase value
    value += 1;
    if (value > 5)
        value = 1; // loop target increase wraps around

    byte bitmap = 0;
    if (value >= 1) bitmap |= 0x01;
    if (value >= 2) bitmap |= 0x02;
    if (value >= 3) bitmap |= 0x04;
    if (value >= 4) bitmap |= 0x08;
    if (value >= 5) bitmap |= 0x10;
    Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE, bitmap);

    Gameplay.add_score_bcd(0x3000);
    Audio.play(SOUND_FX_6);
}

void LoopTargetClass::collect_value()
{
    dword score = 0;
    if      (value == 5) score = 0x173000ul;
    else if (value == 4) score =  0x40000ul;
    else if (value == 3) score =  0x30000ul;
    else if (value == 2) score =  0x20000ul;
    else if (value == 1) score =  0x10000ul;
    Gameplay.add_score_bcd(score);

    Audio.play(SOUND_FX_5);

    value = 1;
    Animator.start(ANIM_TOP_LOOP_COLLECT_VALUE, 0x01);
}

void LoopTargetClass::enable_or_flip_outlanes()
{
    left_outlane ^= 1;
    right_outlane ^= 1;

    // if both on, select one
    if (left_outlane && right_outlane)
        right_outlane = false;

    LampMatrix.set_lamp(LAMP_LEFT_OUTLANE, left_outlane);
    LampMatrix.set_lamp(LAMP_RIGHT_OUTLANE, right_outlane);

    TimeKeeper.callback_later(turn_off_outlanes, GameSettings.spot_light_strategy ? 10000 : 5000);
}

void LoopTargetClass::on_passed_outlane(byte switch_no)
{
    Gameplay.add_score_bcd(0x3000);

    if ((switch_no == SW_LEFT_OUTLANE && left_outlane) ||
        (switch_no == SW_RIGHT_OUTLANE && right_outlane))
        collect_value();
}

void LoopTargetClass::turn_off_outlanes()
{
    // remember this is a static function called by an interrupt
    LoopTarget.left_outlane = 0;
    LoopTarget.right_outlane = 0;

    LampMatrix.lamp_off(LAMP_LEFT_OUTLANE);
    LampMatrix.lamp_off(LAMP_RIGHT_OUTLANE);
}
