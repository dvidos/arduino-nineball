#ifndef DEFINES_H
#define DEFINES_H


typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;



/**
 * Size of the queue that stores switch events in the interrupt handler
 * The idea is that at loop() time (the main loop) things will be fast enough
 * to consume this queue before it overflows.
 */
#define SWITCH_MATRIX_EVENTS_QUEUE_SIZE   16

/**
 * Size of the queue that stores timeout events in the interrupt handler
 * The idea is that at loop() time (the main loop) things will be fast enough
 * to consume this queue before it overflows.
 */
#define TIME_KEEPER_EVENTS_QUEUE_SIZE     8


/**
 * An instruction that does nothing, but burn a few electrons for a while.
 */
#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec


/**
 * A sort of exception handler or core dump.
 * Will freeze processing, but would allow the user to see what caused the fatal.
 * Use wisely.
 */
#define FATAL(flashes)                                                                    \
        pinMode(13, OUTPUT);                                                              \
        while (1) {                                                                       \
            for (byte fatal_flashes = 0; fatal_flashes < (flashes); fatal_flashes++) {    \
                digitalWrite(13, HIGH); delay(175);                                       \
                digitalWrite(13, LOW);  delay(175);                                       \
            }                                                                             \
            delay(1000);                                                                  \
        }


/**
 * A useful macro for tests.
 * We can map it out to nothing if NDEBUG is defined later on...
 */
#if defined(NDEBUG)
    #define ASSERT(condition)   (void)0
#else
    #define ASSERT(condition)    if(!(condition)) { LOG("** Failed asserting that \"%s\"", #condition); }
#endif


/**
 * A central way to have LOG() calls in code, without affecting performance in the end.
 */
#ifdef LOG_ON_SERIAL_MONITOR
    #define LOG_INIT()      Serial.begin(57600/*9600*/); while (!Serial) { ; } Serial.println("Serial Log initialized");      
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
#endif


#endif