#include "EventList.h"
#include "string.h"
#include "Memory.h"

/* Function start: 0x4188D0 */
void EventList::InsertNode(void* data) {
    EventNode* newNode;
    EventNode* node;

    if (data == 0) {
        ShowError("queue fault 0102");
    }
    node = new EventNode;
    newNode = 0;
    if (node) {
        newNode = node;
        node->data = data;
        node->next = 0;
        node->prev = 0;
    }

    if (EventList::current == 0) {
        EventList::current = EventList::head;
    }
    if (EventList::head == 0) {
        EventList::head = newNode;
        EventList::tail = newNode;
        EventList::current = newNode;
    } else {
        newNode->prev = EventList::current;
        newNode->next = EventList::current->next;
        if (EventList::current->next == 0) {
            EventList::head = newNode;
            EventList::current->next = newNode;
        } else {
            EventList::current->next->prev = newNode;
            EventList::current->next = newNode;
        }
    }
}
