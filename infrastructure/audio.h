
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
        LOGM(M_WAVTRIGGER_VERSION_D_DETECTED, buffer);
    } else {
        LOGM(M_COULD_NOT_RETRIEVE_WAVTRIGGER_VERSION);
    }

    byte num_tracks = trigger.getNumTracks();
    LOGM(M_WAVTRIGGER_REPORTS_D_TRACKS_ON_CARD, num_tracks);

    // enable the board to report starts and finishes of tracks
    // this allows our radio mode to detect when a track has finished
    // for this, one needs to call "update()" periodically
    trigger.setReporting(true);

    LOGM(M_AUDIO_INITIALIZED);
}

void CAudio::play(int sound_no)
{
    trigger.trackPlayPoly(sound_no);
    LOGM(M_AUDIO_TRACK_D_LAUNCHED, sound_no);
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
    trigger.stopAllTracks();
    LOGM(M_ALL_AUDIO_TRACKS_STOPPED);
}

