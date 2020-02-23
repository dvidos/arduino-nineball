/**
 * uncomment the line below to run the tests instead of the actual code. 
 * results on the serial console.
 */
#define RUN_TESTS

#include "types.h"
#include "constants.h"
#include "audio.h"
#include "lamp_matrix.h"
#include "switch_matrix.h"
#include "settings.h"

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CSettings Settings;





#define USE_SERIAL_FOR_LOG_NO
#ifdef USE_SERIAL_FOR_LOG
	#define INIT_LOG()    Serial.begin(9600); Serial.println("Serial Log initialized");
	#define LOG(message)  Serial.println(message);     
#else
	#define INIT_LOG()    ;
	#define LOG(message)  ;     
#endif



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
#ifndef RUN_TESTS

	loop_pins();
	delay(100);
#endif
}

void every_second() {
}

void every_half_second() {
}



// see https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
// and https://playground.arduino.cc/Code/AvailableMemory/

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

