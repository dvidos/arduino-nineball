

#include "pins.h"


class CCoils
{
public:
    //  bits allow value up to 64, for timeouts up to 640 msecs.
    byte top_bank_reset_timeout: 6;
    byte right_bank_reset_timeout: 6;
    byte left_bank_reset_timeout: 6;
    byte outhole_eject_timeout: 6;
    byte capture_lane_eject_timeout: 6;
    byte left_slingshot_timeout: 6;
    byte right_slingshot_timeout: 6;
    byte knocker_timeout: 6;

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
    void fire_coil_by_number(byte coil_no);

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
    LOGM(M_COILS_INITIATED);
}

void CCoils::fire_top_bank_reset()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (top_bank_reset_timeout != 0)
        return;

    SET_TOP_BANK_RESET_COIL(1);
    top_bank_reset_timeout = 10; // each unit is 10 msecs
    LOGM(M_TOP_BANK_COIL_FIRED);
}

void CCoils::fire_right_bank_reset()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (right_bank_reset_timeout != 0)
        return;

    SET_RIGHT_BANK_RESET_COIL(1);
    right_bank_reset_timeout = 10; // each unit is 10 msecs
    LOGM(M_RIGHT_BANK_COIL_FIRED);
}

void CCoils::fire_left_bank_reset()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (left_bank_reset_timeout != 0)
        return;

    SET_LEFT_BANK_RESET_COIL(1);
    left_bank_reset_timeout = 10; // each unit is 10 msecs
    LOGM(M_LEFT_8_BANK_COIL_FIRED);
}

void CCoils::fire_outhole_eject()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (outhole_eject_timeout != 0)
        return;

    SET_OUTHOLE_EJECT_COIL(1);
    outhole_eject_timeout = 10; // each unit is 10 msecs
    LOGM(M_OUTHOLE_EJECT_COIL_FIRED);
}

void CCoils::fire_capture_lane_eject()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (capture_lane_eject_timeout != 0)
        return;

    SET_CAPTURE_LANE_EJECT_COIL(1);
    capture_lane_eject_timeout = 10; // each unit is 10 msecs
    LOGM(M_CAPTURE_LANE_EJECT_COIL_FIRED);
}

void CCoils::fire_left_slingshot()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (left_slingshot_timeout != 0)
        return;

    SET_LEFT_SLINGSHOT_COIL(1);
    left_slingshot_timeout = 10; // each unit is 10 msecs
    LOGM(M_LEFT_SLINGSHOT_COIL_FIRED);
}

void CCoils::fire_right_slingshot()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (right_slingshot_timeout != 0)
        return;

    SET_RIGHT_SLINGSHOT_COIL(1);
    right_slingshot_timeout = 10; // each unit is 10 msecs
    LOGM(M_RIGHT_SLINGSHOT_COIL_FIRED);
}

void CCoils::fire_knocker()
{
    // don't fire unless the coil is idle, avoid burning transistors
    if (knocker_timeout != 0)
        return;

    SET_KNOCKER_COIL(1);
    knocker_timeout = 10; // each unit is 10 msecs
    LOGM(M_KNOCKER_COIL_FIRED);
}

void CCoils::set_flippers_relay(bool on)
{
    SET_FLIPPERS_RELAY(on ? 1 : 0);
    LOGM(M_FLIPPERS_RELAY_SET_TO_D, on);
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

void CCoils::fire_coil_by_number(byte coil_no)
{
    switch (coil_no)
    {
        case COIL_KNOCKER:
            fire_knocker();
            break;
        case COIL_LEFT_SLINGSHOT:
            fire_left_slingshot();
            break;
        case COIL_RIGHT_SLINGSHOT:
            fire_right_slingshot();
            break;
        case COIL_TOP_BANK_RESET:
            fire_top_bank_reset();
            break;
        case COIL_RIGHT_BANK_RESET:
            fire_right_bank_reset();
            break;
        case COIL_LEFT_BANK_RESET:
            fire_left_bank_reset();
            break;
        case COIL_OUTHOLE_EJECT:
            fire_outhole_eject();
            break;
        case COIL_CAPTURE_LANE_EJECT:
            fire_capture_lane_eject();
            break;
    }
}
