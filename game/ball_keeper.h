/**
 * Balls passing in the left lane make current object.
 * Capturing all 3 balls starts multiball, no matter where we are
 * Making No 9 also starts multiball.
 * The top 2 switches supposedly make objects 6-8 only.
 *   But I think the possibility of captured ball is implied, though not written.
 */


extern CAnimator Animator;

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



void BallKeeperClass::grant_shoot_again() {
    if (shoot_agains == 3)
        return;

    LOG("Shoot again granted");
    shoot_agains += 1;

    // bells and whistles
    Audio.play(SOUND_EXPLOSION);
    Animator.blink_a_little(LAMP_SHOOT_AGAIN, 1);
}

void BallKeeperClass::enable_ball_capturing() {
    ball_capture_enabled = true;
    LOG("Ball capturing enabled");
}

void BallKeeperClass::release_captured_balls() {
    LOG("Releasing captured balls");
}

