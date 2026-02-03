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
    unsigned int i;

    field_0x8 = other->field_0x8;
    m_duration = other->m_duration;
    for (i = 0; i < 0x20; i++) {
        data_0x10[i] = other->data_0x10[i];
    }

    field_0x30 = other->field_0x30;
    field_0x38 = other->field_0x38;
    field_0x3c = other->field_0x3c;

    for (i = 0; i < 0x40; i++) {
        m_data_0x40[i] = other->m_data_0x40[i];
    }

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
    SC_Message* ebx = esi->GetEmbeddedSCMessage();
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
    LinkedList::InsertNode(data);
}

/* Function start: 0x4025A0 */
void Queue::Push(void* data)
{
    LinkedList::PushNode(data);
}

/* Function start: 0x402680 */
void* Queue::Pop()
{
    return LinkedList::RemoveCurrent();
}

/* Function start: 0x408790 */
void Queue::InsertAtCurrent(void* data)
{
    InsertNode(data);
}

/* Function start: 0x40D2A0 */
TimedEventPool::~TimedEventPool()
{
    SC_Message* msg;
    int counter;
    int tmp;
    PooledEvent* nextBlock;

    // Iterate through active events list (linked via next pointer)
    for (PooledEvent* node = list.head; node != 0; node = node->next) {
        // Get embedded SC_Message at offset +8
        msg = node->GetEmbeddedSCMessage();
        counter = 0;
        do {
            // Call SC_Message destructor
            msg->~SC_Message();
            msg = (SC_Message*)((char*)msg + 0xc0);
            tmp = counter;
            counter--;
        } while (tmp != 0);
    }

    // Clear pool state fields
    m_count = 0;
    m_free_list = 0;
    list.tail = 0;
    list.head = 0;

    // Free pool memory blocks (linked via next pointer)
    for (PooledEvent* poolBlock = m_pool; poolBlock != 0; poolBlock = nextBlock) {
        nextBlock = poolBlock->next;
        delete poolBlock;
    }
    m_pool = 0;
}

/* Function start: 0x417680 */
void* Queue::GetCurrentData()
{
    return LinkedList::GetCurrentData();
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
    PooledEvent* local_18;        // [EBP-0x14] - head node pointer
    int local_1c;                 // [EBP-0x18] - success flag
    
    // Local SC_Message on stack for intermediate copy
    // This will generate SEH due to stack object with destructor
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    local_14 = this;
    local_18 = list.head;           // ECX = *this
    local_1c = 0;                          // EDX = 0
    
    // Get embedded SC_Message at offset 8 from head
    SC_Message* srcMsg = local_18->GetEmbeddedSCMessage();
    
    // Copy from srcMsg to local_d8 using field access
    local_d8.m_subObject = srcMsg->m_subObject;
    local_d8.isProcessingKey = srcMsg->isProcessingKey;

    // Copy currentKey[32]
    unsigned int idx;
    for (idx = 0; idx < 0x20; idx++) {
        local_d8.currentKey[idx] = srcMsg->currentKey[idx];
    }

    local_d8.lineNumber = srcMsg->lineNumber;
    local_d8.savedFilePos = srcMsg->savedFilePos;
    local_d8.field_0x3c = srcMsg->field_0x3c;

    // Copy filename[64]
    for (idx = 0; idx < 0x40; idx++) {
        local_d8.filename[idx] = srcMsg->filename[idx];
    }
    
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
    PooledEvent* nextNode = local_18->next;
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

    for (idx = 0; idx < 0x20; idx++) {
        buffer->currentKey[idx] = local_d8.currentKey[idx];
    }

    buffer->lineNumber = local_d8.lineNumber;
    buffer->savedFilePos = local_d8.savedFilePos;
    buffer->field_0x3c = local_d8.field_0x3c;

    for (idx = 0; idx < 0x40; idx++) {
        buffer->filename[idx] = local_d8.filename[idx];
    }
    
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
