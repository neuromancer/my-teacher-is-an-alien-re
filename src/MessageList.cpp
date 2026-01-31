#include "SC_OnScreenMessage.h"
#include "SoundItem.h"
#include "Memory.h"
#include "string.h"


/* Function start: 0x40C430 */
void MessageList::InsertBeforeCurrent(void* data)
{
    LinkedList::InsertNode(data);
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
        delete node;
        current = 0;
    }

    current = head;
    return result;
}



/* Function start: 0x40C580 */
void* MessageList::GetCurrentData()
{
    return LinkedList::GetCurrentData();
}

/* Function start: 0x40C5B0 */
MessageNode* MessageNode::Init(void* nodeData)
{
    MessageNode::prev = 0;
    MessageNode::next = 0;
    MessageNode::data = nodeData;
    return this;
}
