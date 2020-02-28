/*
    original game had options like:
    - three high score thresholds
    - current high game threshold
    - current credits
    - total plays
    - total replays
    - total times highscore passed
    - number of coins throught chute 1, 2, 3
    - total balls played
    - no of top bank wow
    - no of 8 bank wow
    - total playfield special awards
    - top level 1, 2, 3 passed
    it seems these where to help the owner calibrate the game difficulty
    against the players.
    
    also, there is a matrix for awards on the manual, based on settings no 5, 22, 23, 24, 32.
    we have attempted a more simplified and straightforward way.
    
    there is a distinction between "Shoot Again" and "Replay".
    
    the "shoot again" happens immediately when the ball is drained, the same ball number is kept.
    bonus is applied to the score,  bonus and multiplier are reset, and the ball is kicked into the shooting lane.

    I think that free game means one credit (i.e. as if you had paid for it)
    this seems to be of no use to our case, as we won't be accepting any money anyway.    
      
*/



class CGameSettings
{
public:

    BcdNum highest_score;  // highest score to date
    word balls_served;
    
    struct {
        BcdNum highest_score;
        word balls_served;
    } player_stats[3];     // statistics per specific user profile (0..3)

    // every time we pass the high score, an extra ball (up to 3) is given. extra play is meaningless without money.    
    BcdNum awards_threshold[3];   // some awards threshold, extra ball or free game. incr. per 10K, default: 620K, 1200K, 0
    
    // original nineball switch no in brackets
    
    // int high_score_award: 1;          // [ 6] 0=x-ball, 1=replay (no point replay, as we don't accept coins) we
    
    int five_balls_per_game: 1;       // [ 7] 0=three balls, 1=five balls
    int background_sounds: 1;         // [ 8] 0/1
    int background_music: 1;          // [ -] 0/1
    int spot_light_strategy: 1;       // [13] 0=conservative, 1=liberal (amount of time slingshots, spinner and pop bumpers keep the spot lights on)
    int multiplier_step_up: 1;        // [14] 0=both 3-banks, 1=one 3 bank
    int high_score_feature: 2;        // [15-16]  ???
    int spinner_value_advancement: 1; // [17] 0=center target, 1=any outside target
    int match_features: 1;            // [21] ???
    int eight_bank_wow_turn_on: 1;    // [29] 0=on making no 9, 1=on making no 8.
    int when_super_bonus_lights: 1;   // [30] 0=on making no 9, 1=on making no 8.
    int three_bank_wow_turn_on: 1;    // [31] 0=7x multiplier, 1=6x multiplier.
    
    // my proposal for swithces 5, 22, 23, 24, 32
    // int add_a_ball: 1;                // [ 5]  0/1 for adding an extra ball.. ???
    // int awards_types: 3;              // [22-24] multiple options, we'll have to make something decent.
                                      // essentially, what WOW and SPECIAL do.
                                       
    // up to 3 (or 5) "add a ball" (shoot again?) can happen.
    int wow_award_type: 1;            // 0=70K, 1=Shoot Again.  (maybe also add 40K option?)
    int special_award_type: 2;        // 0=90K, 1=130K, 2=Shoot Again, 3=??? (was replay)
    int unlimited_specials: 1;        // [32] 0=one per ball, 1=many per ball    
        
    void save_to_flash();
    void load_from_flash();
    
    void on_ball_served(char profile_no = -1);
    void on_score_achieved(BcdNum score, char profile_no = -1);
};





void CGameSettings::save_to_flash()
{
}

void CGameSettings::load_from_flash()
{
}

void CGameSettings::on_ball_served(char profile_no)
{
    balls_served += 1;
    if (profile_no >= 0) {
        player_stats[(byte)profile_no].balls_served += 1;
    }
}

void CGameSettings::on_score_achieved(BcdNum score, char profile_no)
{
    if (score > highest_score) {
        highest_score.copy_from(score);
    }
    if (profile_no >= 0) {
        if (score > player_stats[(byte)profile_no].highest_score) {
            player_stats[(byte)profile_no].highest_score.copy_from(score);
        }
    }
}

