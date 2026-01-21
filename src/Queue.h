#ifndef QUEUE_H
#define QUEUE_H

struct QueueNode {
    QueueNode* prev;  // 0x0
    QueueNode* next;  // 0x4
    void* data;       // 0x8

    QueueNode(void* d) {
        data = d;
        prev = 0;
        next = 0;
    }
};

class Queue {
public:
    Queue() {
        m_head = 0;
        m_tail = 0;
        m_current = 0;
        m_field_0xc = 0;
    }
    void* Destroy(int free_memory);

    void Insert(void* data);
    void InsertAtCurrent(void* data);
    void* Pop();
    void* GetCurrentData();
    void Push(void* data);

public:
    void* m_head;          // 0x00
    void* m_tail;          // 0x04
    void* m_current;       // 0x08
    int m_field_0xc;       // 0x0c - type/mode field
};

#endif // QUEUE_H
