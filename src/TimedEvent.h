#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

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

class TimedEvent {
public:
    TimedEvent();
    TimedEvent(const TimedEvent& other);
    ~TimedEvent();

    void SetData(int);
    int Update();
    static TimedEvent* Create(TimedEventPool*, void*, void*);

private:
    void* vtable;
    int m_type;
    int field_0x8;
    int m_duration;
    void* m_next_event_data;
    Timer m_timer;
};

#endif // TIMEDEVENT_H
