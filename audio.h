
class CAudio
{
public:
    
    CAudio();
    
    void init();
    void play(int sound_no);
    void stop_all();
    
protected:
    wavTrigger trigger;
};



CAudio::CAudio()
{
    trigger.start();
}

void CAudio::init()
{
}

void CAudio::play(int sound_no)
{
    LOG("Audio %d launched");
    trigger.trackPlayPoly(sound_no);
}

void CAudio::stop_all()
{
    LOG("All audio sounds stopped");
    trigger.stopAllTracks();
}