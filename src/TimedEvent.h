#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Timer.h"

class TimedEvent;

struct TimedEventList {
    TimedEvent* head;
    TimedEvent* tail;
};

class TimedEventPool {
public:
    TimedEventList list;
    int m_count;
    TimedEvent* m_free_list;
    TimedEvent* m_pool;
    int m_pool_size;

    TimedEvent* Create(void* callback, void* data);
};

class TimedEvent {
public:
    TimedEvent();
    void CopyFrom(const TimedEvent* other);
    ~TimedEvent();


    int Update();

    void* vtable;         // 0x00
    int m_type;           // 0x04
    int field_0x8;        // 0x08
    int m_duration;       // 0x0c
    void* m_next_event_data; // 0x10
    Timer m_timer;        // 0x14 (20 bytes = 5 dwords)
    // After Timer: 0x28
    int field_0x28;       // 0x28
    int field_0x2c;       // 0x2c
    int field_0x30;       // 0x30
    int field_0x34;       // 0x34
    int field_0x38;       // 0x38
    int field_0x3c;       // 0x3c
    char m_data_0x40[64]; // 0x40 - 0x80 (0x40 bytes)
    int field_0x80;       // 0x80
    int field_0x84;       // 0x84
    int field_0x88;       // 0x88
    int field_0x8c;       // 0x8c
    int field_0x90;       // 0x90
    int field_0x94;       // 0x94
    int field_0x98;       // 0x98
    int field_0x9c;       // 0x9c
    int field_0xa0;       // 0xa0
    int field_0xa4;       // 0xa4
    int field_0xa8;       // 0xa8
    int field_0xac;       // 0xac
    int field_0xb0;       // 0xb0
    int field_0xb4;       // 0xb4
    int field_0xb8;       // 0xb8
    int field_0xbc;       // 0xbc
};

#endif // TIMEDEVENT_H
