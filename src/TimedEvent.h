#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Timer.h"
#include "InputManager.h"  // for MousePoint

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

    void* nextEvent;         // 0x00 - used as next pointer in lists (reuses vtable slot)
    int m_type;              // 0x04 - event type (0=one-shot, 1=repeating, 2=countdown display)
    int m_sourceAddress;     // 0x08 - stores sourceAddress from message (who sent this event)
    int m_duration;          // 0x0c - timer duration in ms
    SC_Message* m_eventData; // 0x10 - pointer to event message data (sent when timer expires)
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
    PooledEvent* CopyFrom(const PooledEvent* other);
    
    // Get pointer to embedded message at offset 0x08
    PooledEvent* GetEmbeddedMessage() { return (PooledEvent*)(&next + 2); }

    PooledEvent* next;        // 0x00 - next pointer for free list
    PooledEvent* prev;        // 0x04 - prev/callback pointer
    // Embedded SC_Message fields starting at 0x08
    int field_0x8;           // 0x08 (m_subObject)
    int m_duration;          // 0x0c (isProcessingKey)
    char data_0x10[0x20];    // 0x10-0x2F (includes currentKey at 0x18)
    int field_0x30;          // 0x30
    int field_0x34;          // 0x34
    int field_0x38;          // 0x38 (lineNumber)
    int field_0x3c;          // 0x3c
    char m_data_0x40[64];    // 0x40-0x7F (includes savedFilePos, filename)
    int field_0x80;          // 0x80
    int field_0x84;          // 0x84
    int targetAddress;       // 0x88 (stores pFile from Parser)
    int sourceAddress;       // 0x8c (stores targetAddress from SC_Message)
    int command;             // 0x90 (stores sourceAddress from SC_Message)
    int data;                // 0x94 (stores command from SC_Message)
    int priority;            // 0x98 (stores data from SC_Message)
    int param1;              // 0x9c (stores priority from SC_Message)
    int param2;              // 0xa0 (stores param1 from SC_Message)
    MousePoint clickPos;     // 0xa4 (stores param2/clickX from SC_Message, MousePoint has empty destructor)
    int mouseX;              // 0xac (stores clickY from SC_Message)
    int mouseY;              // 0xb0 (stores mouseX from SC_Message)
    int field_0xb4;          // 0xb4 (stores mouseY from SC_Message)
    int field_0xb8;          // 0xb8 (stores field_b4 from SC_Message)
    int userPtr;             // 0xbc (stores field_b8 from SC_Message)
    int field_0xc0;          // 0xc0 (stores userPtr from SC_Message)
    int field_0xc4;          // 0xc4
};

#endif // TIMEDEVENT_H
