#ifndef DEFINES_H
#define DEFINES_H


typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef enum {
    switch_closed,
    switch_opened,
    timeout_expired,
} event_type;

typedef void (*func_ptr)();

byte isr_fatal_number = 0;


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


#define TIME_KEEPER_CALLBACKS_SIZE       16

/**
 * An instruction that does nothing, but burn a few electrons for a while.
 */
#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec


#define LED13_INIT()     pinMode(13,1)
#define LED13_TOGGLE()   digitalWrite(13, digitalRead(13) ^ 1)



/**
 * A sort of exception handler or core dump.
 * Will freeze processing, but would allow the user to see what caused the fatal.
 *
 * Using a global variable to store a fatal happening in an ISR
 * If/When the user code will run, outside the interrupt, it will CHECK
 * and maybe it will flash then.
 *
 * Inside interrupt: call FATAL_IN_ISR(<num>)
 * In user mode    : call FATAL(<num>)
 *
 * Use wisely. Have fun too.
 */
#define FATAL_INIT()                      (isr_fatal_number = 0)
#define FATAL_IN_ISR(flashes)             (isr_fatal_number = flashes)
#define CHECK_FATAL_IN_ISR()              if (isr_fatal_number) { FATAL(isr_fatal_number); }
#define FATAL(flashes)                                                                    \
        LOG("*** FATAL %d ***", flashes);                                                  \
        pinMode(13, OUTPUT);                                                              \
        while (1) {                                                                       \
            for (byte fatal_flashes = 0; fatal_flashes < (flashes); fatal_flashes++) {    \
                digitalWrite(13, HIGH); delay(150);                                       \
                digitalWrite(13, LOW);  delay(150);                                       \
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

    #define LOG_DISPLAY(bytes1, bytes2)   log_display(bytes1, bytes2)
    void log_display(byte *bytes1, byte *bytes2) {
        // show what the display would show on the Serial port.
        char buffer[64];

        sprintf(buffer, "Score Display is:  [%01X%02X%02X%02X]  [%01X%02X%02X%02X]",
            (bytes1[0] & 0xF),
            bytes1[1],
            bytes1[2],
            bytes1[3],
            (bytes2[0] & 0xF),
            bytes2[1],
            bytes2[2],
            bytes2[3]
        );

        for (byte i = 0; i < 40; i++)
            if (buffer[i] == 'F')
                buffer[i] = '_';
        Serial.println(buffer);
    }
#else
    #define LOG_INIT()           (void)0
    #define LOG(fmt, ...)        (void)0
    #define LOG_DISPLAY(b1, b2)  (void)0
#endif



#endif // DEFINES_H
