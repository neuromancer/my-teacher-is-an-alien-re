#include "Handler13.h"
#include "Memory.h"

// Data cleanup function for list node data
extern void __fastcall FUN_00401910(void*);

/* Function start: 0x401B60 */
Handler13::Handler13() {
    // Set handlerId to 13
    handlerId = 13;

    // Call Timer::Reset() on first timer
    timer1.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
}

/* Function start: 0x401CA0 */
Handler13::~Handler13() {
    MessageList* pList;
    MessageNode* node;
    void* data;

    pList = list;
    if (pList != 0) {
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    data = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == (MessageNode*)pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    // Get data and free node
                    node = (MessageNode*)pList->current;
                    data = 0;
                    if (node != 0) {
                        data = node->data;
                    }
                    if (node != 0) {
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        FreeMemory(node);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (data != 0) {
                    FUN_00401910(data);
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(pList);
        list = 0;
    }
}
