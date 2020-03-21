
class ThreeBankTargetsClass
{
public:
    byte lit_pink_target: 2; // 0=off, 1=top, 2=right.
    byte lit_wow_target: 3;    // 0=none, 1,2=top ones, 3,4 right ones
    void init();
    void on_target_hit(byte switch_no);
    void enable_or_flip_pink_target();
    static void turn_off_pink_target();
    void start_wow();
    static void move_lit_wow_target();
};

