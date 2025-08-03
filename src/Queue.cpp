#include "Queue.h"
#include "Memory.h"

/* Function start: 0x000000 */
Queue::Queue()
{
    m_head = 0;
    m_tail = 0;
    m_current = 0;
    m_field_0xc = 0;
}

extern "C" {
    void ShowError(const char*);
    void FreeFromGlobalHeap(void*);
}


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

/* Function start: 0x4025A0 */
void Queue::Push(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0112");
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
        if (m_tail == 0 || ((QueueNode*)m_tail)->next != 0) {
            ShowError("queue fault 0113");
        }
        newNode->prev = (QueueNode*)m_tail;
        ((QueueNode*)m_tail)->next = newNode;
        m_tail = newNode;
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

    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    void* data = node->data;
    FreeFromGlobalHeap(node);

    m_current = m_head;

    return data;
}

/* Function start: 0x402700 */
void Queue::Destroy(int free_memory)
{
    m_head = 0;
    m_tail = 0;
    m_current = 0;
    if (free_memory) {
        FreeFromGlobalHeap(this);
    }
}
