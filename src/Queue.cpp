#include "Queue.h"
#include "Memory.h"
#include "string.h"
#include <memory.h>
#include <new.h>
#include "TimedEvent.h"
#include "Message.h"
#include "SC_Question.h"
#include "SpriteAction.h"

/* Function start: 0x404E70 */
void Queue::Insert(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }
    ListNode* node = new ListNode(data);
    if (current == 0) current = head;
    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    } else {
        node->next = current;
        node->prev = current->prev;
        if (current->prev == 0) {
            head = node;
        } else {
            current->prev->next = node;
        }
        current->prev = node;
    }
}

/* Function start: 0x406CC0 */
void Queue::Add(void* data)
{
    if (data == 0) ShowError("queue fault 0101");
    current = head;
    if (type != 1 && type != 2) {
        // Non-sorted path: InsertNode inline
        if (data == 0) ShowError("queue fault 0102");
        ListNode* node = new ListNode(data);
        if (current == 0) current = head;
        if (head == 0) {
            head = node;
            tail = node;
            current = node;
        } else {
            node->next = current;
            node->prev = current->prev;
            if (current->prev == 0) {
                head = node;
            } else {
                current->prev->next = node;
            }
            current->prev = node;
        }
        return;
    }
    if (head == 0) {
        // Sorted but empty: InsertNode inline
        if (data == 0) ShowError("queue fault 0102");
        ListNode* node = new ListNode(data);
        if (current == 0) current = head;
        if (head == 0) {
            head = node;
            tail = node;
            current = node;
        } else {
            node->next = current;
            node->prev = current->prev;
            if (current->prev == 0) {
                head = node;
            } else {
                current->prev->next = node;
            }
            current->prev = node;
        }
        return;
    }
    do {
        ListNode* cur = current;
        if (*(int*)(cur->data) < *(int*)data) {
            // InsertNode inline
            if (data == 0) ShowError("queue fault 0102");
            ListNode* node = new ListNode(data);
            if (current == 0) current = head;
            if (head == 0) {
                head = node;
                tail = node;
                current = node;
            } else {
                node->next = current;
                node->prev = current->prev;
                if (current->prev == 0) {
                    head = node;
                } else {
                    current->prev->next = node;
                }
                current->prev = node;
            }
            return;
        }
        if (tail == cur) {
            // PushNode inline
            if (data == 0) ShowError("queue fault 0112");
            ListNode* node = new ListNode(data);
            if (current == 0) current = tail;
            if (head == 0) {
                head = node;
                tail = node;
                current = node;
            } else {
                if (tail == 0 || tail->next != 0) {
                    ShowError("queue fault 0113");
                }
                node->next = 0;
                node->prev = tail;
                tail->next = node;
                tail = node;
            }
            return;
        }
        if (cur != 0) {
            current = cur->next;
        }
    } while (current != 0);
}

/* Function start: 0x4070A0 */
void Queue::Push(void* data)
{
    if (data == 0) ShowError("queue fault 0112");
    ListNode* node = new ListNode(data);
    if (current == 0) current = tail;
    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    } else {
        if (tail == 0 || tail->next != 0) {
            ShowError("queue fault 0113");
        }
        node->next = 0;
        node->prev = tail;
        tail->next = node;
        tail = node;
    }
}

/* Function start: 0x423550 */
void* Queue::Pop()
{
    return LinkedList::RemoveCurrent();
}

/* Function start: 0x406FD0 */
/* Function start: 0x4185C0 */
void Queue::InsertAtCurrent(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }
    ListNode* node = new ListNode(data);
    if (current == 0) current = head;
    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    } else {
        node->next = current;
        node->prev = current->prev;
        if (current->prev == 0) {
            head = node;
        } else {
            current->prev->next = node;
        }
        current->prev = node;
    }
}

/* Function start: 0x42D2D0 */
TimedEventPool::~TimedEventPool()
{
    // Iterate active-events list; each node holds one embedded SpriteAction at +0x8
    PooledEvent* node = list.head;
    while (node != 0) {
        SpriteAction* action = (SpriteAction*)&node->field_0x8;
        int counter = 0;
        int tmp;
        do {
            action->~SpriteAction();
            action = action + 1;
            tmp = counter;
            counter--;
        } while (tmp != 0);
        node = node->next;
    }

    m_count = 0;
    m_free_list = 0;
    list.tail = 0;
    list.head = 0;

    for (PooledEvent* poolBlock = m_pool; poolBlock != 0; ) {
        PooledEvent* nextBlock = poolBlock->next;
        FreeMemory(poolBlock);
        poolBlock = nextBlock;
    }
    m_pool = 0;
}

// Dead duplicate of MessageList::GetCurrentData.
void* Queue::GetCurrentData()
{
    return LinkedList::GetCurrentData();
}

/* Function start: 0x42CAB0 */
SpriteAction TimedEventPool::PopSafe()
{
    return Pop();
}

/* Function start: 0x42D1A0 */
SpriteAction TimedEventPool::Pop()
{
    PooledEvent* headNode = list.head;
    SpriteAction* src = (SpriteAction*)((int*)headNode + 2);
    SpriteAction localAction(*src);

    PooledEvent* nextNode = headNode->next;
    list.head = nextNode;
    if (nextNode != 0) {
        nextNode->prev = 0;
    }
    else {
        list.tail = 0;
    }

    int counter = 0;
    do {
        src->~SpriteAction();
        src = (SpriteAction*)((char*)src + 0x38);
        int tmp = counter;
        counter--;
        if (tmp == 0) break;
    } while (1);

    headNode->next = m_free_list;
    m_free_list = headNode;
    m_count--;

    return localAction;
}

/* Function start: 0x4406F0 */
void PriorityQueue::AddAfterCurrent(void* data)
{
    if (data == 0) {
        ShowError("queue fault 0102");
    }
    ListNode* node = new ListNode(data);
    if (current == 0) current = head;
    if (head == 0) {
        head = node;
        tail = node;
        current = node;
    } else {
        node->next = current;
        node->prev = current->prev;
        if (current->prev == 0) {
            head = node;
        } else {
            current->prev->next = node;
        }
        current->prev = node;
    }
}
