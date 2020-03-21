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

#ifdef RUN_SERIAL_EMULATOR
extern EmulatorClass Emulator;
#endif


class CSwitchMatrix
{
public:

    byte switches[5];
    byte current_row:3;

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
    bool get_first_closed_switch(byte *p_switch_no);
    bool get_next_switch_event(byte *p_switch_no, byte *p_was_pressed);
};





CSwitchMatrix::CSwitchMatrix()
{
    // we do not know the status of all switches,
    // and there's a very good chance that some will be closed (e.g. outhole)
    memset(switches, 0, sizeof(switches));
    current_row = 0;
    memset(events_queue, 0, sizeof(events_queue));
    events_queue_length = 0;
}

void CSwitchMatrix::init()
{
    SET_SWITCH_MATRIX_PINS_MODE();
    LOG("Switch Matrix initialized");
#ifdef RUN_SERIAL_EMULATOR
    Emulator.set_switches_buffers(switches);
#endif
}

bool CSwitchMatrix::is_switch_closed(byte switch_no)
{
    // switch / 8 is the byte offset
    // switch % 8 is the bit
    return (switches[switch_no >> 3] >> (switch_no & 0x7)) & 0x01;
}

bool CSwitchMatrix::get_first_closed_switch(byte *p_switch_no)
{
    // maybe we should disable interrupts here,
    // but this function is for diagnostics, so...
    for (byte row = 0; row < 5; row++)
    {
        if (switches[row] == 0)
            continue;

        for (byte offset = 0; offset <= 8; offset++)
        {
            if ((switches[row] >> offset) & 0x1)
                *p_switch_no = (row * 8) + offset;
                return true;
        }
    }

    return false;
}

void CSwitchMatrix::intercept_next_row()
{
    // called from an interrupt.
    // let's move and read the next row (0..4)
    current_row++;
    if (current_row > 4)
        current_row = 0;

    // move this to the pins
    SET_SWITCH_MATRIX_DEMUX_A(current_row >> 2);
    SET_SWITCH_MATRIX_DEMUX_B(current_row >> 1);
    SET_SWITCH_MATRIX_DEMUX_C(current_row >> 0);

    // give it a moment...
    NOP();
    NOP();
    NOP();

    // now read the 8 returns (hopefully a whole port, for speed)
    byte new_state = GET_SWITCH_MATRIX_RETURNS_OCTET();

    byte toggled = new_state ^ switches[current_row];
    if (toggled != 0)
    {
        // detect changes and put in a queue
        // hopefully, we'll find a way of debouncing
        // let's say that for now, we'll set this on the first toggle,
        // and hope that the 8 msecs it will take to come back to this row,
        // the bouncing will have stopped.

        // to add to the events queue:
        // disable interrupts (they should already be disabled, this is called by an ISR)
        // noInterrupts();

        for (byte offset = 0; offset <= 8; offset++)
        {
            if (((toggled >> offset) & 0x1) == 0)
                continue;

            byte switch_no = (current_row << 3) + offset; // (row * 8) + offset
            byte is_closed = (new_state >> offset) & 0x1;

            // we can detect queue overflow here
            if (events_queue_length >= SWITCH_MATRIX_EVENTS_QUEUE_SIZE) {
                FATAL_IN_ISR(3); // this will stop Arduino and will repeatedly flash the embedded LED
            }

            events_queue[events_queue_length].switch_no = switch_no;
            events_queue[events_queue_length].is_closed = is_closed;
            events_queue_length++;
        }
    }

    // now keep this to detect toggles on next time
    switches[current_row] = new_state;
}

bool CSwitchMatrix::get_next_switch_event(byte *p_switch_no, byte *p_is_closed)
{
#ifndef RUN_SERIAL_EMULATOR
    if (events_queue_length == 0)
        return false;

    // now, hopefully we won't run on race conditions, but...
    // we are not in an ISR, so turn off interrupts
    noInterrupts();

    // we will pop from the first item
    *p_switch_no = events_queue[0].switch_no;
    *p_is_closed = events_queue[0].is_closed;

    // shift everything 1 byte (the size of an event)
    memmove(&events_queue[0], &events_queue[1], SWITCH_MATRIX_EVENTS_QUEUE_SIZE - 1);

    // diminish queue size
    events_queue_length -= 1;

    // re-enable interrupts, we're safe
    interrupts();

    return true;
#else
    bool gotten = Emulator.get_next_switch_event(p_switch_no, p_is_closed);
    if (gotten) {
        // update our bitmaps in memory (it can be slow, we don't care here)
        if (*p_is_closed)
            switches[(*p_switch_no) >> 3] |= (1 << ((*p_switch_no) & 0x7));
        else
            switches[(*p_switch_no) >> 3] &= ~(1 << ((*p_switch_no) & 0x7));
    }
    return gotten;
#endif
}



