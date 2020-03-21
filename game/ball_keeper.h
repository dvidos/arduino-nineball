
class BallKeeperClass
{
public:
    byte shoot_agains: 2; // up to 3 shoot agains may be awarded
    byte ball_capture_enabled: 1; // the yellow arrow light

    void init();
    void enable_ball_capturing();
    void on_switch_closed(byte switch_no);
    bool has_captured_balls();
    bool are_all_balls_in_outhole();
    void grant_shoot_again();
    void consume_shoot_again();
    void eject_outhole_ball();       // get ball in shooting lane
    void release_captured_balls(); // eject all captured balls
    static void eject_outhole_timeout_expired();     // to see if successul
    static void release_captured_timeout_expired();  // to see if other balls still there
};


