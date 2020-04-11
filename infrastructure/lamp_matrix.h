


class CLampMatrix
{
public:

    // 8 bytes of 8 bits => 64 lamps
    byte lamp_rows[8];
    byte current_row: 3;

    CLampMatrix();
    void init();

    void lamp_on(byte lamp_no);
    void lamp_off(byte lamp_no);
    void set_lamp(byte lamp_no, bool value);
    bool is_on(byte lamp_no);
    void all_off();
    void column_on(byte column_no);
    void row_on(byte row_no);

    void output_next_column();
};





CLampMatrix::CLampMatrix()
{
    memset(lamp_rows, 0, 8);
    current_row = 0;
}

void CLampMatrix::init()
{
    SET_LAMP_MATRIX_PINS_MODE();
    LOGM(M_LAMP_MATRIX_INITIALIZED);
}

void inline CLampMatrix::lamp_on(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to turn on

    lamp_rows[lamp_no >> 3] |= (1 << (lamp_no & 0x7));
}

void inline CLampMatrix::lamp_off(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to turn off

    lamp_rows[lamp_no >> 3] &= ~(1 << (lamp_no & 0x7));
}

void inline CLampMatrix::set_lamp(byte lamp_no, bool value)
{
    if (value)
        lamp_on(lamp_no);
    else
        lamp_off(lamp_no);
}

bool CLampMatrix::is_on(byte lamp_no)
{
    // lamp_no / 8 is the byte to store
    // lamp_no % 8 is the bit to check

    return (lamp_rows[lamp_no >> 3] >> (lamp_no & 0x7)) & 0x01;
}

void CLampMatrix::all_off()
{
    memset(lamp_rows, 0, 8);
    LOGM(M_ALL_LAMPS_OFF);
}
void CLampMatrix::column_on(byte column_no)
{
    memset(lamp_rows, (1 << column_no), 8);
    LOGM(M_LAMPS_COLUMN_D_ON, column_no);
}
void CLampMatrix::row_on(byte row_no)
{
    lamp_rows[(row_no & 0x07)] = 0xFF;
    LOGM(M_LAMPS_ROW_D_ON, row_no);
}



void CLampMatrix::output_next_column()
{
    // called from interrupt handler
    // we need to apply the next column to the ports

    // clear all ourputs for current column
    SET_LAMP_MATRIX_RETURNS_OCTET(0);

    // change the 3 bits that dictate the column
    current_row = (current_row + 1) & 0x7;

    SET_LAMP_MATRIX_DEMUX_A(currenr_send >> 2);
    SET_LAMP_MATRIX_DEMUX_B(currenr_send >> 1);
    SET_LAMP_MATRIX_DEMUX_C(currenr_send >> 0);

    // set all outputs for the new column
    SET_LAMP_MATRIX_RETURNS_OCTET(lamp_rows[current_row]);
}

