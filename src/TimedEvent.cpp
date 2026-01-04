#include "globals.h"
#include "TimedEvent.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "SC_OnScreenMessage.h"
#include "Message.h"
#include "string.h"
#include "SC_Question.h"


 
/* Function start: 0x401890 */
TimedEvent::TimedEvent()
{
    memset(this, 0, 10 * sizeof(int));
    m_timer.Reset();
}

/* Function start: 0x401910 */
TimedEvent::~TimedEvent()
{
    SC_Message* next = (SC_Message*)m_next_event_data;
    if (next) {
        delete next;
        m_next_event_data = 0;
    }
}

/* Function start: 0x4019A0 */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();
    TimedEventPool* pool;

    if (m_type == 0) {
        if (remaining_time > 0) {
            return 0;
        }

        void* next_data = m_next_event_data;
        if (next_data) {
            pool = DAT_00436988;
            TimedEvent* node = pool->Create((void*)pool->list.tail, 0);
            ((TimedEvent*)((int*)node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool->list.tail == 0) {
                pool->list.head = node;
            } else {
                ((TimedEventList**)&pool->list.tail)[0]->tail = node; // Wait, original code logic was *(int*)tail = node. Tail is a node pointer. 
                // Ah, pool list is a linked list. 
                // Previous code: *(int*)pool[1] = (int)node; 
                // pool[1] is tail. So it sets tail->next = node? 
                // Queue/List usage usually: tail->next = new_node.
                // But TimedEvent doesn't have a 'next' field at offset 0?
                // TimedEvent: field 0 is vtable.
                // Does it use vtable pointer as next pointer in the list? 
                // Or is the list node a wrapper?
                // Queue::Create returns a node.
                // TimedEventPool::Create seems to return a TimedEvent* but let's check.
                // The cast ((TimedEvent*)((int*)node + 2)) suggests 'node' is the container/wrapper, and +8 is the TimedEvent object.
            }
            // Let's check TimedEventPool::Create signature.
            // But first, let's replicate the exact memory operations using the struct.
            
            // pool[1] is tail.
            // *(int*)pool[1] means dereferencing tail pointer and writing to offset 0 of the tail object.
            // Offset 0 of TimedEvent is vtable... or if it's a wrapper node.
            
            // The code seems to assume the returned pointer 'node' is compatible with being stored in list.head/tail.
            
            if (pool->list.tail == 0) {
                pool->list.head = node;
            } else {
                // *(int*)pool[1] = (int)node; -> tail->val = node? No, *tail = node.
                // This implies offset 0 of the object pointed to by tail is a 'next' pointer.
                // For TimedEvent, offset 0 is vtable. Hacking vtable as 'next'?
                // Or maybe TimedEventPool uses a different node type for the list?
                // The list definition has `TimedEvent* head;`.
                
                // Let's stick to the pointer hacks but using proper types for 'pool'.
                 *(TimedEvent**)pool->list.tail = node;
            }
            pool->list.tail = node;
        }

        next_data = m_next_event_data;
        if (next_data) {
            delete (SC_Message*)next_data;
            m_next_event_data = 0;
        }
        return 1;
    } else if (m_type == 1) {
        if (remaining_time > 0) {
            return 0;
        }

        void* next_data = m_next_event_data;
        if (next_data) {
            pool = DAT_00436988;
            TimedEvent* node = pool->Create((void*)pool->list.tail, 0);
            ((TimedEvent*)((int*)node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool->list.tail == 0) {
                pool->list.head = node;
            } else {
               *(TimedEvent**)pool->list.tail = node;
            }
            pool->list.tail = node;
        }

        m_timer.Reset();
        return 0;
    } else if (m_type == 2) {
        sprintf(g_Buffer_00436960, "%3.3d : %2.2d", remaining_time / 60000, (remaining_time / 1000) % 60);
        g_SoundManager->ShowSubtitle(g_Buffer_00436960, 0x208, 0x1c2, 10000, 8);

        if (remaining_time > 0) {
            return 0;
        }

        void* next_data = m_next_event_data;
        if (next_data) {
            pool = DAT_00436988;
            TimedEvent* node = pool->Create((void*)pool->list.tail, 0);
            ((TimedEvent*)((int*)node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool->list.tail == 0) {
                pool->list.head = node;
            } else {
                *(TimedEvent**)pool->list.tail = node;
            }
            pool->list.tail = node;
        }

        next_data = m_next_event_data;
        if (next_data) {
            delete (SC_Message*)next_data;
            m_next_event_data = 0;
        }
        return 1;
    } else {
        ShowError("illegal type %d, TimedEvent::Update", m_type);
    }
    return 1;
}

