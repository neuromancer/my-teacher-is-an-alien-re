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
    MessageNode* node;
    void* result;
    int zero;

    node = (MessageNode*)current;
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

    if (((MessageNode*)current)->next != 0) {
        ((MessageNode*)current)->next->prev = ((MessageNode*)current)->prev;
    }

    node = (MessageNode*)current;
    result = 0;
    if (node != 0) {
        result = node->data;
    }

    if (node != 0) {
        node->Destroy(1);
        current = 0;
    }

    current = head;
    return result;
}



/* Function start: 0x40C5B0 */
MessageNode* MessageNode::Init(void* nodeData)
{
    MessageNode::prev = 0;
    MessageNode::next = 0;
    MessageNode::data = nodeData;
    return this;
}
