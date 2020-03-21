

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
    void idle_handle_event(Event& e);

    void start_game_mode(byte mode);

    void start_radio_mode();
    void radio_handle_event(Event& e);

    void start_settings_mode();
    void settings_handle_event(Event& e);
    void settings_show_menu_item_value();
    void settings_change_menu_item_value();

    void start_diagnostics_mode();
    void diagnostics_handle_event(Event& e);
    void diagnostics_show_menu_item();
    void diagnostics_menu_item_action();
};
