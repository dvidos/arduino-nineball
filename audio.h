
class CAudio
{
public:

    CAudio();

    void init();
    void play(int sound_no);
    bool is_playing(byte sound_no);
    void stop_all();

protected:
    wavTrigger trigger;
};



CAudio::CAudio()
{
}

void CAudio::init()
{
    trigger.start();

    // get version, this requires bidirectional connect
    // and ensures update() can send back data
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    bool response = trigger.getVersion(buffer, sizeof(buffer) - 1);
    if (response) {
        LOG("WavTrigger version %s detected");
    } else {
        LOG("Could not retrieve wavTrigger version");
    }

    byte num_tracks = trigger.getNumTracks();
    LOG("WavTrigger reports %d tracks on SD card", num_tracks);

    // enable the board to report starts and finishes of tracks
    // this allows our radio mode to detect when a track has finished
    // for this, one needs to call "update()" periodically
    trigger.setReporting(true);

    LOG("Audio initialized");
}

void CAudio::play(int sound_no)
{
    LOG("Audio %d launched", sound_no);
    trigger.trackPlayPoly(sound_no);
}

bool CAudio::is_playing(byte sound_no)
{
    // update is to consume and messages sent by the sound board
    // back to Arduino.
    // it has to be called periodically.
    // it is assumed that "is_playing" is called periodically
    trigger.update();

    return trigger.isTrackPlaying(sound_no);
}

void CAudio::stop_all()
{
    LOG("All audio sounds stopped");
    trigger.stopAllTracks();
}

