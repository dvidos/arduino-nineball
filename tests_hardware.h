#ifdef RUN_HARDWARE_TESTS

#define KEY_ESCAPE   0
#define KEY_ENTER    1
#define KEY_NEXT     2
#define KEY_PREV     3
#define KEY_F1       4
#define KEY_F2       5
#define KEY_F3       6
#define KEY_F4       7


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
        
        LOG("hardware tests initialized, press any key to select");
    }
    
    void tick()
    {
        byte k = get_key_down();
        handle_key(k);
    }
    
    byte get_key_down()
    {
        byte old_status = PINF;
        volatile byte new_status;
        while (1) {
            new_status = PINF; // it seems we have to use PINx to read a port. Maybe PORT IN?
            if (new_status == old_status)
                continue;
            
            // definite change, let's wait for debouncing, try again.
            delay(5);
            new_status = PINF;
            if (new_status == old_status)
                continue;
            
            // get only key down changes, remember we are pulling the signal LOW.
            byte changes = (new_status ^ old_status) & ~new_status;
            for (byte k = 0; k < 8; k++) {
                if (changes & (1 << k))
                    return k; 
            }
            
            // if we are here, it's a key up, which we don't care
            old_status = PINF;
        }
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
                LOG("F1");
                break;
            case KEY_F2:
                LOG("F2");
                break;
            case KEY_F3:
                LOG("F3");
                break;
            case KEY_F4:
                LOG("F4");
                break;
        }
    }
    
    void run_menu()
    {
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
