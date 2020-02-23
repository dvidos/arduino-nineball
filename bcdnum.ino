
#include "types.h"


class BcdNum
{
public:
    // e.g. [ 0x00, 0x12, 0x34, 0x56 ] represents 123,456
    byte buffer[4];
    
    BcdNum();
    BcdNum(unsigned long value);
    
    byte get_nibble(byte location);
    void from_decimal(unsigned long value);
    unsigned long to_decimal();
    void to_string(char *output);
    void add_bcd_tens(dword bcd_amount_in_tens);
    void subtract_bcd_tens(dword bcd_amount_in_tens);
};

BcdNum::BcdNum()
{
    memset(buffer, 0, 4);
}

BcdNum::BcdNum(unsigned long value)
{
    from_decimal(value);
}

byte BcdNum::get_nibble(byte location)
{
    // location should be 0 for the rightmost, 7 for the left most
    // location / 2 (>>1) is the byte, location & 1 is the nibble
    // i.e. 0=units, 1=tens, 2=hundreds etc.
    
    if (location & 1)
        return ((buffer[(location >> 1)]) >> 4);
    else
        return (buffer[(location >> 1)]) & 0x0F;
}

void BcdNum::to_string(char *output)
{
    output[0] = '0' + (buffer[0] >> 4);
    output[1] = '0' + (buffer[0] & 0xF);
    output[2] = '0' + (buffer[1] >> 4);
    output[3] = '0' + (buffer[1] & 0xF);
    output[4] = '0' + (buffer[2] >> 4);
    output[5] = '0' + (buffer[2] & 0xF);
    output[6] = '0' + (buffer[3] >> 4);
    output[7] = '0' + (buffer[3] & 0xF);
    output[8] = 0;
}

void BcdNum::from_decimal(unsigned long value)
{
    byte left_nibble, right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    buffer[3] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    buffer[2] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    buffer[1] = (left_nibble << 4) | right_nibble;

    right_nibble = value % 10;
    value /= 10;
    left_nibble = value % 10;
    value /= 10;
    
    buffer[0] = (left_nibble << 4) | right_nibble;
}

unsigned long BcdNum::to_decimal()
{
    unsigned long value = 0;
    
    value += (buffer[0] >> 4)  * 10000000L;
    value += (buffer[0] & 0xF) *  1000000L;
    value += (buffer[1] >> 4)  *   100000L;
    value += (buffer[1] & 0xF) *    10000L;
    value += (buffer[2] >> 4)  *     1000L;
    value += (buffer[2] & 0xF) *      100L;
    value += (buffer[3] >> 4)  *       10L;
    value += (buffer[3] & 0xF) *        1L;
    
    return value;
}

void BcdNum::add_bcd_tens(dword bcd_amount_in_tens)
{
    // essentially, we add the two nibbles and if we have a carry, we move on to the next.
    // score buffer: 0x00, 0x00, 0x00, 0x00
    // bcd_amount_in_tens: 0x1234
    // result buffer: 0x00, 0x01, 0x23, 0x40.
    
    char to_add;
    char nibble;
    char carry;
    
    // buffer[3], left nibble
    to_add = (bcd_amount_in_tens & 0x0F);
    nibble = (buffer[3] >> 4) + to_add;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[3] = (nibble << 4) | (buffer[3] & 0x0F); 
    
    // next digit, buffer[2], right nibble
    to_add = (bcd_amount_in_tens >> 4) & 0x0F;
    nibble = (buffer[2] & 0xF) + to_add + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[2] = (buffer[2] & 0xF0) | nibble;
    
    // next digit, buffer[2], left nibble
    to_add = (bcd_amount_in_tens >> 8) & 0x0F;
    nibble = (buffer[2] >> 4) + to_add + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[2] = (nibble << 4) | (buffer[2] & 0x0F);
    
    // next digit, buffer[1], right nibble
    to_add = (bcd_amount_in_tens >> 12) & 0x0F;
    nibble = (buffer[1] & 0xF) + to_add + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[1] = (buffer[1] & 0xF0) | nibble;
    
    // next digit, buffer[1], left nibble
    nibble = (buffer[1] >> 4) + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[1] = (nibble << 4) | (buffer[1] & 0x0F);
    
    // next digit, buffer[0], right nibble
    nibble = (buffer[0] & 0xF) + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[0] = (buffer[0] & 0xF0) | nibble;
    
    // next digit, buffer[0], left nibble
    nibble = (buffer[0] >> 4) + carry;
    carry = 0;
    if (nibble > 9) {
        nibble -= 10;
        carry = 1;
    }
    buffer[0] = (nibble << 4) | (buffer[0] & 0x0F);
     
    // if there's a carry, we have overflow
    if (carry > 0) {
        // ignore it? reset to zero? no idea.
        buffer[0] = 0x99;
        buffer[1] = 0x99;
        buffer[2] = 0x99;
        buffer[3] = 0x99;
    }
}

void BcdNum::subtract_bcd_tens(dword bcd_amount_in_tens)
{
    // essentially, we add the two nibbles and if we have a carry, we move on to the next.
    // score buffer: 0x00, 0x00, 0x00, 0x00
    // bcd_amount_in_tens: 0x1234
    // result buffer: 0x00, 0x01, 0x23, 0x40.
    
    char to_sub;
    char nibble;
    char carry;
    
    // buffer[3], left nibble
    to_sub = (bcd_amount_in_tens & 0x0F);
    nibble = (buffer[3] >> 4) - to_sub;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[3] = (nibble << 4) | (buffer[3] & 0x0F); 
    
    // next digit, buffer[2], right nibble
    to_sub = (bcd_amount_in_tens >> 4) & 0x0F;
    nibble = (buffer[2] & 0xF) - to_sub - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[2] = (buffer[2] & 0xF0) | nibble;
    
    // next digit, buffer[2], left nibble
    to_sub = (bcd_amount_in_tens >> 8) & 0x0F;
    nibble = (buffer[2] >> 4) - to_sub - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[2] = (nibble << 4) | (buffer[2] & 0x0F);
    
    // next digit, buffer[1], right nibble
    to_sub = (bcd_amount_in_tens >> 12) & 0x0F;
    nibble = (buffer[1] & 0xF) - to_sub - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[1] = (buffer[1] & 0xF0) | nibble;
    
    // next digit, buffer[1], left nibble
    nibble = (buffer[1] >> 4) - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[1] = (nibble << 4) | (buffer[1] & 0x0F);
    
    // next digit, buffer[0], right nibble
    nibble = (buffer[0] & 0xF) - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[0] = (buffer[0] & 0xF0) | nibble;
    
    // next digit, buffer[0], left nibble
    nibble = (buffer[0] >> 4) - carry;
    carry = 0;
    if (nibble < 0) {
        nibble += 10;
        carry = 1;
    }
    buffer[0] = (nibble << 4) | (buffer[0] & 0x0F);
     
    // if there's a carry, we have overflow
    if (carry > 0) {
        // ignore it? reset to zero? no idea.
    }
}
