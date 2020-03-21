
class EightBankTargetsClass
{
public:
    byte object_made: 4;          // 0=none, 1..9=1..9
    byte number_nine_number: 4;   // 0=none, 1..8=1..9
    byte wow_number: 4;           // 0=none, 1..8=1..8
    byte special_number: 4;       // 0=none, 2..8=2..8 (1 is missing)
    byte spot_number_enabled: 1;  // this is a timed mode
    byte special_made: 1;         // to allow at most one
    byte super_bonus_made: 1;
    byte bringing_targets_up_tries: 2; // 3 or 4 tries at most.
    byte sequence[8] = { 2, 4, 6, 8, 7, 5, 3, 1 };

    void init();
    void on_target_hit(byte switch_no);

    bool all_drop_targets_down();
    bool any_drop_target_down();
    void bring_drop_targets_up();
    static void verify_drop_targets_are_up();

    void award_current_number();
    void award_number_nine();
    void award_wow();
    void award_special();

    void start_number_nine_sequence();
    void start_wow_sequence();
    void start_special_sequence();
    void start_spot_number_timeout();

    static void spot_number_timed_out();
    static void advance_number_nine_target();
    static void advance_wow_target();
    static void advance_special_target();
};

