#include "SC_OnScreenMessage.h"
#include "SoundItem.h"
#include "Memory.h"
#include "string.h"


/* Function start: 0x448F80 */
void MessageList::InsertBeforeCurrent(void* data)
{
    LinkedList::InsertNode(data);
}

/* Function start: 0x431B60 */
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



/* Function start: 0x4233A0 */
void MessageList::InsertNode(void* data) {
    if (data == 0) {
        ShowError("queue fault 0102");
    }
    MessageNode* newNode = new MessageNode(data);
    if (current == 0) {
        current = head;
    }
    if (head == 0) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        newNode->next = (MessageNode*)current;
        newNode->prev = ((MessageNode*)current)->prev;
        if (((MessageNode*)current)->prev == 0) {
            head = newNode;
        } else {
            ((MessageNode*)current)->prev->next = newNode;
        }
        ((MessageNode*)current)->prev = newNode;
    }
}
