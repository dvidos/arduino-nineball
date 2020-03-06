#ifndef DEFINES_H
#define DEFINES_H


typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;



/**
 * Size of the queue that stores switch events in the interrupt handler
 * The idea is that at tick() time (the main loop) things will be fast enough
 * to consume this queue before it overflows.
 */
#define SWITCH_MATRIX_EVENTS_QUEUE_SIZE   16


/**
 * An instruction that does nothing, but burn a few electrons for a while.
 */
#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec


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
#endif


#endif