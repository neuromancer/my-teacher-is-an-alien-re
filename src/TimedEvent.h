#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Timer.h"

class TimedEvent;
class SC_Message;

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

    TimedEventPool() {
        m_count = 0;
        m_free_list = 0;
        list.tail = 0;
        list.head = 0;
        m_pool = 0;
        m_pool_size = 0x32;
    }
    ~TimedEventPool();
    TimedEvent* Create(void* callback, void* data);
    SC_Message* Pop(SC_Message* buffer);
    SC_Message* PopSafe(SC_Message* buffer);
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
    int targetAddress;    // 0x88 - destination handler address
    int sourceAddress;    // 0x8c - source/from address
    int command;          // 0x90 - message type/command code
    int data;             // 0x94 - associated data
    int priority;         // 0x98 - queue priority
    int param1;           // 0x9c
    int param2;           // 0xa0
    int clickX;           // 0xa4 - mouse click X
    int clickY;           // 0xa8 - mouse click Y
    int mouseX;           // 0xac - current mouse X
    int mouseY;           // 0xb0 - current mouse Y
    int field_0xb4;       // 0xb4
    int field_0xb8;       // 0xb8
    int userPtr;          // 0xbc - user pointer
    int field_0xc0;       // 0xc0
    int field_0xc4;       // 0xc4
};

#endif // TIMEDEVENT_H
