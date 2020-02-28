
    
#define NOP()      __asm__("nop\n\t")  // every nop is one CPU cycle, 62.5 nsec


class CSwitchMatrix
{
public:
    
    byte switches[5];
    byte current_send:3;
   
    struct {
        // we have to find a way to understand valid events. switch 0 is a valid switch.
        byte switch_no:6; // we have up to 40 switches, 6 bits give us 64 switches
        byte is_closed:1; 
    } events_queue[SWITCH_MATRIX_EVENTS_QUEUE_SIZE];
    
    byte events_queue_length: 5; 
    
    CSwitchMatrix();
    
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
        // if (events_queue_length >= SWITCH_MATRIX_EVENTS_QUEUE_SIZE) { ... }
        
        events_queue[events_queue_length].switch_no = 12;
        events_queue[events_queue_length].is_closed = 1;
        events_queue_length++;  
    }
}

bool CSwitchMatrix::get_next_switch_event(byte *switch_no, byte *is_closed)
{
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
}

