/*
	From MIOS:
	-------------------------------------

	#define DISPLAY_BCD0	       PORTBbits.RB0 // BCD value 1
	#define DISPLAY_BCD1	       PORTBbits.RB1 // BCD value 2
	#define DISPLAY_BCD2	       PORTBbits.RB2 // BCD value 4
	#define DISPLAY_BCD3	       PORTBbits.RB3 // BCD value 8
	#define DISPLAY_DEMUX_A	       PORTBbits.RB4 // current digit demux 0
	#define DISPLAY_DEMUX_B	       PORTBbits.RB5 // current digit demux 1
	#define DISPLAY_DEMUX_C	       PORTBbits.RB6 // current digit demux 2
	#define DISLAY_DEMUX_COM	   PORTBbits.RB7 // demux enable (demux com)
	#define DISPLAY_BLANKING_LINE  PORTDbits.RD5 // blanking line (0=blanking off - display on, 1=blanking on - display off)
	#define DISPLAY_LEFT_LATCH	   PORTDbits.RD6 // strobe left score display
	#define DISPLAY_RIGHT_LATCH	   PORTDbits.RD7 // strobe right score display
	
	each score display has the following signals:
	  7 lines for specific digit enable, same for all displays		pins 4,5,6,7,8,9,
	    stern uses pin 11 for 7th digit, bally and PinLED use pin 12.

	  4 lines BCD data for the digit, same for all displays.		pins 16-19
	  1 line display blanking, same for all displays.				pin 10
	  1 line latch. different for each display.						pin 15
	  1 line +5VDC													pin 20
	  1 line GND													pin 13
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
		DISP_DigitsValues[ 1] = value;
		DISP_DigitsValues[ 2] = value;
		DISP_DigitsValues[ 3] = value;
		DISP_DigitsValues[ 4] = value;
		DISP_DigitsValues[ 5] = value;
		DISP_DigitsValues[ 6] = value;
		DISP_DigitsValues[ 7] = value;
		DISP_DigitsValues[ 8] = value;
		DISP_DigitsValues[ 9] = value;
		DISP_DigitsValues[10] = value;
		DISP_DigitsValues[11] = value;
		DISP_DigitsValues[12] = value;
		DISP_DigitsValues[13] = value;
	}
	
	void DISP_HideAll()
	{
		DISP_DigitsValues[ 0] = 0xF;
		DISP_DigitsValues[ 1] = 0xF;
		DISP_DigitsValues[ 2] = 0xF;
		DISP_DigitsValues[ 3] = 0xF;
		DISP_DigitsValues[ 4] = 0xF;
		DISP_DigitsValues[ 5] = 0xF;
		DISP_DigitsValues[ 6] = 0xF;
		DISP_DigitsValues[ 7] = 0xF;
		DISP_DigitsValues[ 8] = 0xF;
		DISP_DigitsValues[ 9] = 0xF;
		DISP_DigitsValues[10] = 0xF;
		DISP_DigitsValues[11] = 0xF;
		DISP_DigitsValues[12] = 0xF;
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
		DISPLAY_DEMUX_C	= DISP_DigitToShow >> 2;
		DISPLAY_DEMUX_B	= DISP_DigitToShow >> 1;
		DISPLAY_DEMUX_A	= DISP_DigitToShow;
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
    
    struct {
       byte digits[4];   // most significant nibble is ignored
       byte lit_digits;  // most significant bit is ignored
    } displays[2];
    
    byte next_display_digit_to_strobe = 0;    
    
    // digit_no can be 0..15 from left to right, 0 and 8 are ignored
    // value can be 0..9
    void show_digit(byte digit_no, byte value);
    
    // this turns off the specific digit.
    // digit_no can be 0..15 from left to right, 0 and 8 are ignored
    void hide_digit(byte digit_no);
    void hide_all();
    
    // score_display can be: 0=for left, 1=for right.
    // turns off anything at the left that is zero
    void display_bcd_num(byte score_display, const BcdNum &num);
    
    // this is called by interrupt handler every msec 
    void ISR_strobe_next_display_digit();
};


void CScoreDisplay::show_digit(byte digit_no, byte value)
{
    digit_no &= 0xF;               // force 0..15
    value &= 0xF;                  // force 0..15
    byte display = digit_no >> 3;  // so that 8+ means 2nd display
    digit_no &= 0x7;               // make digit no in the range 0..7
    byte offset = digit_no >> 1;   // digit / 2 is the byte offset (0..3)
    
    if (digit_no & 1) {
        // right nibble
        displays[display].digits[offset] = (displays[display].digits[offset] & 0xF0) | (value & 0xF);
    } else {
        // left nibble
        displays[display].digits[offset] = (value << 4) | (displays[display].digits[offset] & 0xF);
    }
    
    // turn on that digit too
    displays[display].lit_digits |= (1 << (7 - digit_no))
}

void CScoreDisplay::hide_digit(byte digit_no)
{
    digit_no &= 0xF;               // force 0..15
    byte display = digit_no >> 3;  // so that 8+ means 2nd display
    digit_no &= 0x7;               // make digit no in the range 0..7
    
    // turn on that digit too
    displays[display].lit_digits &= ~(1 << (7 - digit_no))
}

void CScoreDisplay::hide_all()
{
    displays[0].lit_digits = 0
    displays[1].lit_digits = 0
}

void CScoreDisplay::display_bcd_num(byte score_display, const BcdNum &num)
{
    digit_no &= 0xF;               // force 0..15
    byte display = digit_no >> 3;  // so that 8+ means 2nd display
    digit_no &= 0x7;               // make digit no in the range 0..7
    
    displays[display].digits[0] = num->bcd[0];
    displays[display].digits[1] = num->bcd[1];
    displays[display].digits[2] = num->bcd[2];
    displays[display].digits[3] = num->bcd[3];
    
    // elimintate zeros, find the first nonzero item
    byte mask = 0;
    if (displays[display].digits[0] & 0x0F)
        mask |= 0x40;
    if (mask || (displays[display].digits[1] & 0xF0))
        mask |= 0x20;
    if (mask || (displays[display].digits[1] & 0x0F))
        mask |= 0x10;
    if (mask || (displays[display].digits[2] & 0xF0))
        mask |= 0x08;
    if (mask || (displays[display].digits[2] & 0x0F))
        mask |= 0x04;
    if (mask || (displays[display].digits[3] & 0xF0))
        mask |= 0x02;
    if (mask || (displays[display].digits[3] & 0x0F))
        mask |= 0x01;
    
    displays[display].lit_digits = mask
}


void CScoreDisplay::ISR_strobe_next_display_digit()
{
}


