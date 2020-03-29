

class CAttract
{
public:
    void start();
    void handle_switch(byte switch_no);

private:
    byte mode: 4;
    byte menu_item;
    byte item_value;
    byte blink_on: 1;

    void start_idle_mode();
    void idle_handle_switch(byte switch_no);

    void start_game_mode(byte mode);

    void start_radio_mode();
    void radio_handle_switch(byte switch_no);
    void radio_next_song();
    void radio_prev_song();
    static void check_song_finished();

    void start_settings_mode();
    void settings_handle_switch(byte switch_no);
    void settings_show_menu_item_value();
    void settings_change_menu_item_value();

    void start_diagnostics_mode();
    void diagnostics_handle_switch(byte switch_no);
    static void diagnostics_every_half_second();
    void diagnostics_show_menu_item();
    void diagnostics_menu_item_action();
};
