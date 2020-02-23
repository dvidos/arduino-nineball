#ifndef LAMP_MATRIX_H
#define LAMP_MATRIX_H

#include "types.h"

class CLampMatrix
{
public:
    
    // 8 bytes of 8 bits => 64 lamps    
    byte lamps[8];
    byte current_send: 3; 
    
    CLampMatrix();
    
    void lamp_on(byte lamp_no);
    void lamp_off(byte lamp_no);
    bool is_on(byte lamp_no);
    
    void output_next_column();
};


#endif
