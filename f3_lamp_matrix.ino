


class CLampMatrix
{
public:
    
    // 8 bytes of 8 bits => 64 lamps    
    byte lamps[8];
    byte current_send: 3; 
    
    CLampMatrix();
    
    void lamp_on(byte lamp_no);
    void lamp_off(byte lamp_no);
    bool is_on(byte lamp_no);
    
    void output_next_column();
};





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
    // lamp_no % 8 is the bit to turn off
    
    lamps[lamp_no >> 3] &= ~(1 << (lamp_no & 0x7));
}

bool CLampMatrix::is_on(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to check
    
    return (lamps[lamp_no >> 3] >> (lamp_no & 0x7)) & 0x01;
}

void CLampMatrix::output_next_column()
{
    // called from interrupt handler
    // we need to apply the next column to the ports

    // clear all ourputs for current column
    SET_LAMP_MATRIX_RETURNS_OCTET(0);
    
    // change the 3 bits that dictate the column
    current_send = (current_send + 1) & 0x7;
    
    SET_LAMP_MATRIX_DEMUX_A(currenr_send >> 2);
    SET_LAMP_MATRIX_DEMUX_B(currenr_send >> 1);
    SET_LAMP_MATRIX_DEMUX_C(currenr_send >> 0);
    
    // set all outputs for the new column
    SET_LAMP_MATRIX_RETURNS_OCTET(lamps[current_send]);
}

