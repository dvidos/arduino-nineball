

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
    LOG("Coils initialized");
}

void CCoils::fire_top_bank_reset()
{
    // don't fire unless the coil is idle
    if (top_bank_reset_timeout != 0)
        return;

    SET_TOP_BANK_RESET_COIL(1);
    top_bank_reset_timeout = 10;
    LOG("Top Bank coil fired");
}

void CCoils::fire_right_bank_reset()
{
    // don't fire unless the coil is idle
    if (right_bank_reset_timeout != 0)
        return;

    SET_RIGHT_BANK_RESET_COIL(1);
    right_bank_reset_timeout = 10;
    LOG("Right Bank coil fired");
}

void CCoils::fire_left_bank_reset()
{
    // don't fire unless the coil is idle
    if (left_bank_reset_timeout != 0)
        return;

    SET_LEFT_BANK_RESET_COIL(1);
    left_bank_reset_timeout = 10;
    LOG("Left (8) Bank coil fired");
}

void CCoils::fire_outhole_eject()
{
    // don't fire unless the coil is idle
    if (outhole_eject_timeout != 0)
        return;

    SET_OUTHOLE_EJECT_COIL(1);
    outhole_eject_timeout = 10;
    LOG("Outhole Eject coil fired");
}

void CCoils::fire_capture_lane_eject()
{
    // don't fire unless the coil is idle
    if (capture_lane_eject_timeout != 0)
        return;

    SET_CAPTURE_LANE_EJECT_COIL(1);
    capture_lane_eject_timeout = 10;
    LOG("Capture Lane Eject coil fired");
}

void CCoils::fire_left_slingshot()
{
    // don't fire unless the coil is idle
    if (left_slingshot_timeout != 0)
        return;

    SET_LEFT_SLINGSHOT_COIL(1);
    left_slingshot_timeout = 10;
    LOG("Left Slingshot coil fired");
}

void CCoils::fire_right_slingshot()
{
    // don't fire unless the coil is idle
    if (right_slingshot_timeout != 0)
        return;

    SET_RIGHT_SLINGSHOT_COIL(1);
    right_slingshot_timeout = 10;
    LOG("Right Slingshot coil fired");
}

void CCoils::fire_knocker()
{
    // don't fire unless the coil is idle
    if (knocker_timeout != 0)
        return;

    SET_KNOCKER_COIL(1);
    knocker_timeout = 10;
    LOG("Knocker coil fired");
}

void CCoils::set_flippers_relay(bool on)
{
    SET_FLIPPERS_RELAY(on ? 1 : 0);
    LOG("Flippers relay set to %d", on);
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
