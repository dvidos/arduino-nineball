/*
  
  menu selection
  ------------------
  pushing the key under the pinbal, we shuold cycle the various modes, pushing start would start a mode.
  the second button will cycle the item in that menu
  
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

  
  I want to find a way to automate lamp animations, even at the application level,
  without having to deal with them in the state machine. We should be able to 
  "launchAnimation(x)" and it should work to completion, upon which completion, 
  we could be notified (similar to a timeout) and we would update the lamps as should be.  
  
  Sameway, we might be able to be notified when a sound has finished.
  Maybe we will do this for long songs, set a long timer, having hardcoded the number of seconds
  the song is long. 
  
  
*/


#define USE_SERIAL_FOR_LOG_NO
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



void setup() {
	// INIT_LOG();
	// LOG("Hello from Arduino")
	
	noInterrupts();
	setup_timers();
	setup_pins();
	setup_sounds();
	interrupts();
}

void loop() {
	loop_pins();
	delay(100);
}

void every_second() {
}

void every_half_second() {
}

/*
	Updates to future self:
	- created timers interrupt handlers.
		- timer1, every second, for long running timeouts
		- timer3, every msec, for switch matrix, lamp matrix and score displays
		- timer4, every 100 msec for coils and animations
	- wanna move documentation and timers_init() to different file
*/


