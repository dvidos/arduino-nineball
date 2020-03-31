

class CGameplay
{
public:
    void start(byte mode);
    void handle_switch(byte switch_no);
    void every_100_msecs_interrupt();

    void add_score_bcd(dword bcd);
    void super_bonus_for_next_ball_achieved();
    void multipler_6x_achieved();
    void get_object_made();

private:
    word mode: 2;           // mode under which we are running
    word num_players: 3;    // number of players
    word current_player: 2; // current player no

    struct player_info
    {
        byte profile_number: 2; // 0=none, 1..3 = 1..3
        byte ball_number: 3;       // 1..5 shoot-agains don't increase this.
        byte achieved_6x_bonus_multiplier: 1; // to reset to 2x for next round
        byte achieved_super_bonus_for_next_ball: 1; // to light up the bonus.
        byte achieved_l1_highscore: 1; // whether 3-level high score achieved
        byte achieved_l2_highscore: 1; // whether 3-level high score achieved
        byte achieved_l3_highscore: 1; // whether 3-level high score achieved
        BcdNum score;
    } player_info[4];

    BcdNum temp_score;             // a place to add score that is slowly moved to player score and display.

    word running: 1;               // whether we are running (or merely existing in memory)
    word collecting_bonuses: 1;

    void prepare_game(byte player_no, byte ball_no);
    void make_current_target_object();

    void collect_bonus();
    inline byte get_object_bonus_lamp(byte object_no);
};

