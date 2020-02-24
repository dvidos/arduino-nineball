/**
 * uncomment the line below to run the tests instead of the actual code. 
 * results on the serial console.
 */
#define RUN_TESTS              // comment out to run normal code
#define LOG_ON_SERIAL_MONITOR  // comment out to stop sending log messages to the serial port (USB cable)

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



