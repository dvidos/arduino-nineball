
/**
 * Created by Dimitris Vidos, for a custom Stern Nineball pinball machine.
 * All rights reserved.
 *
 * All .ino and .pde files in the sketch folder (shown in the IDE as tabs with no extension) 
 * are concatenated together, starting with the file that matches the folder name followed 
 * by the others in alphabetical order, and the .cpp extension is added to the filename.
 *
 * See also: https://github.com/arduino/Arduino/wiki/Build-Process
 */
#define RUN_TESTS              // comment out to run normal code
#define LOG_ON_SERIAL_MONITOR  // comment out to stop sending log messages to the serial port (USB cable)

#define SWITCH_MATRIX_EVENTS_QUEUE_SIZE   16

#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;


#ifdef LOG_ON_SERIAL_MONITOR
    #define LOG_INIT()      Serial.begin(9600); Serial.println("Serial Log initialized");      
    #define LOG(...)        log_info(__VA_ARGS__)
    void log_info(const char *fmt, ...) {
        char buffer[128];
        va_list args_list;
        va_start(args_list, fmt);
        vsprintf(buffer, fmt, args_list);
        Serial.println(buffer);
    }
#else
    #define LOG_INIT()      (void)0
    #define LOG(fmt, ...)   (void)0
#endif // LOG_ON_USB_CABLE

#include <wavTrigger.h>
#include <EEPROM.h>

#include "constants.h"
#include "pins.h"

#include "bcdnum.h"
#include "audio.h"
#include "lamp_matrix.h"
#include "switch_matrix.h"
#include "coils.h"
#include "score_display.h"
#include "animations.h"
#include "game_settings.h"
#include "gameplay.h"
#include "attract.h"

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CGameSettings GameSettings;
CScoreDisplay ScoreDisplay;
CAnimator Animator;
CCoils Coils;
CGameplay Gameplay;
CAttract Attract;

#include "timers.h"
#include "tests.h"

void setup() {
    #ifdef RUN_TESTS
        run_tests();
    #else
    	noInterrupts();
    	
    	setup_timers();
    	setup_pins();
    	setup_sounds();
    	interrupts();
    #endif
}

void loop() {
    #ifdef RUN_TESTS
    #else
    #endif
}






