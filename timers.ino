/*
  Timers
  ---------
  Timers 0 and 2, 8 bits
  Timer 1, 16 bits
  On mega, there are also timers 3, 4, 5, 16 bits
  Timer 0 is for delay(), millis() and micro()
  Timer 1 is for servo library (we don't care)
  Timer 2 is for tone (we don't care)
  
  noInterrupts() will disable interrupts (inside the IRSR)
  interrupts() enables them
  well, quite a few pseudo-variables to set. TCCR3A, TCCR3B, OCR3A etc
  quite a lot to digest, here: https://oscarliang.com/arduino-timer-and-interrupt-tutorial/
  but I think we can make an interrupt timer for milliseconds (lamp & matrix switch)
  and another timer for timeouts (coils and game timeouts)
  
	timer1 is not natively used for something
	arduino mega also has timers 3, 4, 5 that we could use
	for now, we want the interrupt routine to be called every.. 1 millisecond.
	we will update lamps and switch matrices, and we will update timers every 10 ms
	this allows 8bit timers to run up to 2.5 seconds (e.g. coild, animations etc)
	and we can implement seconds timers for up to 255 secs = 4.25 minutes.
	
	each timer has a control register,
	a compare match register (to match against a specific value, not necessarily zero)
	and a prescaler
	
	prescaler   speed
	- 1           16 MHz
	- 8           2 MHz
	- 64          250 Khz
	- 256         62.5 KHz
	- 1024        1562,5 Hz
	
	interrupt_freq_Hz = 16,000,000 / (prescaler * (compare_register + 1))
	desired_compare_register = [ 16,000,000 / (prescaler * desired_freq_hz) ] - 1
	
	for timer1, the compare register can be between 0..65535
	for interrupt every second, you'd use: prescaler=1024, compare=15624
	
	https://oscarliang.com/arduino-timer-and-interrupt-tutorial/
	
	Arduino Mega has 6 timers. Timers 0 and 2 are 8 bit, timers 1, 3, 4, 5 are 16 bit.
	- timer0 is used for millis() and time functions
	- timer1 is used by servo library on Arduino Uno
	- timer2 is used for tone() function
	- timer3
	- timer4 
	- timer5 is used by servo library on Arduino Mega  
		
	registers (where n = 1, 3, 4, 5, x=A/B/C):
	- TCNTn  - Timer/Counter Value Registers. The actual timer value is stored here. 16 bits. 
	- TCCRn  - Timer/Counter Control Registers. The pre-scaler can be configured here. 16 bits, (except timers 0 & 2).
	- OCRnx  - Output Compare Registers. 16 bits
	           this, in some modes, is the TOP value the counter counts to.
	- ICRnx  - Input Capture Registers. 16 bits (only for 16bit timers)
				  this is to capture the value TCNTx upon some event, typically, ICP1 (arduino pin 8) 
	- TIMSKn - Timer/Counter Interrupt Mask Register. To enable/disable timer interrupts.
	- TIFRn  - Timer/Counter Interrupt Flag Register. Indicates a pending timer interrupt.

	The Timer/Counter (TCNTn), Output Compare Registers (OCRnA/B/C), and Input Capture Register (ICRn) are all
	16-bit registers. Special procedures must be followed when accessing the 16-bit registers. These procedures are
	described in the section “Accessing 16-bit Registers” on page 135. The Timer/Counter Control Registers
	(TCCRnA/B/C) are 8-bit registers and have no CPU access restrictions. Interrupt requests (shorten as Int.Req.)
	signals are all visible in the Timer Interrupt Flag Register (TIFRn). All interrupts are individually masked with the
	Timer Interrupt Mask Register (TIMSKn). TIFRn and TIMSKn are not shown in the figure since these registers are
	shared by other timer units.	
	
	- TCCRnA bits
		7: COMnA1 R/W
		6: COMnA0 R/W
		5: COMnB1 R/W
		4: COMnB0 R/W
		3: COMnC1 R/W
		2: COMnC0 R/W
		1: WGMn1  R/W
		0:	WGMn0  R/W
	- TCCRxB bits
		7: ICNCn  R/W
		6: ICESn  R/W
		5: -      R
		4: WGMn3  R/W
		3: WGMn2  R/W
		2: CSn2   R/W
		1: CSn1   R/W
		0: CSn0   R/W
	
	- WGM  = Waveform Generation Mode.
	- COM  = Compare Output Mode
	- ICNC = Input Capture Noise Canceler (1 activates, 0 deactivates)
	- ICES = Input Capture Edge Select (0 failing edge, 1 rising edge)
	- CS   = Clock Selection Bits
	

	- WGM values
	  WGMn3 WGMn2 WGMn1 WGMn0 Description                TOP     Update OCRnx at TOVn flag set on
	    0     0     0     0   Normal counting            0xFFFF  Immediate       MAX
	    0     0     0     1   PWM, phase correct, 8bit   0x00FF  TOP             BOTTOM
	    0     0     1     0   PWM, phase correcr, 9bit   0x01FF  TOP             BOTTOM
	    0     0     1     1   PWM, phase correcr, 10bit  0x03FF  TOP             BOTTOM
	    0     1     0     0   CTC                        OCRnA   Immediate       MAX
	    0     1     0     1   Fast PWM, 8bit             0x00FF  TOP             BOTTOM
	    0     1     1     0   Fast PWM, 9bit             0x01FF  TOP             BOTTOM
	    0     1     1     1   Fast PWM, 10bit            0x03FF  TOP             BOTTOM
	    1     0     0     0   PWM phase & freq correct   ICRn    BOTTOM          BOTTOM
	    1     0     0     1   PWM phase & freq correct   OCRnA   BOTTOM          BOTTOM
	    1     0     1     0   PWM phase correct          ICRn    TOP             BOTTOM
	    1     0     1     1   PWM phase correct          OCRnA   TOP             BOTTOM
	    1     1     0     0   CTC                        ICRn    Immediate       MAX
	    1     1     0     1   (reserved)
	    1     1     1     0   Fast PWM                   ICRn    BOTTOM          TOP
	    1     1     1     1   Fast PWM                   OCRnA   BOTTOM          TOP
	
	- COM values for non-PWM modes (n=1,3,4,5, x=A/B/C). PWM modes have different values, but I don't care.
	  COMnx1  COMnx0    Description
	    0       0       Normal operation, OCnx disconnected.
	    0       1       Toggle OCnx on compare match
	    1       0       Clear OCnx on compare match (set output to low level)
	    1       1       Set OCnx on compare match (set output to high level)
	    
	  
	- CS12, CS11, CS10 values
		000 - no clock source (timer is off, does not count)
		001 - clock, no prescaling
		010 - clock, prescale / 8
		011 - clock, prescale / 64
		100 - clock, prescale / 256
		101 - clock, prescale / 1024
		110 - external clock on T1, falling edge
		111 - external clock on T1, rising edge
	
	- TIMSKn Timer Interrupt Mask Register
	  bit 7  -
	  bit 6  -
	  bit 5  ICIEn   Input capture interrupt enable
	  bit 4  -
	  bit 3  OCIEnC  Output compare C match interrupt enable
	  bit 2  OCIEnB  Output compare B match interrupt enable
	  bit 1  OCIEnA  Output compare A match interrupt enable
	  bit 0  TOIEn   Timer Overflow Interrupt enable
	
	- TIFRn Timer Interrupt Flag Register
	  Follows same format at TIMSKn, has one when an interrupt should be fired. 
	  Flags are cleared automatically after the interrupt vector is executed.
	  
	
	Timer Overflow:
	
	Timer overflow means the timer has reached is limit value. 
	When a timer overflow interrupt occurs, the timer overflow bit TOVx 
	will be set in the interrupt flag register TIFRx. When the timer overflow 
	interrupt enable bit TOIEx in the interrupt mask register TIMSKx is set, 
	the timer overflow interrupt service routine ISR(TIMERx_OVF_vect) will be called.

	Output Compare Match:
	
	When a output compare match interrupt occurs, the OCFxy flag will be set in the 
	interrupt flag register TIFRx . When the output compare interrupt enable bit OCIExy 
	in the interrupt mask register TIMSKx is set, the output compare match interrupt 
	service ISR(TIMERx_COMPy_vect) routine will be called.		

	Timer Input Capture:
	
	When a timer input capture interrupt occurs, the input capture flag bit ICFx 
	will be set in the interrupt flag register TIFRx. When the input capture interrupt 
	enable bit  ICIEx in the interrupt mask register TIMSKx is set, the timer input 
	capture interrupt service routine ISR(TIMERx_CAPT_vect) will be called.
	
	Each timer has 16 modes of operations, configured by TCCRxA & TCCRxB.
	Ignoring the PWM modes, as I don't really care about them, here are the most intersting ones:
	Code Mode                   Top      Update of OCRxy    TOVx flag set
	00   Normal                 0xFFFF   Immediate          MAX
	04   CTC                    OCRxA    Immediate          MAX
	12   CTC                    ICRxB    Immediate          MAX 
	
	In the PWM modes (that we don't care), the counters count up and maybe down or reset to zero,
	depending on the mode, setting their output pins.
	
	CTC mode is the normal counting more. The counter counts up, depending on the clock and prescaler
	setup, It could count external events, if setup to external clock, read from T1, arduino pin 5.
	
	For the best explanation, the AtMega2560 datasheet is actually the most comprehensive...
	chapter 16 (p.115) for 8 bits, chapter 17 (p.133) for 16 bits  
			
	The functions to be called:	
	- ISR(TIMERx_OVF_vect)   - on timer overflow, i.e. max value reached
	- ISR(TIMERx_COMPy_vect) - on comparator value reched (y=A or B)
	
	
*/

void setup_timers() {
	
	// timer1 -> 1 sec
	// timer3 -> 1 msec
	// timer4 -> 100 msecs
	
	
	// set compare match register for 1hz increments	
	TCCR1A = 0;                           // set entire TCCR1A register to 0
	TCCR1B = 0;                           // same for TCCR1B
	TCNT1  = 0;                           //initialize counter value to 0
	OCR1A = 15624;                        // = (16,000,000) / (1*1024) - 1 (must be <65536)
	TCCR1B |= (1 << WGM12);               // turn on CTC mode
	TCCR1B |= (1 << CS12) | (1 << CS10);  // Set CS10 and CS12 bits for 1024 prescaler  
	TIMSK1 |= (1 << OCIE1A);              // enable timer compare interrupt
	
	// timer 3 to 1 msec - prescaler 64, compare value 249.
	TCCR3A = 0;                           // set entire TCCRnA register to 0
	TCCR3B = 0;                           // same for TCCRnB
	TCNT3  = 0;                           // initialize counter value to 0
	OCR3A = 249;                          // = (16,000,000) / (64 * 1000) - 1 (must be <65536)
	TCCR3B |= (1 << WGM32);               // turn on CTC mode
	TCCR3B |= (1 << CS31) | (1 << CS30);  // Set CS11 and CS10 bits for 64 prescaler  	
	TIMSK3 |= (1 << OCIE3A);              // enable timer compare interrupt
	
	// timer 4 to 100 msecs - prescaler 64, compare value 24999.
	TCCR4A = 0;                           // set entire TCCRnA register to 0
	TCCR4B = 0;                           // same for TCCRnB
	TCNT4  = 0;                           // initialize counter value to 0
	OCR4A = 24999;                        // = (16,000,000) / (64 * 10) - 1 (must be <65536)
	TCCR4B |= (1 << WGM42);               // turn on CTC mode
	TCCR4B |= (1 << CS41) | (1 << CS40);  // Set CS11 and CS10 bits for 64 prescaler  	
	TIMSK4 |= (1 << OCIE4A);              // enable timer compare interrupt
}


/**
 * Timer 1, currently running every sec
 */
ISR(TIMER1_COMPA_vect){
   noInterrupts();
}

/**
 * Timer 3, currently running every msec
 */
ISR(TIMER3_COMPA_vect){
   noInterrupts();
   
   LampMatrix.output_next_column();
   SwitchMatrix.intercept_next_row();
}

/**
 * Timer 4, currently running every 100 msec
 */
ISR(TIMER4_COMPA_vect){
   noInterrupts();
}

