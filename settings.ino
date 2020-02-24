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

#include "settings.h"


void CSettings::save_to_flash()
{
}

void CSettings::load_from_flash()
{
}

void CSettings::on_ball_served(char profile_no)
{
    balls_served += 1;
    if (profile_no >= 0) {
        player_stats[(byte)profile_no].balls_served += 1;
    }
}

void CSettings::on_score_achieved(BcdNum score, char profile_no)
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

