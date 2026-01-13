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
        m_field_0xc = 0;
        m_head = 0;
        m_tail = 0;
        m_current = m_head;
    }
    void* Destroy(int free_memory);

    void Insert(void* data);
    void* Pop();
    void* GetCurrentData();
    void Push(void* data);

public:
    void* m_head;
    void* m_tail;
    void* m_current;
    int m_field_0xc;
};

#endif // QUEUE_H
