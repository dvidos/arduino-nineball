

class Event
{
public:
    // Event();
    Event(event_type type, byte number);
    
    event_type type;
    byte number; // either switch_no or timeout_no
};

Event::Event(event_type type, byte number)
{
    this->type = type;
    this->number = number;
}