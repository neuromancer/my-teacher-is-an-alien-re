#include "Queue.h"
#include "Memory.h"
#include "string.h"
extern "C" void WriteToLog(const char* format, ...);
#include <memory.h>
#include <new.h>
#include "TimedEvent.h"
#include "Message.h"
#include "SC_Question.h"
#include "SpriteAction.h"

/* Function start: 0x404E70 */
void Queue::Insert(void* data)
{
    LinkedList::InsertNode(data);
}

/* Function start: 0x406CC0 */
void Queue::Add(void* data)
{
    if (data == 0) ShowError("queue fault 0101");
    current = head;
    if (type == 1 || type == 2) {
        if (head != 0) {
            do {
                ListNode* cur = current;
                if (*(int*)(cur->data) < *(int*)data) {
                    InsertNode(data);
                    return;
                }
                if (tail == cur) {
                    PushNode(data);
                    return;
                }
                if (cur != 0) {
                    current = cur->next;
                }
            } while (current != 0);
            return;
        }
        InsertNode(data);
    } else {
        InsertNode(data);
    }
}

/* Function start: 0x4070A0 */
void Queue::Push(void* data)
{
    LinkedList::PushNode(data);
}

/* Function start: 0x423550 */
void* Queue::Pop()
{
    return LinkedList::RemoveCurrent();
}

/* Function start: 0x406FD0 */
void Queue::InsertAtCurrent(void* data)
{
    InsertNode(data);
}

/* Function start: 0x42D2D0 */ /* ~98% match */
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

/* Function start: 0x448C50 */
void* Queue::GetCurrentData()
{
    return LinkedList::GetCurrentData();
}

/* Function start: 0x42CAB0 */ /* ~95% match */
SC_Message* TimedEventPool::PopSafe(SC_Message* buffer)
{
    MousePoint completed;
    completed.x = 0;
    Pop(buffer);
    completed.x |= 1;
    return buffer;
}

/* Function start: 0x42CAB0 */
SpriteAction* TimedEventPool::PopSafe(SpriteAction* buffer)
{
    int completed;

    completed = 0;
    Pop(buffer);
    completed |= 1;
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

    int* srcClick = &local_msg.clickX;
    int* dstClick = &buffer->clickX;
    dstClick[0] = srcClick[0];
    dstClick[1] = srcClick[1];

    buffer->mouseX = local_msg.mouseX;
    buffer->mouseY = local_msg.mouseY;
    buffer->lastKey = local_msg.lastKey;
    completed |= 1;
    buffer->time = local_msg.time;
    buffer->userPtr = local_msg.userPtr;

    return buffer;
}

/* Function start: 0x42D1A0 */
SpriteAction* TimedEventPool::Pop(SpriteAction* buffer)
{
    volatile int completed;
    PooledEvent* headNode;
    SpriteAction localAction;

    completed = 0;
    headNode = list.head;
    localAction.CopyFrom((SpriteAction*)((int*)headNode + 2));

    PooledEvent* nextNode = headNode->next;
    list.head = nextNode;
    if (nextNode == 0) {
        list.tail = 0;
    }
    else {
        nextNode->prev = 0;
    }

    int counter = 0;
    SpriteAction* ebx = (SpriteAction*)((int*)headNode + 2);
    do {
        ebx->~SpriteAction();
        ebx = (SpriteAction*)((char*)ebx + 0x38);
        int tmp = counter;
        counter--;
        if (tmp == 0) break;
    } while (1);

    headNode->next = m_free_list;
    m_free_list = headNode;
    m_count--;

    buffer->mousePos.x = 0;
    buffer->mousePos.y = 0;
    memset(buffer, 0, sizeof(SpriteAction));
    buffer->CopyFrom(&localAction);
    completed |= 1;

    return buffer;
}

/* Function start: 0x4406F0 */
void PriorityQueue::AddAfterCurrent(void* data)
{
    LinkedList::InsertNode(data);
}
