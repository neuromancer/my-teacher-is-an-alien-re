#include "EventList.h"
#include "string.h"
#include "Memory.h"


/* Function start: 0x431C30 */
void EventList::InsertNode(void* data) {
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

/* Function start: 0x431B60 */
void* EventList::RemoveCurrent()
{
    ListNode* node;
    void* result;

    node = current;
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

    if (current->next != 0) {
        current->next->prev = current->prev;
    }

    node = current;
    result = (node != 0) ? node->data : 0;

    if (current != 0) {
        delete current;
        current = 0;
    }

    current = head;
    return result;
}

/* Function start: 0x431D00 */
ListNode* ListNode::Init(void* nodeData) {
    prev = 0;
    next = 0;
    data = nodeData;
    return this;
}
