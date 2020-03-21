
class SpinnerClass
{
public:
    byte value: 4; // 0=100, 1=400, 2=900, 3=1600, 4=2500
    void init();
    void advance_value();
    void advance_to_top_value();
    void on_spinner_spun();
    static void reset_spinner_value();
};
