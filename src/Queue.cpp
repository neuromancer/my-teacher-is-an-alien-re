#include "Queue.h"

extern "C" {
    void ShowError(const char*);
    void* AllocateMemory_Wrapper(int);
}

struct QueueNode {
    QueueNode* next;
    QueueNode* prev;
    void* data;
};

/* Function start: 0x4024D0 */
void Queue::Insert(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }

    QueueNode* newNode = (QueueNode*)AllocateMemory_Wrapper(sizeof(QueueNode));
    if (newNode) {
        newNode->data = data;
        newNode->next = 0;
        newNode->prev = 0;
    }

    if (m_head == 0) {
        m_head = newNode;
        m_tail = newNode;
        m_current = newNode;
    }
    else {
        newNode->prev = (QueueNode*)m_current;
        newNode->next = ((QueueNode*)m_current)->next;
        if (((QueueNode*)m_current)->next == 0) {
            m_tail = newNode;
        }
        else {
            ((QueueNode*)((QueueNode*)m_current)->next)->prev = newNode;
        }
        ((QueueNode*)m_current)->next = newNode;
        m_current = newNode;
    }
}
