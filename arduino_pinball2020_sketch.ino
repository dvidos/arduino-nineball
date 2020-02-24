/**
 * Created by Dimitris Vidos, for a custom Stern Nineball pinball machine.
 * All rights reserved.
 */
#define RUN_TESTS              // comment out to run normal code
#define LOG_ON_SERIAL_MONITOR  // comment out to stop sending log messages to the serial port (USB cable)

#include "types.h"
#include "constants.h"
#include "audio.h"
#include "lamp_matrix.h"
#include "switch_matrix.h"
#include "settings.h"

/**
 * All .ino and .pde files in the sketch folder (shown in the IDE as tabs with no extension) 
 * are concatenated together, starting with the file that matches the folder name followed 
 * by the others in alphabetical order, and the .cpp extension is added to the filename.
 *
 * See also: https://github.com/arduino/Arduino/wiki/Build-Process
 *
 * Another idea for organizing code:
 *  ArduinoWorkspace --- /myproj/ ----+ /src/ --------+ myproj.cpp
 *                                    + myproj.ino    + sensor.h
 *                                                    + sensor.cpp
 *                                                    + (more files)
 */

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CSettings Settings;


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



