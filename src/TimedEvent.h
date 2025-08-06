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

    void* Init();
    void SetData(int);
    int Update();
    void Delete();
    static TimedEvent* Create(TimedEventPool*, void*, void*);

private:
    int field_0x0;
    int m_type;
    int field_0x8;
    int m_duration;
    void* m_next_event_data;
    Timer m_timer;
    char field_0x30[0x90]; // Placeholder for the rest
};

#endif // TIMEDEVENT_H
