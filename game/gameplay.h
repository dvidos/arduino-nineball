

class CGameplay
{
public:
    CGameplay();
    void start(byte mode);
    void handle_event(Event& e);
    void every_100_msecs_interrupt();

    void add_score_bcd(dword bcd);
    void add_shoot_again();

private:
    word mode: 2;                  // mode under which we are running
    word highest_player: 2;        // highest player number (0..3) (essentially num_players - 1)
    word current_player: 2;        // current player no

    struct player_info
    {
        word ball_number: 3;       // 1..5 shoot-agains don't increase this.
        BcdNum score;
    } player_info[4];
    BcdNum temp_score;             // a place to add score that is slowly moved to player score and display.

    word running: 1;               // whether we are running (or merely existing in memory)
    word collecting_bonuses: 1;

    void handle_switch_closed(char switch_no);

    void prepare_game(byte player_no, byte ball_no);
    void make_current_target_object();
};

