#ifndef QUEUE_H
#define QUEUE_H

struct QueueNode {
    QueueNode* next;
    QueueNode* prev;
    void* data;
};

class Queue {
public:
    Queue();
    void Destroy(int free_memory);

    void Insert(void* data);
    void* Pop();
    void Push(void* data);

public:
    void* m_head;
    void* m_tail;
    void* m_current;
    int m_field_0xc;
};

#endif // QUEUE_H
