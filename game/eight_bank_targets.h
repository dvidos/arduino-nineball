
class EightBankTargetsClass
{
public:
    void init(bool was_super_bonus_made_previous_ball);
    void on_target_hit(byte switch_no);
    void start_spot_number_timeout();
    void make_current_target_object(byte target_no = 0);
    byte get_object_made();
    bool get_super_bonus_made();

    // static functions called by timer interrupts
    static void spot_number_timed_out();
    static void advance_number_nine_target();
    static void advance_wow_target();
    static void advance_special_target();
    static void verify_drop_targets_are_up();

    byte object_made: 4;          // 0=none, 1..9=1..9
    byte number_nine_number: 4;   // 0=none, 1..8=1..9
    byte wow_number: 4;           // 0=none, 1..8=1..8
    byte special_number: 4;       // 0=none, 2..8=2..8 (1 is missing)
    byte spot_number_enabled: 1;  // this is a timed mode
    byte left_inlane_enabled: 1;  // this allows us to flip the inlanes
    byte right_inlane_enabled: 1;
    byte special_made: 1;         // to allow at most one
    byte super_bonus_made: 1;
    byte bringing_targets_up_tries: 2; // 3 or 4 tries at most.

private:

    struct object_info {
        byte switch_no: 6;
        byte target_lamp_no: 6;
        byte wow_lamp_no: 6;
        byte special_lamp_no: 6;
        byte bonus_lamp_no: 6;
    } objects[9] = {
        { 0, 0, 0, 0, 0 }, // dummy entry to make things 1-based.
        { SW_LEFT_BANK_TARGET_1, LAMP_OBJECT_1_DROP_TARGET, LAMP_OBJECT_1_WOW, 0,                     LAMP_BONUS_MADE_1 },
        { SW_LEFT_BANK_TARGET_2, LAMP_OBJECT_2_DROP_TARGET, LAMP_OBJECT_2_WOW, LAMP_OBJECT_2_SPECIAL, LAMP_BONUS_MADE_2 },
        { SW_LEFT_BANK_TARGET_3, LAMP_OBJECT_3_DROP_TARGET, LAMP_OBJECT_3_WOW, LAMP_OBJECT_3_SPECIAL, LAMP_BONUS_MADE_3 },
        { SW_LEFT_BANK_TARGET_4, LAMP_OBJECT_4_DROP_TARGET, LAMP_OBJECT_4_WOW, LAMP_OBJECT_4_SPECIAL, LAMP_BONUS_MADE_4 },
        { SW_LEFT_BANK_TARGET_5, LAMP_OBJECT_5_DROP_TARGET, LAMP_OBJECT_5_WOW, LAMP_OBJECT_5_SPECIAL, LAMP_BONUS_MADE_5 },
        { SW_LEFT_BANK_TARGET_6, LAMP_OBJECT_6_DROP_TARGET, LAMP_OBJECT_6_WOW, LAMP_OBJECT_6_SPECIAL, LAMP_BONUS_MADE_6 },
        { SW_LEFT_BANK_TARGET_7, LAMP_OBJECT_7_DROP_TARGET, LAMP_OBJECT_7_WOW, LAMP_OBJECT_7_SPECIAL, LAMP_BONUS_MADE_7 },
        { SW_LEFT_BANK_TARGET_8, LAMP_OBJECT_8_DROP_TARGET, LAMP_OBJECT_8_WOW, LAMP_OBJECT_8_SPECIAL, LAMP_BONUS_MADE_8 }
    };

    byte get_target_number(byte switch_no);

    inline void set_target_lamps(bool value);
    inline void set_wow_lamps(bool value);
    inline void set_special_lamps(bool value);
    inline void set_bonus_lamps(bool value);

    bool all_drop_targets_down();
    bool any_drop_target_down();
    void bring_drop_targets_up();

    void award_current_number(byte target_no);
    void award_number_nine(byte target_no);
    void award_wow(byte target_no);
    void award_special(byte target_no);

    void start_number_nine_sequence();
    void start_wow_sequence();
    void start_special_sequence();

    byte get_next_target_number(byte current = 0);
};

