#include "Queue.h"
#include "Memory.h"
#include "string.h"

/* Function start: 0x4024D0 */
void Queue::Insert(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }

    QueueNode* newNode = (QueueNode*)AllocateMemory(sizeof(QueueNode));
    QueueNode* node = 0;
    try {
        if (newNode != 0) {
            newNode->data = data;
            newNode->prev = 0;
            newNode->next = 0;
            node = newNode;
        }
    } catch (...) {
    }

    if (m_current == 0) {
        m_current = m_head;
    }

    if (m_head == 0) {
        m_head = node;
        m_tail = node;
        m_current = node;
    }
    else {
        node->next = (QueueNode*)m_current;
        node->prev = ((QueueNode*)m_current)->prev;
        if (((QueueNode*)m_current)->prev == 0) {
            m_head = node;
        }
        else {
            ((QueueNode*)((QueueNode*)m_current)->prev)->next = node;
        }
        ((QueueNode*)m_current)->prev = node;
    }
}

/* Function start: 0x4025A0 */
void Queue::Push(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0112");
    }

    QueueNode* newNode = (QueueNode*)AllocateMemory(sizeof(QueueNode));
    QueueNode* node;
    try {
        if (newNode == 0) {
            node = 0;
        }
        else {
            newNode->data = data;
            newNode->prev = 0;
            newNode->next = 0;
            node = newNode;
        }
    } catch (...) {
    }

    if (m_current == 0) {
        m_current = m_tail;
    }

    if (m_head == 0) {
        m_head = node;
        m_tail = node;
        m_current = node;
    }
    else {
        if (m_tail == 0 || ((QueueNode*)m_tail)->next != 0) {
            ShowError("queue fault 0113");
        }
        node->next = 0;
        node->prev = (QueueNode*)m_tail;
        ((QueueNode*)m_tail)->next = node;
        m_tail = node;
    }
}

/* Function start: 0x402680 */
void* Queue::Pop()
{
    QueueNode* node = (QueueNode*)m_current;
    if (node == 0) {
        return 0;
    }

    if (m_head == node) {
        m_head = node->next;
    }
    if (m_tail == node) {
        m_tail = node->prev;
    }

    if (node->prev != 0) {
        node->prev->next = node->next;
    }

    QueueNode* nextNode = ((QueueNode*)m_current)->next;
    if (nextNode != 0) {
        nextNode->prev = ((QueueNode*)m_current)->prev;
    }

    QueueNode* curr = (QueueNode*)m_current;
    void* data = 0;
    if (curr != 0) {
        data = curr->data;
        curr->data = 0;
        curr->prev = 0;
        curr->next = 0;
        FreeMemory(curr);
        m_current = 0;
    }

    m_current = m_head;
    return data;
}

/* Function start: 0x402700 */
void* Queue::Destroy(int free_memory)
{
    m_current = 0;
    m_head = 0;
    m_tail = 0;
    if (free_memory & 1) {
        FreeMemory(this);
    }
    return this;
}
