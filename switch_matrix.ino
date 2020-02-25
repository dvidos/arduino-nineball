
#include "switch_matrix.h"

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
    // say SET(PORTF, 3, current_send >> 2); 
    // say SET(PORTF, 7, current_send >> 1); 
    // say SET(PORTF, 0, current_send >> 0);
    
    // give it a moment...
    asm("nop \n nop \n nop \n");
    
    // now read the 8 bites (hopefully a whole port)
    byte current_returns = 0; // say PORTK
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
