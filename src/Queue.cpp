#include "Queue.h"
#include "Memory.h"
#include "string.h"
#include "TimedEvent.h"
#include "Message.h"

extern "C" void __fastcall FUN_0040d2a0(void* thisptr);

TimedEventPool::~TimedEventPool()
{
    FUN_0040d2a0(this);
}

/* Function start: 0x402310 */
void TimedEvent::CopyFrom(const TimedEvent* other)
{
    unsigned int eax;
    char* src = (char*)other;
    char* dst = (char*)this;
    char bl;

    this->field_0x8 = other->field_0x8;
    this->m_duration = other->m_duration;
    eax = 0;
    do {
        bl = src[0x10 + eax];
        eax++;
        dst[0xf + eax] = bl;
    } while (eax < 0x20);

    this->field_0x30 = other->field_0x30;
    {
        int* esi = (int*)(&dst[0x38]);
        int* ptr = (int*)(&src[0x38]);
        int edi = ptr[0];
        int ebx = ptr[1];
        esi[0] = edi;
        esi[1] = ebx;
    }
    eax = 0;
    do {
        bl = src[0x40 + eax];
        eax++;
        dst[0x3f + eax] = bl;
    } while (eax < 0x40);

    this->field_0x80 = other->field_0x80;
    this->targetAddress = other->targetAddress;
    this->sourceAddress = other->sourceAddress;
    this->command = other->command;
    this->data = other->data;
    this->priority = other->priority;
    this->param1 = other->param1;
    this->param2 = other->param2;
    this->clickX = other->clickX;
    this->clickY = other->clickY;
    this->mouseX = other->mouseX;
    this->mouseY = other->mouseY;
    this->field_0xb4 = other->field_0xb4;
    this->field_0xb8 = other->field_0xb8;
    this->userPtr = other->userPtr;
}

/* Function start: 0x402420 */
TimedEvent* TimedEventPool::Create(void* callback, void* data)
{
    if (this->m_free_list == 0) {
        int* new_pool = (int*)AllocateMemory(this->m_pool_size * 200 + 4);
        *new_pool = (int)this->m_pool;
        int count = this->m_pool_size;
        this->m_pool = (TimedEvent*)new_pool;

        int* current = new_pool + count * 50 - 49;
        count--;
        if (count >= 0) {
            do {
                count--;
                *current = (int)this->m_free_list;
                this->m_free_list = (TimedEvent*)current;
                current = current - 50;
            } while (count >= 0);
        }
    }

    TimedEvent* event = this->m_free_list;
    Message* ebx = (Message*)((int*)event + 2);
    this->m_free_list = *(TimedEvent**)event;
    *(void**)((char*)event + 4) = callback;
    *(void**)event = data;
    this->m_count++;

    int edi = 0;
    if (ebx == 0) goto done;
loop:
    {
        Message* tmp = ebx;
        tmp->Message::Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    ebx = (Message*)((char*)ebx + 0xc0);
    {
        int eax = edi;
        edi--;
        if (eax != 0) goto loop;
    }
done:
    return event;
}

/* Function start: 0x4024B0 */
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
