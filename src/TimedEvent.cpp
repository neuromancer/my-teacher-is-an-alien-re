#include "TimedEvent.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"

extern "C" {
    void ShowError(const char*, ...);
    void TimedEvent_dtor(void*);
    void TimedEvent__CopyConstructor(void*, void*);
    void* TimedEvent__Create(void*, void*, int);
    void FUN_0041c000(void*, char*, int, int, int);
}

extern void* g_GameStruct2;
extern void* g_SoundManager;
extern char DAT_00436960[256];

const char* s_illegal_type__d__TimedEvent__Upd_004350ac = "illegal type %d, TimedEvent::Update";
const char* s__3_3d____2_2d_004350d0 = "%3.3d : %2.2d";

/* Function start: 0x402310 */
TimedEvent::TimedEvent(const TimedEvent& other)
{
    // This is a copy of the data, not a real copy constructor
    memcpy(&this->m_type, &other.m_type, sizeof(TimedEvent) - 4);
}

/* Function start: 0x402420 */
TimedEvent* TimedEvent::Create(TimedEventPool* pool, void* callback, void* data)
{
    if (pool->m_free_list == 0) {
        TimedEvent* new_pool = (TimedEvent*)AllocateMemory(pool->m_pool_size * 200 + 4);
        *(TimedEvent**)new_pool = pool->m_pool;
        pool->m_pool = new_pool;

        TimedEvent* current = (TimedEvent*)((char*)new_pool + pool->m_pool_size * 200);
        for (int i = 0; i < pool->m_pool_size; i++) {
            *(TimedEvent**)current = pool->m_free_list;
            pool->m_free_list = current;
            current = (TimedEvent*)((char*)current - 200);
        }
    }

    TimedEvent* new_event = pool->m_free_list;
    pool->m_free_list = *(TimedEvent**)new_event;

    *(void**)((char*)new_event + 4) = callback;
    *(void**)new_event = data;
    pool->m_count++;

    memset((char*)new_event + 8, 0, 0x30 * 4);

    for (int i = 0; i < 1; i++) {
        //Message_Constructor((char*)new_event + 8 + i * 0xc0, 0,0,0,0,0,0,0,0,0,0);
    }

    return new_event;
}

/* Function start: 0x4019A0 */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();
    if (m_type == 0) {
        if (remaining_time > 0) {
            return 0;
        }
        if (m_next_event_data != 0) {
            void* new_event = TimedEvent__Create(g_GameStruct2, *(void**)((char*)g_GameStruct2 + 4), 0);
            TimedEvent__CopyConstructor((char*)new_event + 8, m_next_event_data);
            if (*(void**)((char*)g_GameStruct2 + 4) == 0) {
                *(void**)g_GameStruct2 = new_event;
            } else {
                **(void***)((char*)g_GameStruct2 + 4) = new_event;
            }
            *(void**)((char*)g_GameStruct2 + 4) = new_event;
        }
        if (m_next_event_data != 0) {
            TimedEvent_dtor(m_next_event_data);
            FreeFromGlobalHeap(m_next_event_data);
            m_next_event_data = 0;
        }
        return 1;
    }
    if (m_type == 1) {
        if (remaining_time > 0) {
            return 0;
        }
        if (m_next_event_data != 0) {
            void* new_event = TimedEvent__Create(g_GameStruct2, *(void**)((char*)g_GameStruct2 + 4), 0);
            TimedEvent__CopyConstructor((char*)new_event + 8, m_next_event_data);
            if (*(void**)((char*)g_GameStruct2 + 4) == 0) {
                *(void**)g_GameStruct2 = new_event;
            } else {
                **(void***)((char*)g_GameStruct2 + 4) = new_event;
            }
            *(void**)((char*)g_GameStruct2 + 4) = new_event;
        }
        m_timer.Reset();
        return 0;
    }
    if (m_type != 2) {
        ShowError(s_illegal_type__d__TimedEvent__Upd_004350ac, m_type);
    }
    sprintf(DAT_00436960, s__3_3d____2_2d_004350d0, remaining_time / 60000, (remaining_time / 1000) % 60);
    FUN_0041c000(g_SoundManager, DAT_00436960, 0x208, 0x1c2, 10000);
    if (remaining_time > 0) {
        return 0;
    }
    if (m_next_event_data != 0) {
        void* new_event = TimedEvent__Create(g_GameStruct2, *(void**)((char*)g_GameStruct2 + 4), 0);
        TimedEvent__CopyConstructor((char*)new_event + 8, m_next_event_data);
        if (*(void**)((char*)g_GameStruct2 + 4) == 0) {
            *(void**)g_GameStruct2 = new_event;
        } else {
            **(void***)((char*)g_GameStruct2 + 4) = new_event;
        }
        *(void**)((char*)g_GameStruct2 + 4) = new_event;
    }
    if (m_next_event_data != 0) {
        TimedEvent_dtor(m_next_event_data);
        FreeFromGlobalHeap(m_next_event_data);
        m_next_event_data = 0;
    }
    return 1;
}
