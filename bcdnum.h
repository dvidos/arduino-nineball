#ifndef BCDNUM_H
#define BCDNUM_H

#include "types.h"


class BcdNum
{
public:
    // e.g. [ 0x00, 0x12, 0x34, 0x56 ] represents 123,456
    byte bcd[4];
    byte overflow: 1;  // or underflow
    
    BcdNum();
    BcdNum(unsigned long value);
    
    void copy_from(BcdNum other);
    void add(BcdNum other);
    void subtract(BcdNum other);
        
    void add_tens(dword tens_bcd);
    void add_hundreds(dword hundreds_bcd);
    void add_thousands(dword thousands_bcd);
    
    bool is_zero();
    byte get_nibble(byte location);
    void from_decimal(unsigned long value);
    unsigned long to_decimal();
    void to_string(char *output);
    
    friend bool operator==(const BcdNum &a, const BcdNum &b);
    friend bool operator>(const BcdNum &a, const BcdNum &b);
    
protected:
    void math_operation(byte bcd_amount[], bool do_addition);
};





#endif