#ifdef RUN_SOFTWARE_TESTS



bool test_bcd_math(BcdNum *n, unsigned long *expected_value, long amount);
void test_bcdnum_comparisons(dword decimal_1, dword decimal_2);
void test_callbacks();
void test_bcd();
void test_score_display();
void test_nesting_1();
void test_nesting_2();
void test_nesting_3();


extern CGameSettings GameSettings;
extern CLampMatrix LampMatrix;
extern CAudio Audio;
extern CSwitchMatrix SwitchMatrix;
extern CGameplay Gameplay;
extern CCoils Coils;
extern CAnimator Animator;
extern CTimeKeeper TimeKeeper;
extern CAttract Attract;



void run_tests()
{
    BcdNum n;
    LOG("Structures sizes (bytes)");
    LOG("Animator: %d", sizeof(Audio));
    LOG("LampMatrix: %d", sizeof(LampMatrix));
    LOG("SwitchMatrix: %d", sizeof(SwitchMatrix));
    LOG("ScoreDisplay: %d", sizeof(ScoreDisplay));
    LOG("Coils: %d", sizeof(Coils));
    LOG("Animator: %d", sizeof(Animator));
    LOG("TimeKeeper: %d", sizeof(TimeKeeper));
    LOG("Attract: %d", sizeof(Attract));
    LOG("GameSettings: %d", sizeof(GameSettings));
    LOG("Gameplay: %d", sizeof(Gameplay));
    LOG("BallKeeper: %d", sizeof(BallKeeper));
    LOG("BonusMultiplier: %d", sizeof(BonusMultiplier));
    LOG("EightBankTargets: %d", sizeof(EightBankTargets));
    LOG("LoopTarget: %d", sizeof(LoopTarget));
    LOG("Spinner: %d", sizeof(Spinner));
    LOG("ThreeBankTargets: %d", sizeof(ThreeBankTargets));

    int global_variables_total_memory =
        sizeof(Audio) +
        sizeof(LampMatrix) +
        sizeof(SwitchMatrix) +
        sizeof(ScoreDisplay) +
        sizeof(Coils) +
        sizeof(Animator) +
        sizeof(TimeKeeper) +
        sizeof(Attract) +
        sizeof(GameSettings) +
        sizeof(Gameplay) +
        sizeof(BallKeeper) +
        sizeof(BonusMultiplier) +
        sizeof(EightBankTargets) +
        sizeof(LoopTarget) +
        sizeof(Spinner) +
        sizeof(ThreeBankTargets);
    LOG("All global variables: %d", global_variables_total_memory);

    LOG("Free memory reports %d", freeMemory());

    LOG("Depth %d, Stack used %d", 1, stack_size());
    test_nesting_1();

    LOG("Testing PROGMEM");
    test_print(2);
    LOG("^ should show String 2");
    test_print(4);
    LOG("^ should show a very long String 4");

    test_bcd();
    test_score_display();
    test_callbacks();
}

void test_nesting_1() {
    char something[16];
    memset(something, 0, sizeof(something));
    something[2] = something[1];

    LOG("Depth %d, Stack used %d", 2, stack_size());
    test_nesting_2();
}

void test_nesting_2() {
    char something[128];
    memset(something, 0, sizeof(something));
    something[2] = something[1];

    LOG("Depth %d, Stack used %d", 3, stack_size());
    test_nesting_3();
}

void test_nesting_3() {
    char something[256];
    memset(something, 0, sizeof(something));
    something[2] = something[1];

    LOG("Depth %d, Stack used %d", 4, stack_size());
}



void test_bcd()
{
    BcdNum n;
    unsigned long expected_value = 0;

    ASSERT(n.to_decimal() == 0);

    test_bcd_math(&n, &expected_value, 0);
    test_bcd_math(&n, &expected_value, 6);
    test_bcd_math(&n, &expected_value, 6);
    test_bcd_math(&n, &expected_value, 63);
    test_bcd_math(&n, &expected_value, 63);
    test_bcd_math(&n, &expected_value, 673);
    test_bcd_math(&n, &expected_value, 673);
    test_bcd_math(&n, &expected_value, 6730);
    test_bcd_math(&n, &expected_value, 6000);
    test_bcd_math(&n, &expected_value, 9999);
    test_bcd_math(&n, &expected_value, 5555);
    test_bcd_math(&n, &expected_value, 4444);
    test_bcd_math(&n, &expected_value, 3333);
    test_bcd_math(&n, &expected_value, 9999);
    test_bcd_math(&n, &expected_value, 123456);
    test_bcd_math(&n, &expected_value, 654321);
    test_bcd_math(&n, &expected_value, 99174678);

    test_bcd_math(&n, &expected_value, -6);
    test_bcd_math(&n, &expected_value, -6);
    test_bcd_math(&n, &expected_value, -63);
    test_bcd_math(&n, &expected_value, -63);
    test_bcd_math(&n, &expected_value, -673);
    test_bcd_math(&n, &expected_value, -673);
    test_bcd_math(&n, &expected_value, -6730);
    test_bcd_math(&n, &expected_value, -6000);
    test_bcd_math(&n, &expected_value, -9999);
    test_bcd_math(&n, &expected_value, -5555);
    test_bcd_math(&n, &expected_value, -4444);
    test_bcd_math(&n, &expected_value, -3333);
    test_bcd_math(&n, &expected_value, -9999);
    test_bcd_math(&n, &expected_value, -123456);
    test_bcd_math(&n, &expected_value, -654321);
    test_bcd_math(&n, &expected_value, -99174678);

    ASSERT(n.is_zero());

    n.set_nibble(0, 0);
    n.set_nibble(1, 1);
    n.set_nibble(2, 2);
    n.set_nibble(3, 3);
    n.set_nibble(4, 4);
    n.set_nibble(5, 5);
    n.set_nibble(6, 6);
    n.set_nibble(7, 7);

    ASSERT(n.get_nibble(0) == 0);
    ASSERT(n.get_nibble(1) == 1);
    ASSERT(n.get_nibble(2) == 2);
    ASSERT(n.get_nibble(3) == 3);
    ASSERT(n.get_nibble(4) == 4);
    ASSERT(n.get_nibble(5) == 5);
    ASSERT(n.get_nibble(6) == 6);
    ASSERT(n.get_nibble(7) == 7);
    ASSERT(n.to_decimal() == 1234567);

    n.zero();
    ASSERT(n.is_zero());

    n.add_bcd(0x0003);
    ASSERT(n.to_decimal() == 3);

    n.add_bcd(0x0030);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 33);

    n.add_bcd(0x0300);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 333);

    n.add_bcd(0x3000);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 3333);

    n.from_decimal(0);

    n.add_tens(0x0002);
    ASSERT(n.to_decimal() == 20);

    n.add_tens(0x0008);
    ASSERT(n.to_decimal() == 100);

    n.add_hundreds(0x0001);
    ASSERT(n.to_decimal() == 200);

    n.add_hundreds(0x0008);
    ASSERT(n.to_decimal() == 1000);

    n.add_thousands(0x0002);
    ASSERT(n.to_decimal() == 3000);

    n.add_thousands(0x0007);
    ASSERT(n.to_decimal() == 10000);

    n.add_thousands(0x0302);
    ASSERT(n.to_decimal() == 312000);

    test_bcdnum_comparisons(0, 0);
    test_bcdnum_comparisons(1, 1);
    test_bcdnum_comparisons(0, 1);
    test_bcdnum_comparisons(1, 0);
    test_bcdnum_comparisons(100, 101);
    test_bcdnum_comparisons(100, 201);
    test_bcdnum_comparisons(1000, 1001);
    test_bcdnum_comparisons(2000, 1999);
    test_bcdnum_comparisons(3333, 3399);
}

bool test_bcd_math(BcdNum *n, unsigned long *expected_value, long amount)
{
    bool addition = true;

    if (amount < 0) {
        addition = false;
        amount *= -1;
    }

    BcdNum other = BcdNum(amount);

    if (addition) {
        n->add(other);
        (*expected_value) += amount;
    } else {
        n->subtract(other);
        (*expected_value) -= amount;
    }

    bool success = (n->to_decimal() == *expected_value);
    if (success) {
        // LOG("OK, gotten %lu", n->to_decimal());
    } else {
        LOG("ERROR! expected %lu, got %lu", *expected_value, n->to_decimal());
    }

    return success;
}

void test_bcdnum_comparisons(dword decimal_1, dword decimal_2)
{
    BcdNum n1 = BcdNum(decimal_1);
    BcdNum n2 = BcdNum(decimal_2);

    LOG("comparing %u to %u...", decimal_1, decimal_2);
    if (decimal_1 == decimal_2) {
        ASSERT(n1 == n2);
    } else if (decimal_1 > decimal_2) {
        ASSERT(n1 > n2);
    } else if (decimal_2 > decimal_1) {
        ASSERT(n2 > n1);
    }
}

void test_score_display()
{
    ScoreDisplay.hide_all();
    ScoreDisplay.show_digit(0, 0);
    ScoreDisplay.show_digit(1, 1);
    ScoreDisplay.show_digit(2, 2);
    ScoreDisplay.show_digit(3, 3);
    ScoreDisplay.show_digit(4, 4);
    ScoreDisplay.show_digit(5, 5);
    ScoreDisplay.show_digit(6, 6);
    ScoreDisplay.show_digit(7, 7);
    ScoreDisplay.show_digit(8, 8);
    ScoreDisplay.show_digit(9, 9);
    ScoreDisplay.show_digit(10, 0);
    ScoreDisplay.show_digit(11, 1);
    ScoreDisplay.show_digit(12, 2);
    ScoreDisplay.show_digit(13, 3);
    ScoreDisplay.show_digit(14, 4);
    ScoreDisplay.show_digit(15, 5);

    ScoreDisplay.hide_all();

    BcdNum n;
    n.from_decimal(12);
    ScoreDisplay.show_bcd_num(0, n);
    n.from_decimal(98765);
    ScoreDisplay.show_bcd_num(0, n);
    n.from_decimal(54321);
    ScoreDisplay.show_bcd_num(1, n);
    n.from_decimal(4999888);
    ScoreDisplay.show_bcd_num(1, n);
}

int callback_integer = 0;

void increment_callback_integer()
{
    callback_integer += 1;
}

void test_callbacks()
{
    callback_integer = 0;
    TimeKeeper.callback_later(increment_callback_integer, 50);
    ASSERT(callback_integer == 0);
    delay(100);
    ASSERT(callback_integer == 1);
}

#endif // RUN_TESTS
