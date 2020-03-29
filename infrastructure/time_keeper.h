



class CTimeKeeper
{
public:
    CTimeKeeper();

    void init();
    void every_10_milliseconds_interrupt();
    void every_100_milliseconds_interrupt();
    void every_second_interrupt();

    void callback_later(func_ptr function, word milliseconds);

private:
    struct callback_info {
        byte ticks_left: 7;
        byte is_slow_ticks: 1;
        func_ptr function;
    } callbacks[TIME_KEEPER_CALLBACKS_SIZE];

    void update_timeouts(bool is_slow_tick);
};

CTimeKeeper::CTimeKeeper()
{
    memset(callbacks, 0, sizeof(callbacks));
}

void CTimeKeeper::every_10_milliseconds_interrupt()
{
    update_timeouts(false);
}

/**
 * We generate events every second and every half second.
 * To aleviate the need for listeners to toggle state,
 * we color our events as EVEN and ODD.
 */
void CTimeKeeper::every_100_milliseconds_interrupt()
{
}

void CTimeKeeper::every_second_interrupt()
{
    update_timeouts(true);
}

void CTimeKeeper::update_timeouts(bool is_slow_tick)
{
    for (byte i = 0; i < TIME_KEEPER_CALLBACKS_SIZE; i++)
    {
        // if zero, no need to diminish
        if (callbacks[i].ticks_left == 0)
            continue;

        // do we care about slow or fast ticks?
        if (callbacks[i].is_slow_ticks != (byte)is_slow_tick)
            continue;

        // diminish, see if done
        callbacks[i].ticks_left -= 1;
        if (callbacks[i].ticks_left == 0) {
            // it's important to do this last,
            // and not touch the ticks afterwards,
            // as the function may reset itself.
            (callbacks[i].function)();
        }
    }

}

void CTimeKeeper::callback_later(func_ptr function, word milliseconds)
{
    // find the first non used slot
    // or the slot that points to the same function.
    // it's important for a function to be able to prolong its timeout.
    byte slot = 255;
    for (byte i = 0; i < TIME_KEEPER_CALLBACKS_SIZE; i++) {
        if ((callbacks[i].function == function) ||
            (callbacks[i].ticks_left == 0)) {
            slot = i;
            break;
        }
    }
    if (slot == 255) {
        // we don't have enough slots to add it!
        FATAL(5);
    }

    callbacks[slot].function = function;
    if (milliseconds > 1270)
    {
        // maximum for WORD is 65K -> 65 seconds.
        // this allows 1 to 127 seconds.
        callbacks[slot].ticks_left = (milliseconds / 1000);
        callbacks[slot].is_slow_ticks = 1;
    }
    else
    {
        if (milliseconds < 10)
            milliseconds = 10;

        // this allows 10 milliseconds to 1.27 seconds.
        callbacks[slot].ticks_left = (milliseconds / 10);
        callbacks[slot].is_slow_ticks = 0;
    }
}
