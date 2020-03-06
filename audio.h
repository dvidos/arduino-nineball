
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
    trigger.trackPlayPoly(sound_no);
}

void CAudio::stop_all()
{
    trigger.stopAllTracks();
}