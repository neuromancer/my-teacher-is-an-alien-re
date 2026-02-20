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

    int* srcPair = (int*)&other->field_0x38;
    int* dstPair = (int*)&field_0x38;
    dstPair[0] = srcPair[0];
    dstPair[1] = srcPair[1];

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
    int counter;
    SC_Message* msg;
    int tmp;

    // Iterate through active events list (linked via next pointer)
    PooledEvent* node = list.head;
    while (node != 0) {
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
        node = node->next;
    }

    // Clear pool state fields
    m_count = 0;
    m_free_list = 0;
    list.tail = 0;
    list.head = 0;

    // Free pool memory blocks (linked via first dword as next pointer)
    for (PooledEvent* poolBlock = m_pool; poolBlock != 0; ) {
        PooledEvent* nextBlock = poolBlock->next;
        FreeMemory(poolBlock);
        poolBlock = nextBlock;
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
    MousePoint completed;
    completed.x = 0;
    Pop(buffer);
    completed.x |= 1;
    return buffer;
}

/* Function start: 0x4185C0 */
SC_Message* TimedEventPool::Pop(SC_Message* buffer)
{
    TimedEventPool* pool;
    PooledEvent* headNode;
    volatile int completed;

    pool = this;
    headNode = (PooledEvent*)list.head;
    completed = 0;

    SC_Message* srcMsg = headNode->GetEmbeddedSCMessage();
    SC_Message local_msg = *srcMsg;

    PooledEvent* nextNode = headNode->next;
    pool->list.head = nextNode;

    if (nextNode != 0) {
        nextNode->prev = 0;
    } else {
        pool->list.tail = 0;
    }

    int counter = 0;
    do {
        srcMsg->~SC_Message();
        srcMsg = (SC_Message*)((char*)srcMsg + 0xc0);
        int tmp = counter;
        counter--;
        if (tmp == 0) break;
    } while (1);

    headNode->next = pool->m_free_list;
    pool->m_free_list = headNode;
    pool->m_count--;

    buffer->m_subObject = ((Parser*)&local_msg)->m_subObject;
    buffer->isProcessingKey = ((Parser*)&local_msg)->isProcessingKey;
    memcpy(buffer->currentKey, ((Parser*)&local_msg)->currentKey, 0x20);
    buffer->lineNumber = ((Parser*)&local_msg)->lineNumber;

    int* srcPair = (int*)&local_msg.savedFilePos;
    int* dstPair = (int*)&buffer->savedFilePos;
    dstPair[0] = srcPair[0];
    dstPair[1] = srcPair[1];

    memcpy(buffer->filename, ((Parser*)&local_msg)->filename, 0x40);

    buffer->pFile = ((Parser*)&local_msg)->pFile;
    buffer->targetAddress = local_msg.targetAddress;
    buffer->sourceAddress = local_msg.sourceAddress;
    buffer->command = local_msg.command;
    buffer->data = local_msg.data;
    buffer->priority = local_msg.priority;
    buffer->param1 = local_msg.param1;
    buffer->param2 = local_msg.param2;

    int* srcClick = (int*)&local_msg.clickPos;
    int* dstClick = (int*)&buffer->clickPos;
    dstClick[0] = srcClick[0];
    dstClick[1] = srcClick[1];

    buffer->mouseX = local_msg.mouseX;
    buffer->mouseY = local_msg.mouseY;
    buffer->field_b4 = local_msg.field_b4;
    completed |= 1;
    buffer->field_b8 = local_msg.field_b8;
    buffer->userPtr = local_msg.userPtr;

    return buffer;
}
