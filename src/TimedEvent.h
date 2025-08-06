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

    void Init();
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
    Timer m_timer; // at 0x14
    char unknown_fields[0x88 - 0x14 - 20];
    int field_0x88[6];
    Timer m_timer2; // at 0xa0
    Timer m_timer3; // at 0xb4
    void* m_allocated_memory; // at 0xc8
};

#endif // TIMEDEVENT_H
