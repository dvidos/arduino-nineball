#ifdef RUN_SOFTWARE_TESTS




bool test_bcd_math(BcdNum *n, unsigned long *expected_value, long amount);
void test_bcdnum_comparisons(dword decimal_1, dword decimal_2);
void test_timeouts();
void test_callbacks();
void test_bcd();
void test_score_display();
int freeMemory();


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

    LOG("free memory (stack - heap) is %d bytes, (total 8 KB or 8192 bytes), %d%%", freeMemory(), (freeMemory() * 100)/8192);
    LOG("size of settings is %d bytes (total 4 KB or 4096 bytes)", sizeof(GameSettings));
    LOG("size of audio instance is %d bytes", sizeof(Audio));
    LOG("size of lamp matrix instance is %d bytes", sizeof(LampMatrix));
    LOG("size of score display instance is %d bytes", sizeof(ScoreDisplay));
    LOG("size of switch matrix instance is %d bytes", sizeof(SwitchMatrix));
    LOG("size of gameplay instance is %d bytes", sizeof(Gameplay));
    LOG("size of attract instance is %d bytes", sizeof(Attract));
    LOG("size of animator instance is %d bytes", sizeof(Animator));
    LOG("size of time keeper instance is %d bytes", sizeof(TimeKeeper));
    LOG("size of coils instance is %d bytes", sizeof(Coils));
    LOG("size of bcdnum instance is %d bytes", sizeof(n));

    int global_variables_total_memory =
        sizeof(Animator) +
        sizeof(Coils) +
        sizeof(Audio) +
        sizeof(Coils) +
        sizeof(Gameplay) +
        sizeof(GameSettings) +
        sizeof(LampMatrix) +
        sizeof(ScoreDisplay) +
        sizeof(TimeKeeper) +
        sizeof(SwitchMatrix);
    LOG("total size of resident global variables is %d bytes", global_variables_total_memory);

    test_bcd();
    test_score_display();
    test_timeouts();
    test_callbacks();
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
    LOG("n is now %lu", n.to_decimal());

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

    LOG("Adding 3 ones...");
    n.add_bcd(0x0003);
    ASSERT(n.to_decimal() == 3);

    LOG("Adding 30 ones...");
    n.add_bcd(0x0030);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 33);

    LOG("Adding 300 ones...");
    n.add_bcd(0x0300);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 333);

    LOG("Adding 3000 ones...");
    n.add_bcd(0x3000);
    LOG("n is now %lu", n.to_decimal());
    ASSERT(n.to_decimal() == 3333);

    n.from_decimal(0);

    LOG("Adding 2 tens...");
    n.add_tens(0x0002);
    ASSERT(n.to_decimal() == 20);

    LOG("Adding 8 tens...");
    n.add_tens(0x0008);
    ASSERT(n.to_decimal() == 100);

    LOG("Adding 1 hundred...");
    n.add_hundreds(0x0001);
    ASSERT(n.to_decimal() == 200);

    LOG("Adding 8 hundreds...");
    n.add_hundreds(0x0008);
    ASSERT(n.to_decimal() == 1000);

    LOG("Adding 2 thousands...");
    n.add_thousands(0x0002);
    ASSERT(n.to_decimal() == 3000);

    LOG("Adding 7 thousands...");
    n.add_thousands(0x0007);
    ASSERT(n.to_decimal() == 10000);

    LOG("Adding 302 thousands...");
    n.add_thousands(0x0302);
    ASSERT(n.to_decimal() == 312000);

    LOG("Adding 2 tens...");
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
        LOG("Subtracting %ld...", amount);
    } else {
        LOG("Adding %ld...", amount);
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


// see https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
// and https://playground.arduino.cc/Code/AvailableMemory/

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


void test_timeouts()
{
    byte timeout_no;
    byte got_it;

    LOG("Testing timeouts");

    TimeKeeper.start_timeout(TIMEOUT_DEMO, 100);
    // retrieve fast, before the timeout is expired
    got_it = TimeKeeper.get_next_expiration_event(&timeout_no);
    ASSERT(got_it == false);

    // wait a bit, try again.
    delay(150);
    got_it = TimeKeeper.get_next_expiration_event(&timeout_no);
    ASSERT(got_it == true);
    ASSERT(timeout_no == TIMEOUT_DEMO);

    // make sure nothing is in the queue afterwards
    delay(150);
    got_it = TimeKeeper.get_next_expiration_event(&timeout_no);
    ASSERT(got_it == false);
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
    LOG("testing callbacks");
    callback_integer = 0;
    TimeKeeper.callback_later(increment_callback_integer, 50);
    LOG("callback_integer is %d", callback_integer);
    ASSERT(callback_integer == 0);
    delay(100);
    LOG("callback_integer is %d", callback_integer);
    ASSERT(callback_integer == 1);
}

#endif // RUN_TESTS
