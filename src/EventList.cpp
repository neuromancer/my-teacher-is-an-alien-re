#include "EventList.h"
#include "string.h"
#include "Memory.h"

/* Function start: 0x431C30 */
void EventList::InsertNode(void* data) { LinkedList::InsertNode(data); }

/* Function start: 0x431B60 */
void* EventList::RemoveCurrent()
{
    return LinkedList::RemoveCurrent();
}
