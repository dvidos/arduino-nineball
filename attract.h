

class CAttract
{
public:
    byte mode: 3;
    byte menu_item: 4;
    byte item_value;
    
//    void handle_switch(byte switch_no)
    void start();
    void handle_event(Event& e);
    
};

void CAttract::start()
{
}

void CAttract::handle_event(Event& e)
{
}

/*
    Using left button underneath, we cycle the modes.
    Using the START button we enter a mode

    0123456 8901234
    7777777 7777777
    M  II   VVVVVVV
    |   |      +----- value
    |   +------------ menu item
    +---------------- mode
    
    In attract mode, pressing the left P1 button should display the mode on the first left digit.
    It should timeout after 5 seconds, in the sense that it should return to displaying the high score
    Every 5 minutes or so, it could play a song.
    
    Now, when we enter a menu (using start) we can navigate and change settings,
    On the left is the menu (diags, settings etc), then the menu item, on the right display is the value.
    P1 changes menu item, P2 changes value.
    Maybe an extra menu item can be inserted for save and exit.
    
    We are supposed to launch a normal game and a happy game from here.
    So we should be able to setup Gameplay.setup(flags).
    
    
*/
    

#define ATTRACT_IDLE                0   // Start: start game, P2: nothing
#define ATTRACT_EVA_HAPPY_MODE      1   // Start: start game, infinate balls.
#define ATTRACT_JUKE_BOX            2   // Start: start jukebox?   P2: next random song?
#define ATTRACT_SETTINGS            3   // Start: get into settings. P1: cycle settings, P2: cycle value for each setting
#define ATTRACT_DIAGNOSTICS         4   // Start: into diagnostics. P1: cycle program, P2: cycle variation 


#define SETTINGS_HIGH_SCORE_THRESHOLD_1   1   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_THRESHOLD_2   2   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_THRESHOLD_3   3   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_TO_DATE       4   // P2: Press & release 3 times to reset
#define SETTINGS_GAMES_PLAYED             5   // P2: Press & release 3 times to reset
#define SETTINGS_BALLS_SERVED             6   // P2: Press & release 3 times to reset
#define SETTINGS_BACKGROUND_SOUNDS        7   // P2: 1/0 (on/off) 
#define SETTINGS_BACKGROUND_MUSIC         8   // P2: 1/0 (on/off)
#define SETTINGS_BALLS_PER_GAME           9   // P2: 3/5
#define SETTINGS_SPOT_LIGHT_STRATEGY     10   // P2: 0=conservative / 1=liberal
#define SETTINGS_MULTIPLIER_STEP_UP      11   // P2: 0=both 3 banks / 1=one 3 bank
#define SETTINGS_SPINNER_ADVANCES        12   // P2: 0=center target / 1=any outside target
#define SETTINGS_EIGHT_BANK_WOW_STARTS   13   // P2: 0=on making 9 / 1=on making 8
#define SETTINGS_SUPER_BONUS_LIGHTS      14   // P2: 0=on making 9 / 1=on making 8
#define SETTINGS_THREE_BANK_WOW_STARTS   15   // P2: 0=on 7x multiplier / 1=on 6x and 7x multiplier achieved 
#define SETTINGS_WOW_AWARD_TYPE          16   // P2: 0=70K / 1=shoot again
#define SETTINGS_SPECIAL_AWARD_TYPE      17   // P2: 0=90K / 1=130K / 2=shoot again / 3=???
#define SETTINGS_UNLIMITED_SPECIALS      18   // P2: 0=one per ball, 1=unlimited


#define DIAGNOSTICS_LAMP_MATRIX_COLUMNS          0 // all lamps in one row. after 1 sec, P2=next row
#define DIAGNOSTICS_LAMP_MATRIX_ROWS             1 // all lamps in one col. after 1 sec, P2=next col
#define DIAGNOSTICS_LAMP_MATRIX_ALL              2 // all lamps blink on-off every second   
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_NUMBER  3 // display num 0 in all displays.     P2=2,3,4 etc
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_DIGIT   4 // display num 8 in one digit         P2=next digit etc
#define DIAGNOSTICS_SWITCH_MATRIX                5 // display num of switch closed.
#define DIAGNOSTICS_COILS                        6 // fire a coil every second.          P2=start & next coil
#define DIAGNOSTICS_Q_RELAY                      7 // turn Q relay on off                P2=toggle
#define DIAGNOSTICS_SOUND                        8 // play sound                         P2=next sound



/*
void CAttract::handle_switch(byte switch_no)
{
   switch (switch_no)
   {
       case SW_START:
           if (menu_item == ATTRACT_IDLE) {
               // start normal game
           } else if (menu_item == ATTRACT_EVA_HAPPY_MODE) {
               // start eva happy mode game
           } else if (menu_item == ATTRACT_JUKE_BOX) {
               // start jukebox
           } else if (menu_item == ATTRACT_SETTINGS) {
               // enter settings
               mode = ATTRACT_SETTINGS;
               menu_item = 0;
           } else if (menu_item == ATTRACT_DIAGNOSTICS) {
               // enter diagnostics
               mode = ATTRACT_DIAGNOSTICS;
               menu_item = 0;
           }
           break;
           
       case SW_MENU_LEFT:
           
           if (mode == ATTRACT_SETTINGS) {
               settings_next_item();
           } else if (mode == ATTRACT_DIAGNOSTICS) {
               diagnostics_next_item();
           }
           break;
           
       case SW_MENU_RIGHT:
           if (mode == ATTRACT_SETTINGS) {
               settings_next_value();
           } else if (mode == ATTRACT_DIAGNOSTICS) {
               diagnostics_next_value();
           }
           break;
   }
}


{
    if (mode == ATTRACT_IDLE) {
        // display high score
        if (menu_item == 0)
            // show high score as default
            ScoreDisplay.display_bcd_num(0, GameSettings.highest_score);
        else 
            // show menu item, as there is one selected.
            ScoreDisplay.show_digit(0, menu_item);
            
        // the right display shows high score to date
        ScoreDisplay.display_bcd_num(1, GameSettings.highest_score);
        
    } else if (mode == ATTRACT_DIAGNOSTICS) {
        ScoreDisplay.show_digit(0, mode);
        ScoreDisplay.show_digit(3, (menu_item / 10));
        ScoreDisplay.show_digit(4, (menu_item % 10));
        
        switch (menu_item)
        {
            case DIAGNOSTICS_LAMP_MATRIX_COLUMNS:           // all lamps in one row. after 1 sec, P2=next row
                ScoreDisplay.show_digit(15, item_value);
                break;
            case DIAGNOSTICS_LAMP_MATRIX_ROWS:              // all lamps in one col. after 1 sec, P2=next col
                ScoreDisplay.show_digit(15, item_value);
                break;
            case DIAGNOSTICS_LAMP_MATRIX_ALL:               // all lamps blink on-off every second
                break;   
            case DIAGNOSTICS_SCORE_DISPLAY_SINGLE_NUMBER:   // display num 0 in all displays.     P2=2,3,4 etc
                break;
            case DIAGNOSTICS_SCORE_DISPLAY_SINGLE_DIGIT:    // display num 8 in one digit         P2=next digit etc
                break;
            case DIAGNOSTICS_SWITCH_MATRIX:                 // display num of switch closed.
                break;
            case DIAGNOSTICS_COILS:                         // fire a coil every second.          P2=start & next coil
                ScoreDisplay.show_digit(15, item_value);
                break;
            case DIAGNOSTICS_Q_RELAY:                       // turn Q relay on off                P2=toggle
                ScoreDisplay.show_digit(15, item_value);
                break;
            case DIAGNOSTICS_SOUND:                         // play sound                         P2=next sound
                ScoreDisplay.show_digit(15, item_value);
                break;
        }
    }
}

void CAttract::settings_next_item()
{
    menu_item++;
    if (menu_item > SETTINGS_UNLIMITED_SPECIALS)
        menu_item = 0;
    
    items_value = 0;
    
    ScoreDisplay.hide_all();        
    ScoreDisplay.show_digit(0, mode);
    ScoreDisplay.show_digit(3, (menu_item / 10));
    ScoreDisplay.show_digit(4, (menu_item % 10));
    
    switch (menu_item) {
        case SETTINGS_HIGH_SCORE_THRESHOLD_1:   // P2: increase 10k (hold down must keep increasing) zero is allowed
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[0]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_2:   // P2: increase 10k (hold down must keep increasing) zero is allowed
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[1]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_3:   // P2: increase 10k (hold down must keep increasing) zero is allowed
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[2]);
            break;
        case SETTINGS_HIGH_SCORE_TO_DATE:       // P2: Press & release 3 times to reset
            ScoreDisplay.show_bcd_num(1, GameSettings.highest_score);
            break;
        case SETTINGS_GAMES_PLAYED:             // P2: Press & release 3 times to reset
            ScoreDisplay.show_bcd_num(1, GameSettings.games_played);
            break;
        case SETTINGS_BALLS_SERVED:             // P2: Press & release 3 times to reset
            ScoreDisplay.show_bcd_num(1, GameSettings.balls_served);
            break;
        case SETTINGS_BACKGROUND_SOUNDS:        // P2: 1/0 (on/off)
            ScoreDisplay.show_digit(15, GameSettings.background_sounds);
            break; 
        case SETTINGS_BACKGROUND_MUSIC:         // P2: 1/0 (on/off)
            ScoreDisplay.show_digit(15, GameSettings.background_music);
            break; 
        case SETTINGS_BALLS_PER_GAME:           // P2: 3/5
            ScoreDisplay.show_digit(15, GameSettings.five_balls_per_game ? 5 : 3);
            break; 
        case SETTINGS_SPOT_LIGHT_STRATEGY:      // P2: 0=conservative / 1=liberal
            ScoreDisplay.show_digit(15, GameSettings.spot_light_strategy);
            break; 
        case SETTINGS_MULTIPLIER_STEP_UP:       // P2: 0=both 3 banks / 1=one 3 bank
            ScoreDisplay.show_digit(15, GameSettings.multiplier_step_up);
            break; 
        case SETTINGS_SPINNER_ADVANCES:         // P2: 0=center target / 1=any outside target
            ScoreDisplay.show_digit(15, GameSettings.spinner_value_advancement);
            break; 
        case SETTINGS_EIGHT_BANK_WOW_STARTS:    // P2: 0=on making 9 / 1=on making 8
            ScoreDisplay.show_digit(15, GameSettings.eight_bank_wow_turn_on ? 8 : 9);
            break; 
        case SETTINGS_SUPER_BONUS_LIGHTS:       // P2: 0=on making 9 / 1=on making 8
            ScoreDisplay.show_digit(15, GameSettings.when_super_bonus_lights ? 8 : 9;
            break; 
        case SETTINGS_THREE_BANK_WOW_STARTS:    // P2: 0=on 7x multiplier / 1=on 6x and 7x multiplier achieved 
            ScoreDisplay.show_digit(15, GameSettings.three_bank_wow_turn_on);
            break; 
        case SETTINGS_WOW_AWARD_TYPE:           // P2: 0=70K / 1=shoot again
            ScoreDisplay.show_digit(15, GameSettings.wow_award_type);
            break; 
        case SETTINGS_SPECIAL_AWARD_TYPE:       // P2: 0=90K / 1=130K / 2=shoot again / 3=???
            ScoreDisplay.show_digit(15, GameSettings.special_award_type);
            break; 
        case SETTINGS_UNLIMITED_SPECIALS:       // P2: 0=one per ball, 1=unlimited
            ScoreDisplay.show_digit(15, GameSettings.unlimited_specials);
            break;
    } 
}

void CAttract::settings_next_value()
{
    switch (menu_item) {
        case SETTINGS_HIGH_SCORE_THRESHOLD_1:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[0].add_thousands(0x10);
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[0]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_2:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[1].add_thousands(0x10);
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[1]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_3:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[2].add_thousands(0x10);
		    ScoreDisplay.show_bcd_num(1, GameSettings.awards_threshold[2]);
            break;
        case SETTINGS_HIGH_SCORE_TO_DATE:       // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.highest_score.from_decimal(0);
                item_value = 0;
            }
            ScoreDisplay.show_bcd_num(1, GameSettings.highest_score);
            break;
        case SETTINGS_GAMES_PLAYED:             // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.games_played = 0;
                item_value = 0;
            }
            ScoreDisplay.show_bcd_num(1, GameSettings.games_played);
            break;
        case SETTINGS_BALLS_SERVED:             // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.balls_served = 0;
                item_value = 0;
            }
            ScoreDisplay.show_bcd_num(1, GameSettings.balls_served);
            break;
        case SETTINGS_BACKGROUND_SOUNDS:        // P2: 1/0 (on/off)
            GameSettings.background_sounds = !GameSettings.background_sounds;
            ScoreDisplay.show_digit(15, GameSettings.background_sounds);
            break; 
        case SETTINGS_BACKGROUND_MUSIC:         // P2: 1/0 (on/off)
            GameSettings.background_music = !GameSettings.background_music;
            ScoreDisplay.show_digit(15, GameSettings.background_music);
            break; 
        case SETTINGS_BALLS_PER_GAME:           // P2: 3/5
            GameSettings.five_balls_per_game = !GameSettings.five_balls_per_game;
            ScoreDisplay.show_digit(15, GameSettings.five_balls_per_game ? 5 : 3);
            break; 
        case SETTINGS_SPOT_LIGHT_STRATEGY:      // P2: 0=conservative / 1=liberal
            GameSettings.five_balls_per_game = !GameSettings.five_balls_per_game;
            ScoreDisplay.show_digit(15, GameSettings.spot_light_strategy);
            break; 
        case SETTINGS_MULTIPLIER_STEP_UP:       // P2: 0=both 3 banks / 1=one 3 bank
            GameSettings.multiplier_step_up = !GameSettings.multiplier_step_up;
            ScoreDisplay.show_digit(15, GameSettings.multiplier_step_up);
            break; 
        case SETTINGS_SPINNER_ADVANCES:         // P2: 0=center target / 1=any outside target
            GameSettings.spinner_value_advancement = !GameSettings.spinner_value_advancement;
            ScoreDisplay.show_digit(15, GameSettings.spinner_value_advancement);
            break; 
        case SETTINGS_EIGHT_BANK_WOW_STARTS:    // P2: 0=on making 9 / 1=on making 8
            GameSettings.eight_bank_wow_turn_on = !GameSettings.eight_bank_wow_turn_on;
            ScoreDisplay.show_digit(15, GameSettings.eight_bank_wow_turn_on ? 8 : 9);
            break; 
        case SETTINGS_SUPER_BONUS_LIGHTS:       // P2: 0=on making 9 / 1=on making 8
            GameSettings.when_super_bonus_lights = !GameSettings.when_super_bonus_lights;
            ScoreDisplay.show_digit(15, GameSettings.when_super_bonus_lights ? 8 : 9;
            break; 
        case SETTINGS_THREE_BANK_WOW_STARTS:    // P2: 0=on 7x multiplier / 1=on 6x and 7x multiplier achieved 
            GameSettings.three_bank_wow_turn_on = !GameSettings.three_bank_wow_turn_on;
            ScoreDisplay.show_digit(15, GameSettings.three_bank_wow_turn_on);
            break; 
        case SETTINGS_WOW_AWARD_TYPE:           // P2: 0=70K / 1=shoot again
            GameSettings.wow_award_type = !GameSettings.wow_award_type;
            ScoreDisplay.show_digit(15, GameSettings.wow_award_type);
            break; 
        case SETTINGS_SPECIAL_AWARD_TYPE:       // P2: 0=90K / 1=130K / 2=shoot again / 3=???
            GameSettings.special_award_type++;
            GameSettings.special_award_type &= 0x03; // range 0..3
            ScoreDisplay.show_digit(15, GameSettings.special_award_type);
            break; 
        case SETTINGS_UNLIMITED_SPECIALS:       // P2: 0=one per ball, 1=unlimited
            GameSettings.unlimited_specials = !GameSettings.unlimited_specials;
            ScoreDisplay.show_digit(15, GameSettings.unlimited_specials);
            break; 
    }
}

void CAttract::diagnostics_next_item()
{
    menu_item++;
    if (menu_item > DIAGNOSTICS_SOUND)
        menu_item = 0;
    
    items_value = 0;
    
    ScoreDisplay.hide_all();        
    ScoreDisplay.show_digit(0, mode);
    ScoreDisplay.show_digit(3, (menu_item / 10));
    ScoreDisplay.show_digit(4, (menu_item % 10));
    
    // we're supposed to update the display and start the action...
}

void CAttract::diagnostics_next_value()
{
    switch (menu_item)
    {
        case DIAGNOSTICS_LAMP_MATRIX_COLUMNS:           // all lamps in one row. after 1 sec, P2=next row
        case DIAGNOSTICS_LAMP_MATRIX_ROWS:              // all lamps in one col. after 1 sec, P2=next col
        case DIAGNOSTICS_LAMP_MATRIX_ALL:               // all lamps blink on-off every second   
        case DIAGNOSTICS_SCORE_DISPLAY_SINGLE_NUMBER:   // display num 0 in all displays.     P2=2,3,4 etc
        case DIAGNOSTICS_SCORE_DISPLAY_SINGLE_DIGIT:    // display num 8 in one digit         P2=next digit etc
        case DIAGNOSTICS_SWITCH_MATRIX:                 // display num of switch closed.
        case DIAGNOSTICS_COILS:                         // fire a coil every second.          P2=start & next coil
        case DIAGNOSTICS_Q_RELAY:                       // turn Q relay on off                P2=toggle
        case DIAGNOSTICS_SOUND:                         // play sound                         P2=next sound
    }
}

*/
