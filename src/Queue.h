#ifndef QUEUE_H
#define QUEUE_H

struct QueueNode {
    QueueNode* prev;  // 0x0
    QueueNode* next;  // 0x4
    void* data;       // 0x8
};

class Queue {
public:
    Queue() {
        m_flags = 0;
        m_head = 0;
        m_tail = 0;
        m_current = 0;
        m_field_0xc = 0;
    }
    void* Destroy(int free_memory);

    void Insert(void* data);
    void* Pop();
    void* GetCurrentData();
    void Push(void* data);

public:
    unsigned int m_flags;  // 0x00 - flags (bit 2 = jitter mode)
    void* m_head;          // 0x04
    void* m_tail;          // 0x08
    void* m_current;       // 0x0C
    int m_field_0xc;       // 0x10 - type/mode field
};

#endif // QUEUE_H
