#include "Queue.h"
#include "Memory.h"
#include "string.h"
#include <memory.h>
#include "TimedEvent.h"
#include "Message.h"
#include "SC_Question.h"

/* Function start: 0x402310 */
PooledEvent* PooledEvent::CopyFrom(const PooledEvent* other)
{
    unsigned int eax;

    field_0x8 = other->field_0x8;
    eax = 0;
    m_duration = other->m_duration;
    do {
        data_0x10[eax] = other->data_0x10[eax];
        eax++;
    } while (eax < 0x20);

    field_0x30 = other->field_0x30;
    {
        int edi = other->field_0x38;
        int ebx = other->field_0x3c;
        field_0x38 = edi;
        field_0x3c = ebx;
    }
    eax = 0;
    do {
        m_data_0x40[eax] = other->m_data_0x40[eax];
        eax++;
    } while (eax < 0x40);

    field_0x80 = other->field_0x80;
    targetAddress = other->targetAddress;
    sourceAddress = other->sourceAddress;
    command = other->command;
    data = other->data;
    priority = other->priority;
    param1 = other->param1;
    param2 = other->param2;
    clickPos.x = other->clickPos.x;
    clickPos.y = other->clickPos.y;
    mouseX = other->mouseX;
    mouseY = other->mouseY;
    field_0xb4 = other->field_0xb4;
    field_0xb8 = other->field_0xb8;
    userPtr = other->userPtr;
    return this;
}

/* Function start: 0x402420 */
PooledEvent* TimedEventPool::Create(void* callback, void* data)
{
    if (m_free_list == 0) {
        int* new_pool = (int*)new char[m_pool_size * 200 + 4];
        *new_pool = (int)m_pool;
        int count = m_pool_size;
        m_pool = (PooledEvent*)new_pool;

        int* current = new_pool + count * 50 - 49;
        count--;
        if (count >= 0) {
            do {
                count--;
                *current = (int)m_free_list;
                m_free_list = (PooledEvent*)current;
                current = current - 50;
            } while (count >= 0);
        }
    }

    PooledEvent* esi = m_free_list;
    void* ecx_param = callback;
    void* edx_param = data;
    PooledEvent* eax = esi->next;
    SC_Message* ebx = (SC_Message*)&esi->field_0x8;
    m_free_list = eax;
    esi->prev = (PooledEvent*)ecx_param;
    esi->next = (PooledEvent*)edx_param;
    m_count++;
    memset(ebx, 0, 0xC0);

    int edi = 0;
loop:
    if (ebx == 0) goto done;
    ebx->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
done:
    ebx = (SC_Message*)((char*)ebx + 0xc0);
    {
        int tmp = edi;
        edi--;
        if (tmp != 0) goto loop;
    }
    return esi;
}

/* Function start: 0x4024D0 */
void Queue::Insert(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }

    QueueNode* newNode = new QueueNode(data);
    QueueNode* node = 0;
    if (newNode != 0) {
        node = newNode;
    }

    if (current == 0) {
        current = head;
    }

    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    }
    else {
        node->next = (QueueNode*)current;
        node->prev = ((QueueNode*)current)->prev;
        if (((QueueNode*)current)->prev == 0) {
            head = node;
        }
        else {
            ((QueueNode*)((QueueNode*)current)->prev)->next = node;
        }
        ((QueueNode*)current)->prev = node;
    }
}

/* Function start: 0x4025A0 */
void Queue::Push(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0112");
    }

    QueueNode* newNode = new QueueNode(data);
    QueueNode* node;
    if (newNode == 0) {
        node = 0;
    }
    else {
        node = newNode;
    }

    if (current == 0) {
        current = tail;
    }

    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    }
    else {
        if (tail == 0 || ((QueueNode*)tail)->next != 0) {
            ShowError("queue fault 0113");
        }
        node->next = 0;
        node->prev = (QueueNode*)tail;
        ((QueueNode*)tail)->next = node;
        tail = node;
    }
}

/* Function start: 0x402680 */
void* Queue::Pop()
{
    QueueNode* node = (QueueNode*)current;
    if (node == 0) {
        return 0;
    }

    if (head == node) {
        head = node->next;
    }
    if (tail == node) {
        tail = node->prev;
    }

    if (node->prev != 0) {
        node->prev->next = node->next;
    }

    QueueNode* nextNode = ((QueueNode*)current)->next;
    if (nextNode != 0) {
        nextNode->prev = ((QueueNode*)current)->prev;
    }

    QueueNode* curr = (QueueNode*)current;
    void* data = 0;
    if (curr != 0) {
        data = curr->data;
        curr->data = 0;
        curr->prev = 0;
        curr->next = 0;
        delete curr;
        current = 0;
    }

    current = head;
    return data;
}

/* Function start: 0x408790 */
void Queue::InsertAtCurrent(void* data)
{
    QueueNode* node;

    if (data == 0) {
        ShowError("queue fault 0102");
    }

    node = new QueueNode(data);

    if (current == 0) {
        current = head;
    }

    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    }
    else {
        node->next = (QueueNode*)current;
        node->prev = ((QueueNode*)current)->prev;
        if (((QueueNode*)current)->prev == 0) {
            head = node;
        }
        else {
            ((QueueNode*)((QueueNode*)current)->prev)->next = node;
        }
        ((QueueNode*)current)->prev = node;
    }
}

/* Function start: 0x40D2A0 */
TimedEventPool::~TimedEventPool()
{
    PooledEvent* node;
    SC_Message* msg;
    int counter;
    int tmp;
    PooledEvent* poolBlock;
    PooledEvent* nextBlock;

    // Iterate through active events list (linked via next pointer)
    node = list.head;
    if (node != 0) {
        do {
            // Get embedded SC_Message at offset +8 (field_0x8)
            msg = (SC_Message*)&node->field_0x8;
            counter = 0;
            do {
                // Call SC_Message destructor
                msg->~SC_Message();
                msg = (SC_Message*)((char*)msg + 0xc0);
                tmp = counter;
                counter--;
            } while (tmp != 0);
            node = (PooledEvent*)node->next;
        } while (node != 0);
    }

    // Clear pool state fields
    m_count = 0;
    m_free_list = 0;
    list.tail = 0;
    list.head = 0;

    // Free pool memory blocks (linked at offset 0x10)
    poolBlock = m_pool;
    if (poolBlock != 0) {
        do {
            nextBlock = (PooledEvent*)poolBlock->next;
            delete poolBlock;
            poolBlock = nextBlock;
        } while (poolBlock != 0);
    }
    m_pool = 0;
}

/* Function start: 0x417680 */
void* Queue::GetCurrentData()
{
    QueueNode* node = (QueueNode*)current;
    if (node != 0) {
        return node->data;
    }
    return 0;
}

/* Function start: 0x417C50 */
SC_Message* TimedEventPool::PopSafe(SC_Message* buffer)
{
    volatile int local_10 = 0;

    try {
        Pop(buffer);
    }
    catch (...) {
    }
    local_10 |= 1;
    return buffer;
}

/* Function start: 0x4185C0 */
SC_Message* TimedEventPool::Pop(SC_Message* buffer)
{
    // Local variables matching the assembly stack layout
    TimedEventPool* local_14;     // [EBP-0x10] - this pointer
    PooledEvent* local_18;        // [EBP-0x14] - head node pointer
    int local_1c;                 // [EBP-0x18] - success flag
    
    // Local SC_Message on stack for intermediate copy
    // This will generate SEH due to stack object with destructor
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    local_14 = this;
    local_18 = list.head;           // ECX = *this
    local_1c = 0;                          // EDX = 0
    
    // EBX = &head->field_0x8 = SC_Message* in the node (at offset 8 from head)
    SC_Message* srcMsg = (SC_Message*)&local_18->field_0x8;
    
    // Copy from srcMsg to local_d8 using field access
    local_d8.m_subObject = srcMsg->m_subObject;
    local_d8.isProcessingKey = srcMsg->isProcessingKey;
    
    // Copy currentKey[32]
    unsigned int idx = 0;
    do {
        local_d8.currentKey[idx] = srcMsg->currentKey[idx];
        idx++;
    } while (idx < 0x20);
    
    local_d8.lineNumber = srcMsg->lineNumber;
    local_d8.savedFilePos = srcMsg->savedFilePos;
    local_d8.field_0x3c = srcMsg->field_0x3c;
    
    // Copy filename[64]
    idx = 0;
    do {
        local_d8.filename[idx] = srcMsg->filename[idx];
        idx++;
    } while (idx < 0x40);
    
    local_d8.pFile = srcMsg->pFile;
    
    local_d8.targetAddress = srcMsg->targetAddress;
    local_d8.sourceAddress = srcMsg->sourceAddress;
    local_d8.command = srcMsg->command;
    local_d8.data = srcMsg->data;
    local_d8.priority = srcMsg->priority;
    local_d8.param1 = srcMsg->param1;
    local_d8.param2 = srcMsg->param2;
    local_d8.clickPos.x = srcMsg->clickPos.x;
    local_d8.clickPos.y = srcMsg->clickPos.y;
    local_d8.mouseX = srcMsg->mouseX;
    local_d8.mouseY = srcMsg->mouseY;
    local_d8.field_b4 = srcMsg->field_b4;
    local_d8.field_b8 = srcMsg->field_b8;
    local_d8.userPtr = srcMsg->userPtr;

    // Update list head: list.head = head->next
    PooledEvent* nextNode = (PooledEvent*)local_18->next;
    local_14->list.head = nextNode;

    if (nextNode != 0) {
        // Clear prev pointer of new head
        nextNode->prev = 0;
    } else {
        // List is now empty, clear tail
        local_14->list.tail = 0;
    }

    // Call destructor on source SC_Message
    int counter = 0;
    do {
        srcMsg->~SC_Message();
        srcMsg = (SC_Message*)((char*)srcMsg + 0xc0);
        int tmp = counter;
        counter--;
        if (tmp == 0) break;
    } while (1);

    // Add node to free list
    local_18->next = local_14->m_free_list;
    local_14->m_free_list = local_18;
    
    // Decrement count
    local_14->m_count--;
    
    // Copy from local_d8 to output buffer using field access
    buffer->m_subObject = local_d8.m_subObject;
    buffer->isProcessingKey = local_d8.isProcessingKey;
    
    idx = 0;
    do {
        buffer->currentKey[idx] = local_d8.currentKey[idx];
        idx++;
    } while (idx < 0x20);
    
    buffer->lineNumber = local_d8.lineNumber;
    buffer->savedFilePos = local_d8.savedFilePos;
    buffer->field_0x3c = local_d8.field_0x3c;
    
    idx = 0;
    do {
        buffer->filename[idx] = local_d8.filename[idx];
        idx++;
    } while (idx < 0x40);
    
    buffer->pFile = local_d8.pFile;
    
    buffer->targetAddress = local_d8.targetAddress;
    buffer->sourceAddress = local_d8.sourceAddress;
    buffer->command = local_d8.command;
    buffer->data = local_d8.data;
    buffer->priority = local_d8.priority;
    buffer->param1 = local_d8.param1;
    buffer->param2 = local_d8.param2;
    buffer->clickPos.x = local_d8.clickPos.x;
    buffer->clickPos.y = local_d8.clickPos.y;
    buffer->mouseX = local_d8.mouseX;
    buffer->mouseY = local_d8.mouseY;
    buffer->field_b4 = local_d8.field_b4;
    buffer->field_b8 = local_d8.field_b8;
    buffer->userPtr = local_d8.userPtr;
    
    local_1c |= 1;
    
    return buffer;
}
