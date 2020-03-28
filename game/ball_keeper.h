/**
 * Balls passing in the left lane make current object.
 * Capturing all 3 balls starts multiball, no matter where we are
 * Making No 9 also starts multiball.
 * The top 2 switches supposedly make objects 6-8 only.
 *   But I think the possibility of captured ball is implied, though not written.
 */

class BallKeeperClass
{
public:
    void init();
    void on_switch_closed(byte switch_no);
    void enable_ball_capturing();
    void grant_shoot_again();
    void release_captured_balls(); // eject any captured balls
    void send_ball_to_shooting_lane();
    void is_ball_game_over();

private:
    byte ball_game_over: 1;             // whether player has drained with no alternatives
    byte shoot_agains: 2;          // up to 3 shoot agains may be awarded
    byte ball_capture_enabled: 1;  // the yellow arrow light
    byte releasing_balls: 1;       // to avoid false captures from settling balls
    byte serving_shooting_ball: 1; // to avoid false captures from settling balls
    byte eject_tries: 4;           // number of tries so far.

    byte count_captured_balls();
    byte count_drained_balls();

    static void check_ball_got_to_eject_lane();     // to see if successul
    static void check_all_captured_balls_released();  // to see if other balls still there
    static void turn_off_releasing_balls();
    static void turn_off_serving_shooting_ball();
};
