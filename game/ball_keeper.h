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
    void drain_any_captured_balls();
    void on_switch_closed(byte switch_no);
    void enable_ball_capturing();
    void grant_shoot_again();
    void release_captured_balls(bool all_balls = true); // eject any captured balls
    void send_ball_to_shooting_lane();
    bool is_ball_game_over();
    bool all_balls_drained();
    byte count_captured_balls();
    byte count_drained_balls();

private:
    byte ball_game_over: 1;        // whether player has drained with no alternatives
    byte shoot_agains: 2;          // up to 3 shoot agains may be awarded
    byte ball_capture_enabled: 1;  // the yellow arrow light
    byte release_all_balls: 1;     // to know if we want to release all of them.
    byte ball_release_in_progress: 1;       // to avoid false captures from settling balls
    byte ball_serving_in_progress: 1; // to avoid false captures from settling balls
    byte eject_tries: 4;           // number of tries so far.

    void on_ball_captured();
    void on_ball_drained();

    static void check_ball_got_to_eject_lane();     // to see if successul
    static void check_release_captured_balls();  // to see if other balls still there
};
