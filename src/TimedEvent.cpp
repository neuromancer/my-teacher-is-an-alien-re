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

extern int* DAT_00436988;
 
/* Function start: 0x401890 */
TimedEvent::TimedEvent()
{
    try {
        m_timer.Init();
        memset(this, 0, 10 * sizeof(int));
        m_timer.Reset();
    } catch (...) {
    }
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
}

/* Function start: 0x4019A0 */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();
    int* pool;

    if (m_type == 0) {
        if (remaining_time > 0) {
            return 0;
        }

        void* next_data = m_next_event_data;
        if (next_data) {
            pool = DAT_00436988;
            int* node = (int*)((TimedEventPool*)pool)->Create((void*)pool[1], 0);
            ((TimedEvent*)(node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool[1] == 0) {
                pool[0] = (int)node;
            } else {
                *(int*)pool[1] = (int)node;
            }
            pool[1] = (int)node;
        }

        next_data = m_next_event_data;
        if (next_data) {
            TimedEvent_dtor(next_data);
            FreeFromGlobalHeap(next_data);
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
            int* node = (int*)((TimedEventPool*)pool)->Create((void*)pool[1], 0);
            ((TimedEvent*)(node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool[1] == 0) {
                pool[0] = (int)node;
            } else {
                *(int*)pool[1] = (int)node;
            }
            pool[1] = (int)node;
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
            int* node = (int*)((TimedEventPool*)pool)->Create((void*)pool[1], 0);
            ((TimedEvent*)(node + 2))->CopyFrom((TimedEvent*)next_data);

            if (pool[1] == 0) {
                pool[0] = (int)node;
            } else {
                *(int*)pool[1] = (int)node;
            }
            pool[1] = (int)node;
        }

        next_data = m_next_event_data;
        if (next_data) {
            TimedEvent_dtor(next_data);
            FreeFromGlobalHeap(next_data);
            m_next_event_data = 0;
        }
        return 1;
    } else {
        ShowError("illegal type %d, TimedEvent::Update", m_type);
    }
    return 1;
}

