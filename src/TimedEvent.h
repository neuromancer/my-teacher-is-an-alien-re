#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "Timer.h"
#include "InputManager.h"  // for MousePoint

class TimedEvent;
class PooledEvent;
class SC_MessageParser;
class SpriteAction;

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
    TimedEventPool(int ps) {
        m_count = 0;
        m_free_list = 0;
        list.tail = 0;
        list.head = 0;
        m_pool = 0;
        m_pool_size = ps;
    }
    ~TimedEventPool();
    SC_MessageParser* Pop(SC_MessageParser* buffer);
    SC_MessageParser* PopSafe(SC_MessageParser* buffer);
    SpriteAction* Pop(SpriteAction* buffer);
    SpriteAction* PopSafe(SpriteAction* buffer);
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
    SC_MessageParser* m_eventData; // 0x10 - pointer to event message data (sent when timer expires)
    Timer m_timer;           // 0x14 (20 bytes)
};

// PooledEvent: 200 bytes (0xC8)
// Used in TimedEventPool - contains embedded SC_MessageParser at offset 8
// Layout:
//   0x00: next pointer (for free list)
//   0x04: prev/callback pointer
//   0x08: embedded SC_MessageParser data (192 bytes)
class PooledEvent {
public:
    // Get pointer to embedded event data at offset 0x08
    PooledEvent* GetEmbeddedEvent() { return (PooledEvent*)&field_0x8; }

    // Get pointer to embedded SC_MessageParser at offset 0x08 (for constructor/destructor calls)
    SC_MessageParser* GetEmbeddedSCMessage() { return (SC_MessageParser*)&field_0x8; }

    PooledEvent* next;        // 0x00 - next pointer for free list
    PooledEvent* prev;        // 0x04 - prev/callback pointer
    // Embedded SC_MessageParser fields starting at 0x08
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
    int sourceAddress;       // 0x8c (stores targetAddress from SC_MessageParser)
    int command;             // 0x90 (stores sourceAddress from SC_MessageParser)
    int data;                // 0x94 (stores command from SC_MessageParser)
    int priority;            // 0x98 (stores data from SC_MessageParser)
    int param1;              // 0x9c (stores priority from SC_MessageParser)
    int param2;              // 0xa0 (stores param1 from SC_MessageParser)
    MousePoint clickPos;     // 0xa4 (stores param2/clickX from SC_MessageParser, MousePoint has empty destructor)
    int mouseX;              // 0xac (stores clickY from SC_MessageParser)
    int mouseY;              // 0xb0 (stores mouseX from SC_MessageParser)
    int field_0xb4;          // 0xb4 (stores mouseY from SC_MessageParser)
    int field_0xb8;          // 0xb8 (stores lastKey from SC_MessageParser)
    int userPtr;             // 0xbc (stores time from SC_MessageParser)
    int field_0xc0;          // 0xc0 (stores userPtr from SC_MessageParser)
    int field_0xc4;          // 0xc4
};

#endif // TIMEDEVENT_H
