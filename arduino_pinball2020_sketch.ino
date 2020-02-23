/**
 * uncomment the line below to run the tests instead of the actual code. 
 * results on the serial console.
 */
#define RUN_TESTS

#include "audio.h"
#include "lamp_matrix.h"
#include "switch_matrix.h"

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;




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
