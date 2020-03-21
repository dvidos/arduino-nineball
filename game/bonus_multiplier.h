

class BonusMultiplierClass
{
public:
    byte value: 3; // 1=1x, 2=2x..7=7x
    void init(bool was_6_or_higher);
    void increase_multiplier();
};
