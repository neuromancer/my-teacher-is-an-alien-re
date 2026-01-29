#include "EventList.h"
#include "string.h"
#include "Memory.h"

/* Function start: 0x4188D0 */
void EventList::InsertNode(void* data) {
    EventNode* newNode;

    if (data == 0) {
        ShowError("queue fault 0102");
    }
    newNode = new EventNode(data);

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
        if (EventList::current->next != 0) {
            EventList::current->next->prev = newNode;
            EventList::current->next = newNode;
        } else {
            EventList::head = newNode;
            EventList::current->next = newNode;
        }
    }
}
