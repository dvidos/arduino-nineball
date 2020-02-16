/*
  
  
  menu selection
  ------------------
  pushing the key under the pinbal, we shuold cycle the various modes, pushing start would start a mode.
  0 = normal play (e.g. what start will do if no black button is pressed)
  1 = happy mode, infinite balls
  2 = practice mode, specific targets.
  ...
  8 = settings menu
  9 = diagnostics menu
  
  
  watchdog
  -----------
  if the pin is free, have the led being lit every two seconds or so, 
  to understand whether we fell in an infinite loop or something.
  
  
  
  variables
  --------------
  keep everything in 3-4 large structures with bit fields 
  to preserve space and allow for easy reporting of sizeof()
  
  
  timing
  -----------
  should keep time in 10 or 100 msecs increments. 
  A char would allow up to 2.5 seconds or 25 seconds duration which is good for coils, animations, lamps etc.
  
  Should have a way of keeping some stats, such as max lamps blinking at the time and max switches queue length.
  
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
  
  // here's how to define the ISR for a timer:
  ISR(TIMER1_COMPA_vect) {
    Led1.Refresh();
  }
  
  I want to find a way to automate lamp animations, even at the application level,
  without having to deal with them in the state machine. We should be able to 
  "launchAnimation(x)" and it should work to completion, upon which completion, 
  we could be notified (similar to a timeout) and we would update the lamps as should be.  
  
  Sameway, we might be able to be notified when a sound has finished.
  Maybe we will do this for long songs, set a long timer, having hardcoded the number of seconds
  the song is long. 
  
*/

#define ARDUINO_BUILT_IN_LED  13

#define USE_SERIAL_FOR_LOG  1
#ifdef USE_SERIAL_FOR_LOG
	#define INIT_LOG()    Serial.begin(9600); Serial.println("Serial Log initialized");
	#define LOG(message)  Serial.println(message);     
#else
	#define INIT_LOG()    ;
	#define LOG(message)  ;     
#endif


#define MODE_ATTRACT           0
#define MODE_HAPPY_HOUR_GAME   1
#define MODE_PRACTICE_GAME     2
#define MODE_JUKE_BOX          3
#define MODE_NORMAL_GAME       4
#define MODE_SETTINGS          5
#define MODE_DIAGNOSTICS       6

#define DIAGNOSTICS_LAMP_MATRIX_COLUMNS          1 # selects which column
#define DIAGNOSTICS_LAMP_MATRIX_ROWS             2 # B2 selects which row
#define DIAGNOSTICS_LAMP_MATRIX_LAMP             3 # B2 selects which lamp
#define DIAGNOSTICS_LAMP_MATRIX_ALL              4 # all lamps blink on-off every second   
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_NUMBER  5 # B2 selects which number all displays should show 0-9
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_DIGIT   6 # B2 selects which digit will display 8 or F, 0 to 13
#define DIAGNOSTICS_SWITCH_MATRIX                7 # number of switch closed displayed (B1/B2 are excluded)
#define DIAGNOSTICS_COILS                        8 # B2 selects which coild to fire, fired every 2 secs?
#define DIAGNOSTICS_Q_RELAY                      9 # B2 selects whether q relay is energized or not
#define DIAGNOSTICS_SOUND                       10 # B2 selects sound to fire. 

#define GAME_MODE_BALL_EJECTING    0
#define GAME_MODE_BALL_SETTING_UP_NUMBER    0
#define GAME_MODE_BALL_WAITING_FOR_NUMBER_1    0
#define GAME_MODE_BALL_DONE_NUMBER_1           0

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;
typedef unsigned long long uint64;


typedef struct {
	uint8 num_players;
	uint8 curr_player_no;
	struct {
		uint8 ball_no;
		uint8 game_state;
		uint32 score;
		uint32 bonus;
		uint8 bonus_multiplier;
	} player[4];
} normal_game_info_t;

typedef struct {
	uint8 curr_mode_selection;
} attract_mode_info_t;

typedef struct {
	uint8 menu_sel;
	uint8 item_sel;
} diagnostics_mode_info_t;

typedef struct {
	uint8 mode;
	union {
		attract_mode_info_t     attract;
		diagnostics_mode_info_t diagnostics;
		normal_game_info_t      game;
	} data;
} memory_t;

// all memory for the game. easy to sizeof()
memory_t memory;



void setup_timers() {
	/*
	
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

int x = 0;


/**
 * Timer 1, currently running every sec
 */
ISR(TIMER1_COMPA_vect){
   //noInterrupts();
   digitalWrite(13, !digitalRead(13));
}

/**
 * Timer 3, currently running every msec
 */
ISR(TIMER3_COMPA_vect){
   noInterrupts();
   
//   x += 1;
//   if (x >= 1000) {
//	   digitalWrite(13, !digitalRead(13));
//	   x = 0;
//   }
}


/**
 * Timer 4, currently running every 100 msec
 */
ISR(TIMER4_COMPA_vect){
   noInterrupts();
   
//   x += 1;
//   if (x >= 10) {
//	   digitalWrite(13, !digitalRead(13));
//	   x = 0;
//   }
}



void setup() {
	INIT_LOG();
	LOG("Hello from Arduino")
	
	pinMode(13, OUTPUT);
	digitalWrite(13, 0);
	
	setup_timers();
}

void loop() {
	delay(3000);
}

/*
	Updates to future self:
	- created timers interrupt handlers.
		- timer1, every second, for long running timeouts
		- timer3, every msec, for switch matrix, lamp matrix and score displays
		- timer4, every 100 msec for coils and animations
	- wanna move documentation and timers_init() to different file
*/


