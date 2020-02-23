
#include "types.h"
#include "bcdnum.h"


BcdNum::BcdNum()
{
    memset(bcd, 0, 4);
}

BcdNum::BcdNum(unsigned long value)
{
    from_decimal(value);
}


bool operator==(const BcdNum &a, const BcdNum &b)
{
    return a.bcd[0] == b.bcd[0]
        && a.bcd[1] == b.bcd[1]
        && a.bcd[2] == b.bcd[2]
        && a.bcd[3] == b.bcd[3];
}

bool operator>(const BcdNum &a, const BcdNum &b)
{
    if (a.bcd[0] > b.bcd[0]) return true;
    if (a.bcd[0] < b.bcd[0]) return false;
    
    if (a.bcd[1] > b.bcd[1]) return true;
    if (a.bcd[1] < b.bcd[1]) return false;
    
    if (a.bcd[2] > b.bcd[2]) return true;
    if (a.bcd[2] < b.bcd[2]) return false;
    
    return (a.bcd[3] > b.bcd[3]);
}    

void BcdNum::copy_from(BcdNum other)
{
    bcd[0] = other.bcd[0];
    bcd[1] = other.bcd[1];
    bcd[2] = other.bcd[2];
    bcd[3] = other.bcd[3];
}

void BcdNum::add(BcdNum other)
{
    math_operation(other.bcd, true);
}

void BcdNum::subtract(BcdNum other)
{
    math_operation(other.bcd, false);
}

void BcdNum::add_tens(dword tens_bcd)
{
    byte bcd[4];
    bcd[0] = 0x00;
    bcd[1] = (tens_bcd & 0xF000) >> 12;
    bcd[1] = (tens_bcd & 0x0FF0) >> 4;
    bcd[2] = (tens_bcd & 0x000F) << 4; 
    
    math_operation(bcd, true);
}

void BcdNum::add_hundreds(dword hundreds_bcd)
{
    byte bcd[4];
    bcd[0] = 0x00;
    bcd[1] = (hundreds_bcd & 0xFF00) >> 8;
    bcd[1] = (hundreds_bcd & 0x00FF);
    bcd[2] = 0x00; 
    
    math_operation(bcd, true);
}

void BcdNum::add_thousands(dword thousands_bcd)
{
    byte bcd[4];
    bcd[0] = (thousands_bcd & 0xF000) >> 12;
    bcd[1] = (thousands_bcd & 0x0FF0) >> 4;
    bcd[1] = (thousands_bcd & 0x000F) << 4; 
    bcd[2] = 0x00; 
    
    math_operation(bcd, true);
}

bool BcdNum::is_zero()
{
    return bcd[0] == 0
        && bcd[1] == 0
        && bcd[2] == 0
        && bcd[3] == 0; 
}

byte BcdNum::get_nibble(byte location)
{
    // location should be 0 for the rightmost, 7 for the left most
    // location / 2 (>>1) is the byte, location & 1 is the nibble
    // i.e. 0=units, 1=tens, 2=hundreds etc.
    
    if (location & 1)
        return ((bcd[(location >> 1)]) >> 4);
    else
        return (bcd[(location >> 1)]) & 0x0F;
}

void BcdNum::to_string(char *output)
{
    output[0] = '0' + (bcd[0] >> 4);
    output[1] = '0' + (bcd[0] & 0xF);
    output[2] = '0' + (bcd[1] >> 4);
    output[3] = '0' + (bcd[1] & 0xF);
    output[4] = '0' + (bcd[2] >> 4);
    output[5] = '0' + (bcd[2] & 0xF);
    output[6] = '0' + (bcd[3] >> 4);
    output[7] = '0' + (bcd[3] & 0xF);
    output[8] = 0;
}

void BcdNum::from_decimal(unsigned long value)
{
    byte left_nibble, right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    bcd[3] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    bcd[2] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    bcd[1] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    bcd[0] = (left_nibble << 4) | right_nibble;
}

unsigned long BcdNum::to_decimal()
{
    unsigned long value = 0;
    
    value += (bcd[0] >> 4)  * 10000000L;
    value += (bcd[0] & 0xF) *  1000000L;
    value += (bcd[1] >> 4)  *   100000L;
    value += (bcd[1] & 0xF) *    10000L;
    value += (bcd[2] >> 4)  *     1000L;
    value += (bcd[2] & 0xF) *      100L;
    value += (bcd[3] >> 4)  *       10L;
    value += (bcd[3] & 0xF) *        1L;
    
    return value;
}

void BcdNum::math_operation(byte amount_bcd[], bool do_addition)
{
    // expects amount bcd to mirror the bcd property (4 bytes, MSB=0, LSB=3)
    
    // using chars as value may go below zero, in subtraction.
    char carry;
    char amount_nibble;
    char current_value;
    char new_value;
    
    carry = 0;
    for (char i = 7; i >= 0; i--)
    {
        // i / 2 is the offset to use,
        // i % 2 is weather we'll take the left byte or the right one.
        byte offset = i >> 1;
        
        // bytes:   0=[ 0x00 ], 1=[ 0x00 ], 2=[ 0x00 ], 3=[ 0x00 ]
        // nibbles:       01          23          45          67
        
        // discover operands
        if (i & 1) {
            amount_nibble = (char)(amount_bcd[offset] & 0xF);
            current_value = (char)(bcd[offset] & 0xF);
        } else {
            amount_nibble = (char)(amount_bcd[offset] >> 4);
            current_value = (char)(bcd[offset] >> 4);
        }
        
        if (do_addition) {
            new_value = current_value + amount_nibble + carry;
            carry = 0;
            if (new_value >= 10) {
                new_value -= 10;
                carry = 1;
            }
        } else {
            new_value = current_value - amount_nibble - carry;
            carry = 0;
            if (new_value < 0) {
                new_value += 10;
                carry = 1;
            }
        }
        
        // put the new value back in the bcd
        if (i & 1)
            bcd[offset] = (byte)((bcd[offset] & 0xF0) | new_value);
        else
            bcd[offset] = (byte)((new_value << 4) | (bcd[offset] & 0xF));
    }
    
    // mark this if we want to be sophisticated enough.
    if (carry)
        overflow = 1;
}



