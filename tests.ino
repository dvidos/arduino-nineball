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

void run_tests()
{
    Serial.begin(9600);
    Serial.println("Running tests");
    
    char buffer[64];
    // sprintf does support "unsligned long", using %lu. tested with value 4294967295L 
    
    sprintf(buffer, "size of audio instance is: %d", sizeof(Audio));    
    Serial.println(buffer);
      
    sprintf(buffer, "size of lamp matrix instance is: %d", sizeof(LampMatrix));    
    Serial.println(buffer);
      
    sprintf(buffer, "size of switch matrix instance is: %d", sizeof(SwitchMatrix));    
    Serial.println(buffer);
      
    Gameplay gp;
    sprintf(buffer, "size of gameplay instance is: %d", sizeof(gp));    
    Serial.println(buffer);

    BcdNum n;
    sprintf(buffer, "size of bcdnum instance is: %d", sizeof(n));    
    Serial.println(buffer);
    
    unsigned long expected_value = 0;
  
    test_bcd_number(&n, &expected_value, 0x0001, true);
    test_bcd_number(&n, &expected_value, 0x0010, true);
    test_bcd_number(&n, &expected_value, 0x0100, true);
    test_bcd_number(&n, &expected_value, 0x1000, true);
    test_bcd_number(&n, &expected_value, 0x2222, true);
    test_bcd_number(&n, &expected_value, 0x6666, true);
    test_bcd_number(&n, &expected_value, 0x0001, true);
    test_bcd_number(&n, &expected_value, 0x1234, true);
    test_bcd_number(&n, &expected_value, 0x4567, true);
    test_bcd_number(&n, &expected_value, 0x6789, true);
    //for (int i = 0; i < 1000; i++) 
    //    test_bcd_number(&n, &expected_value, 0x9999, true);
    
    n.from_decimal(12345678);
    ASSERT(n.to_decimal() == 12345678L);
    expected_value = n.to_decimal();
    
    test_bcd_number(&n, &expected_value, 0x0001, false);
    test_bcd_number(&n, &expected_value, 0x0010, false);
    test_bcd_number(&n, &expected_value, 0x0100, false);
    test_bcd_number(&n, &expected_value, 0x1000, false);
    test_bcd_number(&n, &expected_value, 0x2222, false);
    test_bcd_number(&n, &expected_value, 0x1111, false);
    test_bcd_number(&n, &expected_value, 0x3333, false);
    test_bcd_number(&n, &expected_value, 0x7777, false);
    test_bcd_number(&n, &expected_value, 0x9999, false);
    test_bcd_number(&n, &expected_value, 0x8888, false);
    test_bcd_number(&n, &expected_value, 0x9999, false);
    test_bcd_number(&n, &expected_value, 0x8888, false);
    test_bcd_number(&n, &expected_value, 0x9999, false);
}


bool test_bcd_number(BcdNum *n, unsigned long *expected_value, dword bcd_tens, bool addition)
{
    char buffer[96];
    sprintf(buffer, "%s %4lX0... ", addition ? "Adding" : "Subtracting", bcd_tens);
    Serial.print(buffer);
    
    unsigned long adder_decimal = 
        ((bcd_tens >> 12) & 0x0F) * 10000L +
        ((bcd_tens >>  8) & 0x0F) *  1000L +
        ((bcd_tens >>  4) & 0x0F) *   100L +
        ((bcd_tens >>  0) & 0x0F) *    10L;  // note, this is tens.

    if (addition)
    {
        n->add_bcd_tens(bcd_tens);
        (*expected_value) += adder_decimal;
    }
    else
    {
        n->subtract_bcd_tens(bcd_tens);
        (*expected_value) -= adder_decimal;
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

