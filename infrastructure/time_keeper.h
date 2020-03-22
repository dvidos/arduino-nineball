



class CTimeKeeper
{
public:
    CTimeKeeper();

    void init();
    void every_10_milliseconds_interrupt();
    void every_100_milliseconds_interrupt();
    void every_second_interrupt();

    void start_timeout(byte timeout_no, word milliseconds);
    bool get_next_expiration_event(byte *timeout_no);

    void callback_later(func_ptr function, word milliseconds);

private:
    struct timeout_timer {
        byte ticks_left: 7;    // this gives us from 1 to 120 ticks
        byte is_slow_ticks: 1; // 1=seconds, 0=10 msecs  (1 allows up to two minutes, 0 allows up to 1.2 secs)
    } timeouts[TIMEOUTS_COUNT];

    struct callback_info {
        byte ticks_left: 7;
        byte is_slow_ticks: 1;
        func_ptr function;
    } callbacks[TIME_KEEPER_CALLBACKS_SIZE];

    // make sure the items in this array are in byte boundaries,
    // our queueing operations depend on it.
    byte events_queue[TIME_KEEPER_EVENTS_QUEUE_SIZE];
    byte events_queue_length: 5; // 5 bits give us up to 32 items

    void add_to_timeout_events_queue(byte timeout_no);
    void update_timeouts(bool is_slow_tick);
};

CTimeKeeper::CTimeKeeper()
{
    memset(timeouts, 0, sizeof(timeouts));
    memset(callbacks, 0, sizeof(callbacks));
    memset(events_queue, 0, sizeof(events_queue));
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
    for (byte i = 0; i < TIMEOUTS_COUNT; i++)
    {
        // if zero, no need to diminish
        if (timeouts[i].ticks_left == 0)
            continue;

        // do we care about slow or fast ticks?
        if (timeouts[i].is_slow_ticks != (byte)is_slow_tick)
            continue;

        // diminish, see if expired
        timeouts[i].ticks_left -= 1;
        if (timeouts[i].ticks_left > 0)
            continue;

        add_to_timeout_events_queue(i);
    }

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

void CTimeKeeper::start_timeout(byte timeout_no, word milliseconds)
{
    if (timeout_no >= TIMEOUTS_COUNT) {
        LOG("bad timeout number: %d", timeout_no);
        return;
    }

    // 7 bits for ticks allow us values 0..127,
    // if these are seconds, we support up to 127000 milliseconds.
    // a WORD goes up to 65000, therefore 65 seconds (a little over one minute)

    if (milliseconds < 10)
        milliseconds = 10;

    if (milliseconds > 1270)
    {
        // this allows 1 to 127 seconds.
        timeouts[timeout_no].ticks_left = (milliseconds / 1000);
        timeouts[timeout_no].is_slow_ticks = 1;
    }
    else
    {
        // this allows 10 milliseconds to 1.27 seconds.
        timeouts[timeout_no].ticks_left = (milliseconds / 10);
        timeouts[timeout_no].is_slow_ticks = 0;
    }
}

bool CTimeKeeper::get_next_expiration_event(byte *timeout_no)
{
    if (events_queue_length == 0)
        return false;

    // now, hopefully we won't run on race conditions, but...
    // we are not in an ISR, so turn off interrupts
    noInterrupts();

    // we will pop from the first item
    *timeout_no = events_queue[0];

    // shift everything 1 byte (the size of an event)
    memmove(&events_queue[0], &events_queue[1], TIME_KEEPER_EVENTS_QUEUE_SIZE - 1);

    // diminish queue size
    events_queue_length -= 1;

    // re-enable interrupts, we're safe
    interrupts();

    return true;
}

void CTimeKeeper::add_to_timeout_events_queue(byte timeout_no)
{
   // we can detect queue overflow here
    if (events_queue_length >= TIME_KEEPER_EVENTS_QUEUE_SIZE) {
        FATAL_IN_ISR(4); // this will stop Arduino and will repeatedly flash the embedded LED
    }

    // add to event queue
    // as we are in an interrupt, interrupts should be disabled.
    events_queue[events_queue_length] = timeout_no;
    events_queue_length++;
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
