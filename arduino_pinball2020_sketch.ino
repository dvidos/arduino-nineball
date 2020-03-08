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
 

/**
 * Whether we want to LOG() onto the serial monitor.
 * Commenting this out makes the LOG() macro to do nothing.
 */
#define LOG_ON_SERIAL_MONITOR

/**
 * Software tests is a function that runs various unit tests.
 * Results logged in serial monitor
 *
 * Expected hardware: USB cable only
 */
#define RUN_SOFTWARE_TESTS

/**
 * Hardware tests is a small applet to run various operations,
 * mainly verifying the cooperation of software and hardware.
 * 
 * Expected hardware: A0-A7: buttons to earth, A8-A15: LEDs
 */
#define RUN_HARDWARE_TESTS




#include <Arduino.h>
#include <wavTrigger.h>
#include <EEPROM.h>
#include "defines.h"
#include "constants.h"
#include "pins.h"
#include "bcdnum.h"
#include "audio.h"
#include "lamp_matrix.h"
#include "switch_matrix.h"
#include "coils.h"
#include "score_display.h"
#include "time_keeper.h"
#include "animations.h"
#include "game_settings.h"
#include "gameplay.h"
#include "attract.h"
#include "timers.h"
#include "tests_software.h"
#include "tests_hardware.h"

/**
 * Declaring global objects affords us avoiding malloc() and nasty out-of-memory surprises
 */
CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CScoreDisplay ScoreDisplay;
CGameSettings GameSettings;
CAnimator Animator;
CTimeKeeper TimeKeeper;
CCoils Coils;
CGameplay Gameplay;
CAttract Attract;



void setup() {
    LOG_INIT();
    LOG("setup() starting");
    
    #if defined(RUN_SOFTWARE_TESTS)
    	setup_timers();
    	
        run_software_tests();
        
    #elif defined(RUN_HARDWARE_TESTS)
        hardware_tests_init();
        
    #else
    	noInterrupts();
    	
    	setup_timers();
    	setup_pins();
    	setup_sounds();
    	
    	Audio.init();
    	LampMatrix.init();
    	SwitchMatrix.init();
    	ScoreDisplay.init();
    	GameSettings.init();
    	Coils.init();
    	
    	interrupts();
    	
    	Attract.start();
    #endif
    
    LOG("setup() finished");
}

void loop() {
    #if defined(RUN_SOFTWARE_TESTS)
        ; // nothing
    #elif defined(RUN_HARDWARE_TESTS)
        hardware_tests_tick();
    #else
        Attract.tick();
    #endif
}






