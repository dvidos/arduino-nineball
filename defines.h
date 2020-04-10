#ifndef DEFINES_H
#define DEFINES_H


typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef void (*func_ptr)();


/**
 * Size of the queue that stores switch events in the interrupt handler
 * The idea is that at loop() time (the main loop) things will be fast enough
 * to consume this queue before it overflows.
 *
 * If overflown, generates FATAL(3)
 */
#define SWITCH_MATRIX_EVENTS_QUEUE_SIZE   16

/**
 * Size of the queue that stores timeout events in the interrupt handler
 * The idea is that at loop() time (the main loop) things will be fast enough
 * to consume this queue before it overflows.
 *
 * If overflown, generates FATAL(4)
 */
#define TIME_KEEPER_EVENTS_QUEUE_SIZE     8


/**
 * Size of the buffer that stores callback registrations in the TimeKeeper.
 * Increase if more call bask are needed (at one time).
 *
 * If overflown, generates FATAL(5)
 */
#define TIME_KEEPER_CALLBACKS_SIZE       16

/**
 * An instruction that does nothing, but burn a few electrons for a while.
 * Creates very small pauses (62 nsec,0.062 micro seconds, 0.000062 milliseconds!)
 * Useful for small delays when waiting for data lines to settle,
 * or to avoid race conditions with clock signals.
 */
#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec


/**
 * This was used for testing inside timer interrupts,
 * Where (i think) the Serial library would not work (because other interrupts are disabled)
 */
#define LED13_INIT()     pinMode(13,1)
#define LED13_TOGGLE()   digitalWrite(13, digitalRead(13) ^ 1)



/**
 * A sort of fatal exception handler or core dump.
 * Will freeze processing, but would allow the user to see what caused the fatal.
 *
 * We are using a global variable to store a fatal happening in an ISR
 * When the user code runs, outside of the interrupt, it will CHECK_FATAL()
 * and maybe it will flash then.
 *
 * Inside interrupt: call FATAL_IN_ISR(<num>)
 * In user mode    : call FATAL(<num>)
 *
 * Use wisely. Have fun too.
 */
byte __isr_fatal_number = 0;
#define FATAL_INIT()                      (__isr_fatal_number = 0)
#define FATAL_IN_ISR(flashes)             (__isr_fatal_number = flashes)
#define CHECK_FATAL_IN_ISR()              if (__isr_fatal_number) { FATAL(__isr_fatal_number); }
#define FATAL(flashes)                                                                    \
        LOG("*** FATAL %d ***", flashes);                                                 \
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
    #define LOG_INIT()         Serial.begin(115200/*9600*/); while (!Serial) { ; } Serial.println("Serial Log initialized");
    #define LOG(...)           log_info(__VA_ARGS__)
    #define LOGM(...)          log_info_from_progmem(__VA_ARGS__)
    #define LOG_DISPLAY(bytes1, bytes2)   log_display(bytes1, bytes2)

    char logging_buffer[64];
    char logging_message[64];

    void log_info(const char *fmt, ...) {
        va_list args_list;
        va_start(args_list, fmt);

        vsnprintf(logging_buffer, sizeof(logging_buffer), fmt, args_list);

        Serial.println(logging_buffer);
    }

    void log_info_from_progmem(byte message_no, ...) {
        va_list args_list;
        va_start(args_list, message_no);

        get_progmem_message(message_no, logging_message, sizeof(logging_message));

        vsnprintf(logging_buffer, sizeof(logging_buffer), logging_message, args_list);
        Serial.println(logging_buffer);
    }

    void log_display(byte *bytes1, byte *bytes2) {
        snprintf(logging_buffer, sizeof(logging_buffer), "Score Display is:  [%01X%02X%02X%02X]  [%01X%02X%02X%02X]",
            (bytes1[0] & 0xF),
            bytes1[1],
            bytes1[2],
            bytes1[3],
            (bytes2[0] & 0xF),
            bytes2[1],
            bytes2[2],
            bytes2[3]
        );

        for (byte i = 0; i < sizeof(logging_buffer); i++)
            if (logging_buffer[i] == 'F')
                logging_buffer[i] = '_';
        Serial.println(logging_buffer);
    }
#else
    #define LOG_INIT()           ((void)0)
    #define LOG(fmt, ...)        ((void)0)
    #define LOGM(msg_no, ...)    ((void)0)
    #define LOG_DISPLAY(b1, b2)  ((void)0)
#endif



#endif // DEFINES_H
