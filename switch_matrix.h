/**
 * 5 sends with 8 returns. 5 sends using 3 pins and a 3-to-8 demux on the switch matrix.
 *
 * See also https://github.com/nickgammon/Keypad_Decoder/blob/master/Keypad_Decoder.ino
 * and      https://playground.arduino.cc/Main/KeypadTutorial/
 *
 * For debouncind switch matrix, read: https://summivox.wordpress.com/2016/06/03/keyboard-matrix-scanning-and-debouncing/
 * Essentially, keep state for more than one time in memory, deduct a switch has moved
 * when it has been in a stable state for long enough.
 * 2-5 msecs is usually the metal bouncing time, so maybe we only need one?
 *
 * Another technique for games is to switch state immediately with the first spike,
 * but avoid reading the switch for a small period of time, until it is stable.
 * Maybe our 5 msecs (time it takes to strobe all outputs) will be enough,
 * maybe we can give 2 msecs to each column, distributing switch reads every 10 msecs.
 */


class CSwitchMatrix
{
public:

    byte switches[5];
    byte current_send:3;

    // make sure this structure is on a byte boundary.
    // our queue operations depend on it.
    struct {
        // we have to find a way to understand valid events. switch 0 is a valid switch.
        byte switch_no:6; // we have up to 40 switches, 6 bits give us 64 switches
        byte is_closed:2;
    } events_queue[SWITCH_MATRIX_EVENTS_QUEUE_SIZE];

    byte events_queue_length: 5;

    CSwitchMatrix();
    void init();

    void intercept_next_row();
    bool is_switch_closed(byte switch_no);
    bool get_next_switch_event(byte *switch_no, byte *was_pressed);
};





CSwitchMatrix::CSwitchMatrix()
{
    // we do not know the status of all switches,
    // and there's a very good chance that some will be closed (e.g. outhole)
    memset(switches, 0, sizeof(switches));
    current_send = 0;
    memset(events_queue, 0, sizeof(events_queue));
    events_queue_length = 0;
}

void CSwitchMatrix::init()
{
    SET_SWITCH_MATRIX_PINS_MODE();
    LOG("Switch Matrix initialized");
}

bool CSwitchMatrix::is_switch_closed(byte switch_no)
{
    // switch / 8 is the byte offset
    // switch % 8 is the bit
    return (switches[switch_no >> 3] >> (switch_no & 0x7)) & 0x01;
}

void CSwitchMatrix::intercept_next_row()
{
    // called from an interrupt.
    // let's move to the next send (0..4)

    current_send++;
    if (current_send > 4)
        current_send = 0;

    // move this to the pins
    SET_SWITCH_MATRIX_DEMUX_A(current_send >> 2);
    SET_SWITCH_MATRIX_DEMUX_B(current_send >> 1);
    SET_SWITCH_MATRIX_DEMUX_C(current_send >> 0);

    // give it a moment...
    NOP();

    // now read the 8 returns (hopefully a whole port, for speed)
    byte current_returns = GET_SWITCH_MATRIX_RETURNS_OCTET();
    if (current_returns != switches[current_send])
    {
        // detect changes and put in a queue
        // hopefully, we'll find a way of debouncing (maybe 3 consec scans?)
        // if so, add new switch event, number and whether it was pressed or released.

        // to add to the events queue:
        // disable interrupts (they should already be disabled, this is called by an ISR)
        // noInterrupts();

        // we can detect queue overflow here
        if (events_queue_length >= SWITCH_MATRIX_EVENTS_QUEUE_SIZE) {
            FATAL(3); // this will stop Arduino and will repeatedly flash the embedded LED
        }

        events_queue[events_queue_length].switch_no = 12;
        events_queue[events_queue_length].is_closed = 1;
        events_queue_length++;
    }
}

bool CSwitchMatrix::get_next_switch_event(byte *switch_no, byte *is_closed)
{
#ifndef RUN_SERIAL_EMULATOR
    if (events_queue_length == 0)
        return false;

    // now, hopefully we won't run on race conditions, but...
    // we are not in an ISR, so turn off interrupts
    noInterrupts();

    // we will pop from the first item
    *switch_no = events_queue[0].switch_no;
    *is_closed = events_queue[0].is_closed;

    // shift everything 1 byte (the size of an event)
    memmove(&events_queue[0], &events_queue[1], SWITCH_MATRIX_EVENTS_QUEUE_SIZE - 1);

    // diminish queue size
    events_queue_length -= 1;

    // re-enable interrupts, we're safe
    interrupts();

    return true;
#else
    if (Serial.available() == 0)
        return false;

    byte received = Serial.read();
    // LOG("Serial received byte 0x%02X (%c)", received, received);
    *is_closed = true;
    switch (received)
    {
        case '1':
            *switch_no = SW_MENU_LEFT;
            LOG("SW_MENU_LEFT");
            return true;
        case '2':
            *switch_no = SW_MENU_RIGHT;
            LOG("SW_MENU_RIGHT");
            return true;
        case 's':
            *switch_no = SW_START;
            LOG("SW_START");
            return true;
    }

    return false;
#endif
}

