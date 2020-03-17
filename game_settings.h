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
    maybe it means you can start again a new 3-ball game?

    the "shoot again" happens immediately when the ball is drained, the same ball number is kept.
    bonus is applied to the score,  bonus and multiplier are reset, and the ball is kicked into the shooting lane.

    I think that free game means one credit (i.e. as if you had paid for it)
    this seems to be of no use to our case, as we won't be accepting any money anyway.
*/



class CGameSettings
{
public:
    union {
        struct {
            // 4 bytes dword x 7 stats = 28 bytes
            dword _highest_score_to_date_decimal;
            dword _personal_high_score_decimal[3]; // 0=dad, 1=mom, 2=nick
            // every time we pass the high score, an extra ball (up to 3) is given. extra play is meaningless without money.
            dword _awards_threshold_decimal[3];   // some awards threshold, extra ball or free game. incr. per 10K, default: 620K, 1200K, 0

            // 2 bytes per word.
            word balls_served;
            word games_played;

            // original nineball switch no in brackets
            word background_sounds: 1;         // [ 8] 0/1
            word background_music: 1;          // [ -] 0/1
            word five_balls_per_game: 1;       // [ 7] 0=three balls, 1=five balls
            word spot_light_strategy: 1;       // [13] 0=conservative, 1=liberal (amount of time slingshots, spinner and pop bumpers keep the spot lights on)
            word multiplier_step_up: 1;        // [14] 0=both 3-banks, 1=one 3 bank
            word spinner_value_advancement: 1; // [17] 0=center target, 1=any outside target
            word eight_bank_wow_turn_on: 1;    // [29] 0=on making no 9, 1=on making no 8.
            word when_super_bonus_lights: 1;   // [30] 0=on making no 9, 1=on making no 8.
            word three_bank_wow_turn_on: 1;    // [31] 0=7x multiplier, 1=6x multiplier.
            word wow_award_type: 1;            // 0=70K, 1=Shoot Again.
            word special_award_type: 2;        // 0=90K, 1=130K, 2=Shoot Again, 3=??? (was replay)
            word unlimited_specials: 1;        // [32] 0=one per ball, 1=many per ball
        };
        byte buffer[64];
    };

    // bcdnums to allow user space to manipulate them,
    BcdNum highest_score_to_date;
    BcdNum personal_high_score[3];
    BcdNum awards_threshold[3];

    CGameSettings();
    void save_to_eeprom();
    void load_from_eeprom();
};


CGameSettings::CGameSettings()
{
    memset(buffer, 0, sizeof(buffer));
}

void CGameSettings::save_to_eeprom()
{
    // convert from BcdNum objects do dword in union
    _highest_score_to_date_decimal = highest_score_to_date.to_decimal();
    _personal_high_score_decimal[0] = personal_high_score[0].to_decimal();
    _personal_high_score_decimal[1] = personal_high_score[1].to_decimal();
    _personal_high_score_decimal[2] = personal_high_score[2].to_decimal();
    _awards_threshold_decimal[0] = awards_threshold[0].to_decimal();
    _awards_threshold_decimal[1] = awards_threshold[1].to_decimal();
    _awards_threshold_decimal[2] = awards_threshold[2].to_decimal();

    // now save the buffer.
    LOG("Saving settings to EEPROM");
    for (byte i = 0; i < sizeof(buffer); i++) {
        EEPROM.update(i, buffer[i]);
    }
}

void CGameSettings::load_from_eeprom()
{
    LOG("Loading settings from EEPROM");
    for (byte i = 0; i < sizeof(buffer); i++) {
        buffer[i] = EEPROM.read(i);
    }

    // from dword to BcdNum, to allow easy manipulation and display.
    highest_score_to_date.from_decimal(_highest_score_to_date_decimal);
    personal_high_score[0].from_decimal(_personal_high_score_decimal[0]);
    personal_high_score[1].from_decimal(_personal_high_score_decimal[1]);
    personal_high_score[2].from_decimal(_personal_high_score_decimal[2]);
    awards_threshold[0].from_decimal(_awards_threshold_decimal[0]);
    awards_threshold[1].from_decimal(_awards_threshold_decimal[1]);
    awards_threshold[2].from_decimal(_awards_threshold_decimal[2]);
}
