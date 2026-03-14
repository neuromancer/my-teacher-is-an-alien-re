#include "EventList.h"
#include "string.h"
#include "Memory.h"

// Demo had EventList::InsertNode (0x4188D0) as a non-inline wrapper
// around LinkedList::InsertNode. Full game inlines this call.

/* Function start: 0x431B60 */
void* EventList::RemoveCurrent()
{
    return LinkedList::RemoveCurrent();
}
