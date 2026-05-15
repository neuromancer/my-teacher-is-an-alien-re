#include "EventList.h"
#include "string.h"
#include "Memory.h"

extern "C" void ShowError(const char* format, ...);

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
    return LinkedList::RemoveCurrent();
}
