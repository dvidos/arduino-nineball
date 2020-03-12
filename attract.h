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


#define ATTRACT_MENU_NORMAL_GAME         0   // Start: start game, P2: nothing
#define ATTRACT_MENU_EVA_HAPPY_MODE      1   // Start: start game, infinate balls.
#define ATTRACT_MENU_RADIO               2   // Start: start jukebox?   P2: next random song?
#define ATTRACT_MENU_SETTINGS            3   // Start: get into settings. P1: cycle settings, P2: cycle value for each setting
#define ATTRACT_MENU_DIAGNOSTICS         4   // Start: into diagnostics. P1: cycle program, P2: cycle variation
#define ATTRACT_MENU_ITEMS_COUNT         5


#define ATTRACT_MODE_IDLE          0
#define ATTRACT_MODE_GAME          1
#define ATTRACT_MODE_RADIO         2
#define ATTRACT_MODE_DIAGNOSTICS   3
#define ATTRACT_MODE_SETTINGS      4


#define SETTINGS_HIGH_SCORE_THRESHOLD_1   0   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_THRESHOLD_2   1   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_THRESHOLD_3   2   // P2: increase 10k (hold down must keep increasing) zero is allowed
#define SETTINGS_HIGH_SCORE_TO_DATE       3   // P2: Press & release 3 times to reset
#define SETTINGS_GAMES_PLAYED             4   // P2: Press & release 3 times to reset
#define SETTINGS_BALLS_SERVED             5   // P2: Press & release 3 times to reset
#define SETTINGS_BACKGROUND_SOUNDS        6   // P2: 1/0 (on/off)
#define SETTINGS_BACKGROUND_MUSIC         7   // P2: 1/0 (on/off)
#define SETTINGS_BALLS_PER_GAME           8   // P2: 3/5
#define SETTINGS_SPOT_LIGHT_STRATEGY      9   // P2: 0=conservative / 1=liberal
#define SETTINGS_MULTIPLIER_STEP_UP      10   // P2: 0=both 3 banks / 1=one 3 bank
#define SETTINGS_SPINNER_ADVANCES        11   // P2: 0=center target / 1=any outside target
#define SETTINGS_EIGHT_BANK_WOW_STARTS   12   // P2: 0=on making 9 / 1=on making 8
#define SETTINGS_SUPER_BONUS_LIGHTS      13   // P2: 0=on making 9 / 1=on making 8
#define SETTINGS_THREE_BANK_WOW_STARTS   14   // P2: 0=on 7x multiplier / 1=on 6x and 7x multiplier achieved
#define SETTINGS_WOW_AWARD_TYPE          15   // P2: 0=70K / 1=shoot again
#define SETTINGS_SPECIAL_AWARD_TYPE      16   // P2: 0=90K / 1=130K / 2=shoot again / 3=???
#define SETTINGS_UNLIMITED_SPECIALS      17   // P2: 0=one per ball, 1=unlimited
#define SETTINGS_SAVE_AND_EXIT           18   // P2: save and exit
#define SETTINGS_OPTIONS_COUNT           19


#define DIAGNOSTICS_LAMP_MATRIX_COLUMNS          0 // all lamps in one row. after 1 sec, P2=next row
#define DIAGNOSTICS_LAMP_MATRIX_ROWS             1 // all lamps in one col. after 1 sec, P2=next col
#define DIAGNOSTICS_LAMP_MATRIX_ALL              2 // all lamps blink on-off every second
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_NUMBER  3 // display num 0 in all displays.     P2=2,3,4 etc
#define DIAGNOSTICS_SCORE_DISPLAY_SINGLE_DIGIT   4 // display num 8 in one digit         P2=next digit etc
#define DIAGNOSTICS_SWITCH_MATRIX                5 // display num of switch closed.
#define DIAGNOSTICS_COILS                        6 // fire a coil every second.          P2=start & next coil
#define DIAGNOSTICS_Q_RELAY                      7 // turn Q relay on off                P2=toggle
#define DIAGNOSTICS_SOUND                        8 // play sound                         P2=next sound
#define DIAGNOSTICS_EXIT                         9 // exit
#define DIAGNOSTICS_OPTIONS_COUNT               10


#include "defines.h"

extern CGameplay Gameplay;
extern CGameSettings GameSettings;
extern CScoreDisplay ScoreDisplay;
extern CLampMatrix LampMatrix;


class CAttract
{
public:
    void start();
    void handle_event(Event& e);

private:
    byte mode: 4;
    byte menu_item;
    byte item_value;

    void start_idle_mode();
    void start_game_mode(byte mode);
    void start_radio_mode();
    void start_diagnostics_mode();
    void start_settings_mode();

    void idle_handle_event(Event& e);
    void radio_handle_event(Event& e);
    void diagnostics_handle_event(Event& e);
    void settings_handle_event(Event& e);

    void diagnostics_show_menu_item();
    void diagnostics_menu_item_action();
    void settings_show_menu_item_value();
    void settings_change_menu_item_value();


};

void CAttract::start()
{
    start_idle_mode();
    LOG("Attract mode started");
}

void CAttract::handle_event(Event& e)
{
    switch (mode)
    {
        case ATTRACT_MODE_IDLE:
            idle_handle_event(e);
            break;
        case ATTRACT_MODE_GAME:
            Gameplay.handle_event(e);
            break;
        case ATTRACT_MODE_RADIO:
            radio_handle_event(e);
            break;
        case ATTRACT_MODE_DIAGNOSTICS:
            diagnostics_handle_event(e);
            break;
        case ATTRACT_MODE_SETTINGS:
            settings_handle_event(e);
            break;

    }
}

void CAttract::start_idle_mode()
{
    LOG("Attract starting idle mode");

    mode = ATTRACT_MODE_IDLE;
    menu_item = 0;
    ScoreDisplay.display_bcd_num(0, GameSettings.highest_score);
    ScoreDisplay.display_bcd_num(1, GameSettings.highest_score);

    // implement mode to have anumations for ever until we stop them.
    Animator.start(ANIM_TOP_LOOP_ADVANCE_VALUE);
    Animator.start(ANIM_SPINNER_INCREASE_VALUE);
    Animator.start(ANIM_BONUS_MULTIPLIER);
}

void CAttract::start_game_mode(byte game_mode)
{
    LOG("Attract starting game mode");

    mode = ATTRACT_MODE_GAME;
    Gameplay.start(game_mode);
}

void CAttract::start_radio_mode()
{
    LOG("Attract starting radio mode");

    // start first song
    // start timeouts
}

void CAttract::start_diagnostics_mode()
{
    LOG("Attract starting diagnostics mode");

    // start diagnostics menu
    mode = ATTRACT_MODE_DIAGNOSTICS;
    ScoreDisplay.show_digit(1, mode);

    // start the menu
    menu_item = 0;
    ScoreDisplay.show_digit(4, menu_item / 10);
    ScoreDisplay.show_digit(5, menu_item % 10);
}

void CAttract::start_settings_mode()
{
    LOG("Attract starting settings mode");

    // start settings menu
    mode = ATTRACT_MODE_SETTINGS;
    ScoreDisplay.show_digit(1, mode);

    // start the menu
    menu_item = 0;
    settings_show_menu_item_value();
}

void CAttract::idle_handle_event(Event& e)
{
    if (e.type == switch_closed)
    {
        switch (e.number)
        {
            case SW_MENU_LEFT:
                // next menu selection.
                // maybe play audio that announces it.
                // maybe start timeout to remove it.
                menu_item++;
                if (menu_item >= ATTRACT_MENU_ITEMS_COUNT)
                    menu_item = 0;
                ScoreDisplay.show_digit(1, menu_item);
                break;

            case SW_START:
                // start something depending on menu selection
                // may play a sound FX too (especcially if starting a game)
                if (menu_item == ATTRACT_MENU_NORMAL_GAME) {
                    start_game_mode(GAMEPLAY_MODE_NORMAL);
                } else if (menu_item == ATTRACT_MENU_EVA_HAPPY_MODE) {
                    start_game_mode(GAMEPLAY_MODE_EVA_HAPPY);
                } else if (menu_item == ATTRACT_MENU_RADIO) {
                    start_radio_mode();
                } else if (menu_item == ATTRACT_MENU_SETTINGS) {
                    start_settings_mode();
                } else if (menu_item == ATTRACT_MENU_DIAGNOSTICS) {
                    start_diagnostics_mode();
                }
                break;
        }
    }
}

void CAttract::radio_handle_event(Event& e)
{
    // timeouts to detect song end,
    // start next random song
    // start or P1 can exit this mode
    // SW_MENU_RIGHT could be "skip"

    if (e.type == switch_closed)
    {
        switch (e.number)
        {
            case SW_START:
                start_idle_mode();
                break;
            case SW_MENU_LEFT:
                break;
            case SW_MENU_RIGHT:
                break;
        }
    }
}

void CAttract::diagnostics_handle_event(Event& e)
{
    if (e.type == switch_closed)
    {
        switch (e.number)
        {
            case SW_START:
                break;
            case SW_MENU_LEFT:
                menu_item += 1;
                if (menu_item >= DIAGNOSTICS_OPTIONS_COUNT)
                    menu_item = 0;
                diagnostics_show_menu_item();
                break;
            case SW_MENU_RIGHT:
                diagnostics_menu_item_action();
                break;
        }
    }
}

void CAttract::diagnostics_show_menu_item()
{
    // show one based
    ScoreDisplay.show_digit(4, (menu_item + 1) / 10);
    ScoreDisplay.show_digit(5, (menu_item + 1) % 10);

    // now show value and/or act on it.
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
        case DIAGNOSTICS_EXIT:
            start_idle_mode();
            break;
    }
}

void CAttract::diagnostics_menu_item_action()
{
    // switch diagnostics objective (e.g. column or coild number)
    // some do need timeouts to function.
}


void CAttract::settings_handle_event(Event& e)
{
    if (e.type == switch_closed)
    {
        switch (e.number)
        {
            case SW_START:
                break;
            case SW_MENU_LEFT:
                menu_item += 1;
                if (menu_item >= SETTINGS_OPTIONS_COUNT)
                    menu_item = 0;
                settings_show_menu_item_value();
                break;
            case SW_MENU_RIGHT:
                settings_change_menu_item_value();
                settings_show_menu_item_value();
                break;
        }
    }
}

void CAttract::settings_show_menu_item_value()
{
    BcdNum n;

    // show one based
    ScoreDisplay.show_digit(4, (menu_item + 1) / 10);
    ScoreDisplay.show_digit(5, (menu_item + 1) % 10);

    // clear right display
    ScoreDisplay.hide_digit(9);
    ScoreDisplay.hide_digit(10);
    ScoreDisplay.hide_digit(11);
    ScoreDisplay.hide_digit(12);
    ScoreDisplay.hide_digit(13);
    ScoreDisplay.hide_digit(14);
    ScoreDisplay.hide_digit(15);

    // now show value and/or act on it.
    switch (menu_item) {
        case SETTINGS_HIGH_SCORE_THRESHOLD_1:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            ScoreDisplay.display_bcd_num(1, GameSettings.awards_threshold[0]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_2:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            ScoreDisplay.display_bcd_num(1, GameSettings.awards_threshold[1]);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_3:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            ScoreDisplay.display_bcd_num(1, GameSettings.awards_threshold[2]);
            break;
        case SETTINGS_HIGH_SCORE_TO_DATE:       // P2: Press & release 3 times to reset
            ScoreDisplay.display_bcd_num(1, GameSettings.highest_score);
            break;
        case SETTINGS_GAMES_PLAYED:             // P2: Press & release 3 times to reset
            n.from_decimal(GameSettings.games_played);
            ScoreDisplay.display_bcd_num(1, n);
            break;
        case SETTINGS_BALLS_SERVED:             // P2: Press & release 3 times to reset
            n.from_decimal(GameSettings.balls_served);
            ScoreDisplay.display_bcd_num(1, n);
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
            ScoreDisplay.show_digit(15, GameSettings.when_super_bonus_lights ? 8 : 9);
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
        case SETTINGS_SAVE_AND_EXIT:
            break;
    }
}

void CAttract::settings_change_menu_item_value()
{
    // change value or perform "enter" action
    switch (menu_item) {
        case SETTINGS_HIGH_SCORE_THRESHOLD_1:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[0].add_bcd(0x10000);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_2:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[1].add_bcd(0x10000);
            break;
        case SETTINGS_HIGH_SCORE_THRESHOLD_3:   // P2: increase 10k (hold down must keep increasing) zero is allowed
            GameSettings.awards_threshold[2].add_bcd(0x10000);
            break;
        case SETTINGS_HIGH_SCORE_TO_DATE:       // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.highest_score.from_decimal(0);
                item_value = 0;
            }
            break;
        case SETTINGS_GAMES_PLAYED:             // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.games_played = 0;
                item_value = 0;
            }
            break;
        case SETTINGS_BALLS_SERVED:             // P2: Press & release 3 times to reset
            item_value += 1;
            if (item_value > 3) {
                GameSettings.balls_served = 0;
                item_value = 0;
            }
            break;
        case SETTINGS_BACKGROUND_SOUNDS:        // P2: 1/0 (on/off)
            GameSettings.background_sounds = !GameSettings.background_sounds;
            break;
        case SETTINGS_BACKGROUND_MUSIC:         // P2: 1/0 (on/off)
            GameSettings.background_music = !GameSettings.background_music;
            break;
        case SETTINGS_BALLS_PER_GAME:           // P2: 3/5
            GameSettings.five_balls_per_game = !GameSettings.five_balls_per_game;
            break;
        case SETTINGS_SPOT_LIGHT_STRATEGY:      // P2: 0=conservative / 1=liberal
            GameSettings.five_balls_per_game = !GameSettings.five_balls_per_game;
            break;
        case SETTINGS_MULTIPLIER_STEP_UP:       // P2: 0=both 3 banks / 1=one 3 bank
            GameSettings.multiplier_step_up = !GameSettings.multiplier_step_up;
            break;
        case SETTINGS_SPINNER_ADVANCES:         // P2: 0=center target / 1=any outside target
            GameSettings.spinner_value_advancement = !GameSettings.spinner_value_advancement;
            break;
        case SETTINGS_EIGHT_BANK_WOW_STARTS:    // P2: 0=on making 9 / 1=on making 8
            GameSettings.eight_bank_wow_turn_on = !GameSettings.eight_bank_wow_turn_on;
            break;
        case SETTINGS_SUPER_BONUS_LIGHTS:       // P2: 0=on making 9 / 1=on making 8
            GameSettings.when_super_bonus_lights = !GameSettings.when_super_bonus_lights;
            break;
        case SETTINGS_THREE_BANK_WOW_STARTS:    // P2: 0=on 7x multiplier / 1=on 6x and 7x multiplier achieved
            GameSettings.three_bank_wow_turn_on = !GameSettings.three_bank_wow_turn_on;
            break;
        case SETTINGS_WOW_AWARD_TYPE:           // P2: 0=70K / 1=shoot again
            GameSettings.wow_award_type = !GameSettings.wow_award_type;
            break;
        case SETTINGS_SPECIAL_AWARD_TYPE:       // P2: 0=90K / 1=130K / 2=shoot again / 3=???
            GameSettings.special_award_type++;
            GameSettings.special_award_type &= 0x03; // range 0..3
            break;
        case SETTINGS_UNLIMITED_SPECIALS:       // P2: 0=one per ball, 1=unlimited
            GameSettings.unlimited_specials = !GameSettings.unlimited_specials;
            break;
        case SETTINGS_SAVE_AND_EXIT:
            GameSettings.save_to_eeprom();
            start_idle_mode();
            break;
    }
}

