
class CTimeout
{
public:
    CTimeout(byte ticks);
    
    byte ticks_remaining;
    
    void tick();
    bool expired();
}

CTimeout::CTimeout(byte ticks)
{
    ticks_remaining = ticks;
}

void CTimeout::tick()
{
    if (ticks_remaining > 0)
        ticks_remaining -= 1;
}

CTimeout::expired()
{
    return ticks_remaining == 0;
}