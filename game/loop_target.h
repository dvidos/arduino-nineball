
class LoopTargetClass
{
public:
    byte value: 4; // 0=10k, 1=20k, 2=30k, 3=40k, 4=173k
    byte left_outlane: 1;  // 1=on, 0=off
    byte right_outlane: 1; // 1=on, 0=off
    void init();
    void advance_value();
    void collect_value();
    void enable_or_flip_outlanes();
    void on_passed_outlane(byte switch_no);
    static void turn_off_outlanes();
};

