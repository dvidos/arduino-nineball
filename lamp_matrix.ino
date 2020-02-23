
#include "lamp_matrix.h" 

CLampMatrix::CLampMatrix()
{
    memset(lamps, 0, 8);
    current_send = 0;
}

void CLampMatrix::lamp_on(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to turn on
    
    lamps[lamp_no >> 3] |= (1 << (lamp_no & 0x7));
}

void CLampMatrix::lamp_off(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to turn on
    
    lamps[lamp_no >> 3] &= ~(1 << (lamp_no & 0x7));
}

bool CLampMatrix::is_on(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to turn on
    
    return (lamps[lamp_no >> 3] >> (lamp_no & 0x7)) & 0x01;
}

void CLampMatrix::output_next_column()
{
    // called from interrupt handler
    // we need to apply the next column to the ports

    // clear all ourputs for current column
    // say, PORTD = 0
    
    // change the 3 bits that dictate the column
    current_send = (current_send + 1) & 0x7;
    // say SET(PORT4, 3, (current_send >> 2))
    // say SET(PORT4, 6, (current_send >> 1))
    // say SET(PORT4, 2, (current_send >> 0))
    
    // set all outputs for the new column
    // say, PORTD = lamps[current_send];
}

