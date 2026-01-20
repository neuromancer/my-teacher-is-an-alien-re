#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Timer.h"

class TimedEvent;
class PooledEvent;
class SC_Message;

struct TimedEventList {
    TimedEvent* head;
    TimedEvent* tail;
};

struct PooledEventList {
    PooledEvent* head;
    PooledEvent* tail;
};

class TimedEventPool {
public:
    PooledEventList list;
    int m_count;
    PooledEvent* m_free_list;
    PooledEvent* m_pool;
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
    PooledEvent* Create(void* callback, void* data);
    SC_Message* Pop(SC_Message* buffer);
    SC_Message* PopSafe(SC_Message* buffer);
};

// TimedEvent: 40 bytes (0x28)
// Used in Handler13 for timer events
class TimedEvent {
public:
    TimedEvent();
    ~TimedEvent();
    void SetType(int type);
    int Update();

    void* vtable;            // 0x00 - used as next pointer in lists
    int m_type;              // 0x04 - event type (0, 1, or 2)
    int field_0x8;           // 0x08 - stores sourceAddress from message
    int m_duration;          // 0x0c - timer duration in ms
    void* m_next_event_data; // 0x10 - pointer to next event data (SC_Message*)
    Timer m_timer;           // 0x14 (20 bytes)
};

// PooledEvent: 200 bytes (0xC8)
// Used in TimedEventPool - contains embedded SC_Message at offset 8
// Layout:
//   0x00: next pointer (for free list)
//   0x04: prev/callback pointer
//   0x08: embedded SC_Message data (192 bytes)
class PooledEvent {
public:
    void CopyFrom(const PooledEvent* other);

    void* next;              // 0x00 - next pointer for free list
    void* prev;              // 0x04 - prev/callback pointer
    // Embedded SC_Message fields starting at 0x08
    int field_0x8;           // 0x08
    int m_duration;          // 0x0c (maps to SC_Message field)
    char data_0x10[0x20];    // 0x10-0x2F
    int field_0x30;          // 0x30
    int field_0x34;          // 0x34
    int field_0x38;          // 0x38
    int field_0x3c;          // 0x3c
    char m_data_0x40[64];    // 0x40 - 0x80 (0x40 bytes)
    int field_0x80;          // 0x80
    int field_0x84;          // 0x84
    int targetAddress;       // 0x88
    int sourceAddress;       // 0x8c
    int command;             // 0x90
    int data;                // 0x94
    int priority;            // 0x98
    int param1;              // 0x9c
    int param2;              // 0xa0
    int clickX;              // 0xa4
    int clickY;              // 0xa8
    int mouseX;              // 0xac
    int mouseY;              // 0xb0
    int field_0xb4;          // 0xb4
    int field_0xb8;          // 0xb8
    int userPtr;             // 0xbc
    int field_0xc0;          // 0xc0
    int field_0xc4;          // 0xc4
};

#endif // TIMEDEVENT_H
