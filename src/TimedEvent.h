#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Parser.h"
#include "Timer.h"

class TimedEvent;

class TimedEventPool {
public:
    int field_0x0;
    int field_0x4;
    int m_count;
    TimedEvent* m_free_list;
    TimedEvent* m_pool;
    int m_pool_size;
};

class TimedEvent : public Parser {
public:
    TimedEvent();
    TimedEvent(const TimedEvent& other);
    ~TimedEvent();

    void SetData(int);
    int Update();
    void Delete();
    static TimedEvent* Create(TimedEventPool*, void*, void*);

private:
    int m_fields_at_88[6];
    Timer m_timer_at_a0;
    Timer m_timer_at_b4;
    void* m_allocated_memory_at_c8;

    // For the update function
    int m_duration;
    void* m_next_event_data;
    Timer m_timer;
};

#endif // TIMEDEVENT_H
