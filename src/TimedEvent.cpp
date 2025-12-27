#include "globals.h"
#include "TimedEvent.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "SC_OnScreenMessage.h"
#include "Message.h"
#include "string.h"

extern "C" {
    void __fastcall TimedEvent_dtor(void*);

}

extern void* g_GameStruct2;
//extern void* g_SoundManager;
 
/* Function start: 0x401890 */
TimedEvent::TimedEvent() : m_timer()
{
    memset(this, 0, 10 * sizeof(int));
    m_timer.Reset();
}

/* Function start: 0x401910 */
TimedEvent::~TimedEvent()
{
    void* next = m_next_event_data;
    if (next) {
        TimedEvent_dtor(next);
        FreeFromGlobalHeap(next);
        m_next_event_data = 0;
    }
    // The base class destructor will be called automatically
}

/* Function start: 0x4019A0 */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();
    TimedEventPool* pool = (TimedEventPool*)g_GameStruct2;

    if (m_type == 0) {
        if (remaining_time > 0) {
            return 0;
        }

        if (m_next_event_data) {
            TimedEvent *new_event = pool->Create((void*)pool->list.tail, 0);
            new_event->CopyFrom((TimedEvent*)m_next_event_data);

            if (pool->list.tail == NULL) {
                pool->list.head = new_event;
            } else {
                *(TimedEvent**)(pool->list.tail) = new_event;
            }
            pool->list.tail = new_event;
        }

        if (m_next_event_data) {
            ((TimedEvent*)m_next_event_data)->~TimedEvent();
            FreeFromGlobalHeap(m_next_event_data);
            m_next_event_data = 0;
        }
        return 1;
    } else if (m_type == 1) {
        if (remaining_time > 0) {
            return 0;
        }

        if (m_next_event_data) {
            TimedEvent *new_event = pool->Create((void*)pool->list.tail, 0);
            new_event->CopyFrom((TimedEvent*)m_next_event_data);

            if (pool->list.tail == NULL) {
                pool->list.head = new_event;
            } else {
                *(TimedEvent**)(pool->list.tail) = new_event;
            }
            pool->list.tail = new_event;
        }

        m_timer.Reset();
        return 0;
    } else if (m_type == 2) {
        sprintf(g_Buffer_00436960, "%3.3d : %2.2d", remaining_time / 60000, (remaining_time / 1000) % 60);
        g_SoundManager->ShowSubtitle(g_Buffer_00436960, 0x208, 0x1c2, 10000);

        if (remaining_time > 0) {
            return 0;
        }

        if (m_next_event_data) {
            TimedEvent *new_event = pool->Create((void*)pool->list.tail, 0);
            new_event->CopyFrom((TimedEvent*)m_next_event_data);

            if (pool->list.tail == NULL) {
                pool->list.head = new_event;
            } else {
                *(TimedEvent**)(pool->list.tail) = new_event;
            }
            pool->list.tail = new_event;
        }

        if (m_next_event_data) {
            ((TimedEvent*)m_next_event_data)->~TimedEvent();
            FreeFromGlobalHeap(m_next_event_data);
            m_next_event_data = 0;
        }
        return 1;
    } else {
        ShowError("illegal type %d, TimedEvent::Update", m_type);
    }
    return 0;
}
