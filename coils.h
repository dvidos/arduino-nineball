

#include "pins.h"


class CCoils
{
public:
    // 5 bits allow value up to 32, for timeouts up to 320 msecs.
    byte top_bank_reset_timeout: 5;
    byte right_bank_reset_timeout: 5;
    byte left_bank_reset_timeout: 5;
    byte outhole_eject_timeout: 5;
    byte capture_lane_eject_timeout: 5;
    byte left_slingshot_timeout: 5;
    byte right_slingshot_timeout: 5;
    byte knocker_timeout: 5;
    
    CCoils();
    void fire_top_bank_reset();
    void fire_right_bank_reset();
    void fire_left_bank_reset();
    void fire_outhole_eject();
    void fire_capture_lane_eject();
    void fire_left_slingshot();
    void fire_right_slingshot();
    void fire_knocker();
    
    void init();
    
    void set_flippers_relay(bool on);
    
    void every_10_msecs_interrupt();
};

CCoils::CCoils()
{
    top_bank_reset_timeout = 0;
    right_bank_reset_timeout = 0;
    left_bank_reset_timeout = 0;
    outhole_eject_timeout = 0;
    capture_lane_eject_timeout = 0;
    left_slingshot_timeout = 0;
    right_slingshot_timeout = 0;
    knocker_timeout = 0;
    
    SET_OUTHOLE_EJECT_COIL(0);
    SET_LEFT_SLINGSHOT_COIL(0);
    SET_RIGHT_SLINGSHOT_COIL(0);
    SET_TOP_BANK_RESET_COIL(0);
    SET_RIGHT_BANK_RESET_COIL(0);
    SET_LEFT_BANK_RESET_COIL(0);
    SET_CAPTURE_LANE_EJECT_COIL(0);
    SET_KNOCKER_COIL(0);
    SET_FLIPPERS_RELAY(0);
}

void CCoils::init()
{
    SET_COILS_PINS_MODE();
}

void CCoils::fire_top_bank_reset()
{
    SET_TOP_BANK_RESET_COIL(1);
    
    // starting with 100 msecs, could go up to 200 msecs.
    top_bank_reset_timeout = 10;
}

void CCoils::fire_right_bank_reset()
{
    SET_RIGHT_BANK_RESET_COIL(1);
    right_bank_reset_timeout = 10;
}

void CCoils::fire_left_bank_reset()
{
    SET_LEFT_BANK_RESET_COIL(1);
    left_bank_reset_timeout = 10;
}

void CCoils::fire_outhole_eject()
{
    SET_OUTHOLE_EJECT_COIL(1);
    outhole_eject_timeout = 10;
}

void CCoils::fire_capture_lane_eject()
{
    SET_CAPTURE_LANE_EJECT_COIL(1);
    capture_lane_eject_timeout = 10;
}

void CCoils::fire_left_slingshot()
{
    SET_LEFT_SLINGSHOT_COIL(1);
    left_slingshot_timeout = 10;
}

void CCoils::fire_right_slingshot()
{
    SET_RIGHT_SLINGSHOT_COIL(1);
    right_slingshot_timeout = 10;
}

void CCoils::fire_knocker()
{
    SET_KNOCKER_COIL(1);
    knocker_timeout = 10;
}

void CCoils::set_flippers_relay(bool on)
{
    SET_FLIPPERS_RELAY(on ? 1 : 0);
}

void CCoils::every_10_msecs_interrupt()
{
    if (top_bank_reset_timeout > 0)
    {
        top_bank_reset_timeout -= 1;
        if (top_bank_reset_timeout == 0)
            SET_TOP_BANK_RESET_COIL(0);
    }
    
    if (right_bank_reset_timeout > 0)
    {
        right_bank_reset_timeout -= 1;
        if (right_bank_reset_timeout == 0)
            SET_RIGHT_BANK_RESET_COIL(0);
    }
    
    if (left_bank_reset_timeout > 0)
    {
        left_bank_reset_timeout -= 1;
        if (left_bank_reset_timeout == 0)
            SET_LEFT_BANK_RESET_COIL(0);
    }
    
    if (outhole_eject_timeout > 0)
    {
        outhole_eject_timeout -= 1;
        if (outhole_eject_timeout == 0)
            SET_OUTHOLE_EJECT_COIL(0);
    }
    
    if (capture_lane_eject_timeout > 0)
    {
        capture_lane_eject_timeout -= 1;
        if (capture_lane_eject_timeout == 0)
            SET_CAPTURE_LANE_EJECT_COIL(0);
    }
    
    if (left_slingshot_timeout > 0)
    {
        left_slingshot_timeout -= 1;
        if (left_slingshot_timeout == 0)
            SET_LEFT_SLINGSHOT_COIL(0);
    }
    
    if (right_slingshot_timeout > 0)
    {
        right_slingshot_timeout -= 1;
        if (right_slingshot_timeout == 0)
            SET_RIGHT_SLINGSHOT_COIL(0);
    }
        
    if (knocker_timeout > 0)
    {
        knocker_timeout -= 1;
        if (knocker_timeout == 0)
            SET_KNOCKER_COIL(0);
    }
}
