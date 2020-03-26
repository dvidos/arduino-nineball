
class SpinnerClass
{
public:
    void init();
    void advance_value();
    void advance_to_top_value();
    void on_spinner_spun();
    dword get_spinner_score_bcd();
    static void reset_spinner_value();

protected:
    byte value: 4; // 0=100, 1=400, 2=900, 3=1600, 4=2500
};
