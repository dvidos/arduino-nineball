#ifndef SWITCH_MATRIX_H
#define SWITCH_MATRIX_H

#define SWITCH_MATRIX_EVENTS_QUEUE_SIZE   16

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



#endif
