#ifdef RUN_HARDWARE_TESTS

#define KEY_ESCAPE   1
#define KEY_ENTER    2
#define KEY_NEXT     3
#define KEY_PREV     4
#define KEY_F1       5
#define KEY_F2       6
#define KEY_F3       7
#define KEY_F4       8


/**
 * Simple applet to run various hardware tests.
 * 
 * Expected hardware:
 *   pins A0 to A7 = 8 buttons to earth
 *      button 0: escape
 *      button 1: left
 *      button 2: right
 *      button 3: enter
 *   pins A8 to A15 = 8 LEDs to earth, through a resistor 
 */
class CHardwareTests
{
public:
    byte switches_status;
    
    void init()
    {
        pinMode(A0, INPUT_PULLUP);
        pinMode(A1, INPUT_PULLUP);
        pinMode(A2, INPUT_PULLUP);
        pinMode(A3, INPUT_PULLUP);
        pinMode(A4, INPUT_PULLUP);
        pinMode(A5, INPUT_PULLUP);
        pinMode(A6, INPUT_PULLUP);
        pinMode(A7, INPUT_PULLUP);
        
        pinMode(A8, OUTPUT);
        pinMode(A9, OUTPUT);
        pinMode(A10, OUTPUT);
        pinMode(A11, OUTPUT);
        pinMode(A12, OUTPUT);
        pinMode(A13, OUTPUT);
        pinMode(A14, OUTPUT);
        pinMode(A15, OUTPUT);
        
        switches_status = PORTF;
        
        LOG("hardware tests initialized, press any key to select");
    }
    
    void tick()
    {
        // byte new_status = PORTF;
        byte new_status = 
                    digitalRead(A0) << 0 |
                    digitalRead(A1) << 1 | 
                    digitalRead(A2) << 2 | 
                    digitalRead(A3) << 3 | 
                    digitalRead(A4) << 4 | 
                    digitalRead(A5) << 5 | 
                    digitalRead(A6) << 6 | 
                    digitalRead(A7) << 7;
        
        if (new_status == switches_status)
            return;
        // LOG("old status: 0x%02X, new status: 0x%02X", switches_status, new_status);
        
        byte changes = (new_status ^ switches_status) & ~new_status;
        // LOG("key press detected: 0x%02X", changes);
        
        if (changes & 0x01) handle_key(KEY_ESCAPE);
        if (changes & 0x02) handle_key(KEY_PREV);
        if (changes & 0x04) handle_key(KEY_NEXT);
        if (changes & 0x08) handle_key(KEY_ENTER);
        if (changes & 0x10) handle_key(KEY_F1);
        if (changes & 0x20) handle_key(KEY_F2);
        if (changes & 0x40) handle_key(KEY_F3);
        if (changes & 0x80) handle_key(KEY_F4);
        
        switches_status = new_status;
    }
    
    void handle_key(byte key)
    {
        switch (key)
        {
            case KEY_ENTER:
                LOG("Enter");
                break;
            case KEY_PREV:
                LOG("Prev");
                break;
            case KEY_NEXT:
                LOG("Next");
                break;
            case KEY_ESCAPE:
                LOG("Escape");
                break;
            case KEY_F1:
            case KEY_F2:
            case KEY_F3:
            case KEY_F4:
                break;
        }
    }
};

CHardwareTests HardwareTests;

void hardware_tests_init()
{
    HardwareTests.init();
}

void hardware_tests_tick()
{
    HardwareTests.tick();
}



#endif  // RUN_HARDWARE_TESTS
