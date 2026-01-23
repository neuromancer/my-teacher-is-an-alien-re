#include "SC_OnScreenMessage.h"
#include "SoundItem.h"
#include "Memory.h"
#include "string.h"

/* Function start: 0x40C0D0 */
void* MessageList::GetCurrentData()
{
    MessageNode* curr;

    curr = (MessageNode*)MessageList::current;
    if (curr != 0) {
        return curr->data;
    }
    return 0;
}

/* Function start: 0x40C430 */
void MessageList::InsertBeforeCurrent(void* data)
{
    MessageNode* newNode;
    MessageNode* curr;
    void* allocResult;

    if (data == 0) {
        ShowError("queue fault 0102");
    }

    allocResult = AllocateMemory(0xc);
    newNode = (MessageNode*)0;
    if (allocResult != 0) {
        newNode = (MessageNode*)allocResult;
        newNode->data = data;
        newNode->prev = 0;
        newNode->next = 0;
    }

    if (MessageList::current == 0) {
        MessageList::current = MessageList::head;
    }

    if (MessageList::head == 0) {
        MessageList::head = newNode;
        MessageList::tail = newNode;
        MessageList::current = newNode;
    } else {
        curr = (MessageNode*)MessageList::current;
        newNode->next = curr;
        newNode->prev = curr->prev;
        if (curr->prev != 0) {
            curr->prev->next = newNode;
            curr->prev = newNode;
        } else {
            MessageList::head = newNode;
            curr->prev = newNode;
        }
    }
}

/* Function start: 0x40C500 */
void* MessageList::PopCurrent()
{
    MessageNode* curr;
    MessageNode* next;
    void* data;

    curr = (MessageNode*)MessageList::current;
    if (curr == 0) {
        return 0;
    }

    if (MessageList::head == curr) {
        MessageList::head = curr->next;
    }

    if (MessageList::tail == MessageList::current) {
        MessageList::tail = ((MessageNode*)MessageList::current)->prev;
    }

    curr = (MessageNode*)MessageList::current;
    if (curr->prev != 0) {
        curr->prev->next = curr->next;
    }

    curr = (MessageNode*)MessageList::current;
    next = curr->next;
    if (next != 0) {
        next->prev = curr->prev;
    }

    curr = (MessageNode*)MessageList::current;
    data = 0;
    if (curr != 0) {
        data = curr->data;
        curr->data = 0;
        curr->prev = 0;
        curr->next = 0;
        FreeMemory(curr);
        MessageList::current = 0;
    }

    MessageList::current = MessageList::head;
    return data;
}

/* Function start: 0x40C580 */
MessageNode* MessageNode::Destroy(int flag)
{
    MessageNode::data = 0;
    MessageNode::prev = 0;
    MessageNode::next = 0;
    if (flag & 1) {
        FreeMemory(this);
    }
    return this;
}

/* Function start: 0x40C5B0 */
MessageNode* MessageNode::Init(void* nodeData)
{
    MessageNode::prev = 0;
    MessageNode::next = 0;
    MessageNode::data = nodeData;
    return this;
}
