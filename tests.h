#ifdef RUN_TESTS


#define ASSERT(condition)    if(!(condition)) { LOG("** Failed asserting that \"%s\"", #condition); }


bool test_bcd_math(BcdNum *n, unsigned long *expected_value, long amount);
void test_bcdnum_comparisons(dword decimal_1, dword decimal_2);
int freeMemory();


void run_tests()
{
    BcdNum n;
    unsigned long expected_value = 0;
    
    LOG_INIT();
    LOG("free memory (stack - heap) %d (total 8 KB or 8192 bytes), %f %", freeMemory(), (freeMemory() * 100.0)/8192);
    LOG("size of settings is %d bytes (total 4 KB or 4096 bytes)", sizeof(GameSettings));
    LOG("size of audio instance is %d bytes", sizeof(Audio));    
    LOG("size of lamp matrix instance is %d bytes", sizeof(LampMatrix));    
    LOG("size of score display instance is %d bytes", sizeof(ScoreDisplay));    
    LOG("size of switch matrix instance is %d bytes", sizeof(SwitchMatrix));    
    LOG("size of gameplay instance is %d bytes", sizeof(Gameplay));    
    LOG("size of attract instance is %d bytes", sizeof(Attract));    
    LOG("size of animator instance is %d bytes", sizeof(Animator));    
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
        sizeof(SwitchMatrix);
    LOG("total size of resident global variables is %d bytes", global_variables_total_memory);
    
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



#endif // RUN_TESTS