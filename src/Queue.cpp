#include "Queue.h"
#include "Memory.h"
#include "string.h"
#include "TimedEvent.h"
#include "Message.h"
#include "SC_Question.h"

/* Function start: 0x40D2A0 */
TimedEventPool::~TimedEventPool()
{
    TimedEvent* node;
    SC_Message* msg;
    int counter;
    int tmp;
    TimedEvent* poolBlock;
    TimedEvent* nextBlock;
    
    // Iterate through active events list (linked at offset 0)
    node = list.head;
    if (node != 0) {
        do {
            // Get embedded SC_Message at offset +8
            msg = (SC_Message*)((int*)node + 2);
            counter = 0;
            do {
                // Call SC_Message destructor
                msg->~SC_Message();
                msg = (SC_Message*)((char*)msg + 0xc0);
                tmp = counter;
                counter--;
            } while (tmp != 0);
            node = *(TimedEvent**)node;
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
            nextBlock = *(TimedEvent**)poolBlock;
            FreeMemory(poolBlock);
            poolBlock = nextBlock;
        } while (poolBlock != 0);
    }
    m_pool = 0;
}

/* Function start: 0x402310 */
void TimedEvent::CopyFrom(const TimedEvent* other)
{
    unsigned int eax;
    char* src = (char*)other;
    char* dst = (char*)this;
    char bl;

    field_0x8 = other->field_0x8;
    m_duration = other->m_duration;
    eax = 0;
    do {
        bl = src[0x10 + eax];
        eax++;
        dst[0xf + eax] = bl;
    } while (eax < 0x20);

    field_0x30 = other->field_0x30;
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

    field_0x80 = other->field_0x80;
    targetAddress = other->targetAddress;
    sourceAddress = other->sourceAddress;
    command = other->command;
    data = other->data;
    priority = other->priority;
    param1 = other->param1;
    param2 = other->param2;
    clickX = other->clickX;
    clickY = other->clickY;
    mouseX = other->mouseX;
    mouseY = other->mouseY;
    field_0xb4 = other->field_0xb4;
    field_0xb8 = other->field_0xb8;
    userPtr = other->userPtr;
}

/* Function start: 0x402420 */
TimedEvent* TimedEventPool::Create(void* callback, void* data)
{
    if (m_free_list == 0) {
        int* new_pool = (int*)AllocateMemory(m_pool_size * 200 + 4);
        *new_pool = (int)m_pool;
        int count = m_pool_size;
        m_pool = (TimedEvent*)new_pool;

        int* current = new_pool + count * 50 - 49;
        count--;
        if (count >= 0) {
            do {
                count--;
                *current = (int)m_free_list;
                m_free_list = (TimedEvent*)current;
                current = current - 50;
            } while (count >= 0);
        }
    }

    TimedEvent* event = m_free_list;
    Message* ebx = (Message*)((int*)event + 2);
    m_free_list = *(TimedEvent**)event;
    *(void**)((char*)event + 4) = callback;
    *(void**)event = data;
    m_count++;

    int edi = 0;
    if (ebx == 0) goto done;
loop:
    {
        SC_Message* tmp = (SC_Message*)ebx;
        tmp->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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

/* Function start: 0x417C50 */
SC_Message* TimedEventPool::PopSafe(SC_Message* buffer)
{
    Pop(buffer);
    return buffer;
}

/* Function start: 0x4185C0 */
SC_Message* TimedEventPool::Pop(SC_Message* buffer)
{
    // Local variables matching the assembly stack layout
    TimedEventPool* local_14;     // [EBP-0x10] - this pointer
    TimedEvent* local_18;         // [EBP-0x14] - head node pointer
    int local_1c;                 // [EBP-0x18] - success flag
    
    // Local SC_Message on stack for intermediate copy
    // This will generate SEH due to stack object with destructor
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    local_14 = this;
    local_18 = list.head;           // ECX = *this
    local_1c = 0;                          // EDX = 0
    
    // EBX = &head->field_0x8 = SC_Message* in the node (at offset 8 from head)
    SC_Message* srcMsg = (SC_Message*)((char*)local_18 + 8);
    
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
    local_d8.clickX = srcMsg->clickX;
    local_d8.clickY = srcMsg->clickY;
    local_d8.mouseX = srcMsg->mouseX;
    local_d8.mouseY = srcMsg->mouseY;
    local_d8.field_b4 = srcMsg->field_b4;
    local_d8.field_b8 = srcMsg->field_b8;
    local_d8.userPtr = srcMsg->userPtr;

    // Update list head: list.head = head->next
    TimedEvent* nextNode = *(TimedEvent**)local_18;
    local_14->list.head = nextNode;
    
    if (nextNode != 0) {
        // Clear prev pointer of new head
        *(int*)((char*)nextNode + 4) = 0;
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
    *(TimedEvent**)local_18 = local_14->m_free_list;
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
    buffer->clickX = local_d8.clickX;
    buffer->clickY = local_d8.clickY;
    buffer->mouseX = local_d8.mouseX;
    buffer->mouseY = local_d8.mouseY;
    buffer->field_b4 = local_d8.field_b4;
    buffer->field_b8 = local_d8.field_b8;
    buffer->userPtr = local_d8.userPtr;
    
    local_1c |= 1;
    
    return buffer;
}


/* Function start: 0x4024B0 */
void Queue::Insert(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }

    QueueNode* newNode = (QueueNode*)AllocateMemory(sizeof(QueueNode));
    QueueNode* node = 0;
    if (newNode != 0) {
        newNode->data = data;
        newNode->prev = 0;
        newNode->next = 0;
        node = newNode;
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
    if (newNode == 0) {
        node = 0;
    }
    else {
        newNode->data = data;
        newNode->prev = 0;
        newNode->next = 0;
        node = newNode;
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

/* Function start: 0x417680 */
void* Queue::GetCurrentData()
{
    QueueNode* node = (QueueNode*)m_current;
    if (node != 0) {
        return node->data;
    }
    return 0;
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
