#include "audio.h"

#define ASSERT(x)    \
            Serial.print("Testing "); \
            Serial.print(#x); \
            if (x) { \
                Serial.println(" ...OK"); \
            } else { \
                Serial.println(" ... ERROR!!!"); \
            }

bool test_bcd_number(BcdNum *n, unsigned long *expected_value, dword bcd_tens, bool addition);
void test_bcdnum_comparisons(dword decimal_1, dword decimal_2);

void run_tests()
{
    Serial.begin(9600);
    Serial.println("Running tests");
    
    char buffer[64];
    // sprintf does support "unsligned long", using %lu. tested with value 4294967295L 
    
    sprintf(buffer, "size of settings instance is: %d", sizeof(Settings));    
    Serial.println(buffer);
    
    sprintf(buffer, "size of audio instance is: %d", sizeof(Audio));    
    Serial.println(buffer);
      
    sprintf(buffer, "size of lamp matrix instance is: %d", sizeof(LampMatrix));    
    Serial.println(buffer);
      
    sprintf(buffer, "size of switch matrix instance is: %d", sizeof(SwitchMatrix));    
    Serial.println(buffer);
      
    Gameplay gp;
    sprintf(buffer, "size of gameplay instance is: %d", sizeof(gp));    
    Serial.println(buffer);

    BcdNum n = BcdNum();
    sprintf(buffer, "size of bcdnum instance is: %d", sizeof(n));    
    Serial.println(buffer);

    ASSERT(n.to_decimal() == 0);
        
    unsigned long expected_value = 0;
  
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
    char buffer[64];
    
    if (amount < 0) {
        addition = false;
        amount *= -1;
        
        sprintf(buffer, "Subtracting %ld...", amount);
    } else {
        sprintf(buffer, "Adding %ld...", amount);
    }
    Serial.println(buffer);
    
    
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
        sprintf(buffer, " OK, gotten %10lu", n->to_decimal());
    } else {
        sprintf(buffer, " ERROR! expected %lu, got %lu", *expected_value, n->to_decimal());
    }
    Serial.println(buffer);
    
    return success;    
}

void test_bcdnum_comparisons(dword decimal_1, dword decimal_2)
{
    BcdNum n1 = BcdNum(decimal_1);
    BcdNum n2 = BcdNum(decimal_2);
    
    if (decimal_1 == decimal_2) {
        ASSERT(n1 == n2);
    } else if (decimal_1 > decimal_2) {
        ASSERT(n1 > n2);
    } else if (decimal_2 > decimal_1) {
        ASSERT(n2 > n1);
    }
}
