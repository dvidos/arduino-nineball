/*
    From MIOS:
    -------------------------------------

    #define DISPLAY_BCD0           PORTBbits.RB0 // BCD value 1
    #define DISPLAY_BCD1           PORTBbits.RB1 // BCD value 2
    #define DISPLAY_BCD2           PORTBbits.RB2 // BCD value 4
    #define DISPLAY_BCD3           PORTBbits.RB3 // BCD value 8
    #define DISPLAY_DEMUX_A        PORTBbits.RB4 // current digit demux 0
    #define DISPLAY_DEMUX_B        PORTBbits.RB5 // current digit demux 1
    #define DISPLAY_DEMUX_C        PORTBbits.RB6 // current digit demux 2
    #define DISLAY_DEMUX_COM       PORTBbits.RB7 // demux enable (demux com)
    #define DISPLAY_BLANKING_LINE  PORTDbits.RD5 // blanking line (0=blanking off - display on, 1=blanking on - display off)
    #define DISPLAY_LEFT_LATCH     PORTDbits.RD6 // strobe left score display
    #define DISPLAY_RIGHT_LATCH    PORTDbits.RD7 // strobe right score display

    On the MIOS PCB, the port we had connected for the score displays, above the PIC microcontroller,
    has the following pings:

    top row, left to right, verified with schematic as well.
    B+  B-  Vs  Vd  V0  D5  D6  D7
    B7  B6  B5  B4  B3  B2  B1  B0

    each score display has the following signals:
      7 lines for specific digit enable, same for all displays      pins 4,5,6,7,8,9,
        stern uses pin 11 for 7th digit, bally and PinLED use pin 12.
      4 lines BCD data for the digit, same for all displays.        pins 16-19
      1 line display blanking, same for all displays.               pin 10
      1 line latch. different for each display.                     pin 15
      1 line +5VDC                                                  pin 20
      1 line GND                                                    pin 13
    ----
     13 lines total

    the CPU needs to output the following signals:
      7 lines for specific digit enable
      4 lines for BCD data
      1 line for digit blanking
      2 lines (one each display) for latching
    ----
     14 lines total

    unless we use a 74HCT238 for a demux, so we need:
      3 lines for specific digit enable
      4 lines BCD data
      1 line digit blanking
      2 lines for latching
    ----
     10 lines. we now have 11 pins free since we do not have an LCD.

    the cpu has to do the following:
        1. disable the current digit of the score displays.
        2. set the next digit selection among the 7 (shared among both score displays)
        3. for the LEFT score display:
            a. set the correct BCD value
            b. set the correct blanking line
            c. raise the correct latch,
            d. lower the correct latch,
        4. the same for the RIGHT score display
        5. enable the current digit.

    unsigned char DISP_DigitToShow;
    unsigned char DISP_DigitsValues[14];

    void DISP_Hide(unsigned char digit_num)
    {
        DISP_DigitsValues[13 - digit_num] = 0xF;
    }

    void DISP_Show(unsigned char digit_num, unsigned char value)
    {
        DISP_DigitsValues[13 - digit_num] = (value & 0xF);
    }

    void DISP_Show2Digits(unsigned char start_digit_num, unsigned char value)
    {
        DivideBy10(value);

        DISP_Show(start_digit_num,     (char)DivideBy10_Result);
        DISP_Show(start_digit_num + 1, (char)DivideBy10_Remainder);
    }

    void DISP_ShowAll(unsigned char value)
    {
        DISP_DigitsValues[ 0] = value;
        ...
        DISP_DigitsValues[13] = value;
    }

    void DISP_HideAll()
    {
        DISP_DigitsValues[ 0] = 0xF;
        ..
        DISP_DigitsValues[13] = 0xF;
    }

    int tick() {
        // move to next digit
        DISP_DigitToShow++;
        if (DISP_DigitToShow >= 7)
            DISP_DigitToShow = 0;

        // we need to lower the display line, before making any changes.
        DISLAY_DEMUX_COM = 0;
        DISPLAY_BLANKING_LINE = 1;
        __asm
            nop
            nop
            nop
        __endasm;

        // set the correct BCD value (lower 4 bits) and the blanking line (highest bit)
        i = DISP_DigitsValues[DISP_DigitToShow];
        DISPLAY_BCD0 = i;
        i >>= 1;
        DISPLAY_BCD1 = i;
        i >>= 1;
        DISPLAY_BCD2 = i;
        i >>= 1;
        DISPLAY_BCD3 = i;

        // latch the value into the register of the score display board
        DISPLAY_RIGHT_LATCH = 1;
        __asm
            nop
            nop
            nop
        __endasm;
        DISPLAY_RIGHT_LATCH = 0;

        // set the correct BCD value (lower 4 bits) and the blanking line (highest bit)
        i = DISP_DigitsValues[DISP_DigitToShow + 7];
        DISPLAY_BCD0 = i;
        i >>= 1;
        DISPLAY_BCD1 = i;
        i >>= 1;
        DISPLAY_BCD2 = i;
        i >>= 1;
        DISPLAY_BCD3 = i;

        // latch the value into the register of the score display board
        DISPLAY_LEFT_LATCH = 1;
        __asm
            nop
            nop
            nop
        __endasm;
        DISPLAY_LEFT_LATCH = 0;

        // now raise the new enabled digit
        DISPLAY_DEMUX_C = DISP_DigitToShow >> 2;
        DISPLAY_DEMUX_B = DISP_DigitToShow >> 1;
        DISPLAY_DEMUX_A = DISP_DigitToShow;
        __asm
            nop
            nop
            nop
        __endasm;
        DISLAY_DEMUX_COM = 1;
        DISPLAY_BLANKING_LINE = 0;
    }
*/






class CScoreDisplay
{
public:

    void init();

    // I think that on the pinled.de displays, 0xA - 0xF means don't show anything
    // so we don't need to keep a bitmap of what to display or not.

    struct {
       byte digits[4];   // most significant nibble is ignored
    } displays[2];

    byte last_digit_strobed;

    inline byte get_nibble_value(byte display_no, byte digit_no);
    inline void set_nibble_value(byte display_no, byte digit_no, byte value);

    // digit_no can be 0..15 from left to right, 0 and 8 are ignored
    // value can be 0..9
    void show_digit(byte digit_no, byte value);

    // this turns off the specific digit.
    // digit_no can be 0..15 from left to right, 0 and 8 are ignored
    void hide_digit(byte digit_no);
    void hide_all();

    // score_display can be: 0=for left, 1=for right.
    // turns off anything at the left that is zero
    void hide_display(byte display_no);
    void display_bcd_num(byte display_no, BcdNum& num);

    // this is called by interrupt handler every msec
    void ISR_strobe_next_display_digit();
};

void CScoreDisplay::init()
{
    hide_all();
    SET_SCORE_DISPLAY_PINS_MODE();
    LOG("Score Display initialized");
}

inline byte CScoreDisplay::get_nibble_value(byte display_no, byte digit_no)
{
    register byte offset = digit_no >> 1;

    // byte offset is digit_no / 2 (from 0..7 to 0..3)
    if (digit_no & 1) {
        // right nibble
        return displays[display_no].digits[offset] & 0xF;
    } else {
        // left nibble
        return displays[display_no].digits[offset] >> 4;
    }
}

inline void CScoreDisplay::set_nibble_value(byte display_no, byte digit_no, byte value)
{
    register byte offset = digit_no >> 1;

    // byte offset is digit_no / 2 (from 0..7 to 0..3)
    if (digit_no & 1) {
        // right nibble
        displays[display_no].digits[offset] =
            (displays[display_no].digits[offset] & 0xF0) | (value & 0xF);
    } else {
        // left nibble
        displays[display_no].digits[offset] =
            (value << 4) | (displays[display_no].digits[offset] & 0x0F);
    }
}

void CScoreDisplay::show_digit(byte digit_no, byte value)
{
    register byte display_no = digit_no >> 3; // 0..15 becomes 0..1
    digit_no &= 0x7; // keep it to 0..7 range
    set_nibble_value(display_no, digit_no, value);

    LOG_DISPLAY(displays[0].digits, displays[1].digits);
}

void CScoreDisplay::hide_digit(byte digit_no)
{
    // the BCD-to-7segments decoder used does not display A-F hexadecimal,
    // so we use this to "turn off" a specific digit.
    show_digit(digit_no, 0xF);

    LOG_DISPLAY(displays[0].digits, displays[1].digits);
}

void CScoreDisplay::hide_all()
{
    memset(displays[0].digits, 0xFF, 4);
    memset(displays[1].digits, 0xFF, 4);

    LOG_DISPLAY(displays[0].digits, displays[1].digits);
}

void CScoreDisplay::hide_display(byte display_no)
{
    memset(displays[display_no & 1].digits, 0xFF, 4);

    LOG_DISPLAY(displays[0].digits, displays[1].digits);
}

void CScoreDisplay::display_bcd_num(byte display_no, BcdNum& num)
{
    register byte first_non_zero_found = 0;
    register byte value;

    // go from left to right. detect first non-zero value, hide the ones to its left.
    for (byte nibble = 0; nibble < 8; nibble++) {
        value = num.get_nibble(nibble);

        if (value >= 1 && value <= 9) {
            first_non_zero_found = 1;
        } else {
            // value is zero. should we display it?
            // the last nibble should always be displayed.
            if (!first_non_zero_found && nibble < 7)
                value = 0xF;
        }

        set_nibble_value(display_no, nibble, value);
    }

    LOG_DISPLAY(displays[0].digits, displays[1].digits);
}


void CScoreDisplay::ISR_strobe_next_display_digit()
{
    register byte value;

    last_digit_strobed += 1;
    if (last_digit_strobed >= 7)
        last_digit_strobed = 0;

    // turn off the 3-to-8 demux
    SET_SCORE_DISPLAY_DEMUX_OUTPUT(0);
    SET_SCORE_DISPLAY_BLANKING_LINE(1);
    NOP();
    NOP();

    // left display (0) first
    value = get_nibble_value(0, last_digit_strobed);
    SET_SCORE_DISPLAY_BCD_BIT_3(value >> 3);
    SET_SCORE_DISPLAY_BCD_BIT_2(value >> 2);
    SET_SCORE_DISPLAY_BCD_BIT_1(value >> 1);
    SET_SCORE_DISPLAY_BCD_BIT_0(value >> 0);

    // latch this into the digit's memory
    SET_SCORE_DISPLAY_LEFT_LATCH(1);
    NOP();
    NOP();
    SET_SCORE_DISPLAY_LEFT_LATCH(0);

    // right display (1) next
    value = get_nibble_value(1, last_digit_strobed);
    value = value;
    SET_SCORE_DISPLAY_BCD_BIT_3(value >> 3);
    SET_SCORE_DISPLAY_BCD_BIT_2(value >> 2);
    SET_SCORE_DISPLAY_BCD_BIT_1(value >> 1);
    SET_SCORE_DISPLAY_BCD_BIT_0(value >> 0);

    // latch this into the digit's memory
    SET_SCORE_DISPLAY_RIGHT_LATCH(1);
    NOP();
    NOP();
    SET_SCORE_DISPLAY_RIGHT_LATCH(0);

    SET_SCORE_DISPLAY_DEMUX_C(last_digit_strobed >> 2);
    SET_SCORE_DISPLAY_DEMUX_B(last_digit_strobed >> 1);
    SET_SCORE_DISPLAY_DEMUX_A(last_digit_strobed >> 0);

    NOP();
    NOP();

    // now enable things back on (how are we hiding digits???)
    SET_SCORE_DISPLAY_DEMUX_OUTPUT(1);
    SET_SCORE_DISPLAY_BLANKING_LINE(0);
}


