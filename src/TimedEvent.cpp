#include "TimedEvent.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "SC_OnScreenMessage.h"
#include "Message.h"

extern "C" {
    void ShowError(const char*, ...);
    void TimedEvent_dtor(void*);
    void FUN_0041c000(void*, char*, int, int, int);
}

extern void* g_GameStruct2;
extern void* g_SoundManager;
extern char DAT_00436960[256];

const char* s_illegal_type__d__TimedEvent__Upd_004350ac = "illegal type %d, TimedEvent::Update";
const char* s__3_3d____2_2d_004350d0 = "%3.3d : %2.2d";

/* Function start: 0x401890 */
TimedEvent::TimedEvent() : m_timer()
{
    memset(this, 0, 10 * sizeof(int));
    m_timer.Reset();
}


/* Function start: 0x402310 */
void TimedEvent::CopyFrom(const TimedEvent* other)
{
    unsigned int eax;
    char* src = (char*)other;
    char* dst = (char*)this;
    char bl;

    this->field_0x8 = other->field_0x8;
    eax = 0;
    this->m_duration = other->m_duration;
loop1:
    bl = src[0x10 + eax];
    eax++;
    if (eax < 0x20) {
        dst[0xf + eax] = bl;
        goto loop1;
    }
    dst[0xf + eax] = bl;

    this->field_0x30 = other->field_0x30;
    {
        int* esi = (int*)(&dst[0x38]);
        int* ptr = (int*)(&src[0x38]);
        int edi = ptr[0];
        int ebx = ptr[1];
        eax = 0;
        esi[0] = edi;
        esi[1] = ebx;
    }
loop2:
    bl = ((char*)&other->m_data_0x40)[eax];
    eax++;
    if (eax < 0x40) {
        this->m_data_0x40[eax - 1] = bl;
        goto loop2;
    }
    this->m_data_0x40[eax - 1] = bl;

    this->field_0x80 = other->field_0x80;
    this->field_0x88 = other->field_0x88;
    this->field_0x8c = other->field_0x8c;
    this->field_0x90 = other->field_0x90;
    this->field_0x94 = other->field_0x94;
    this->field_0x98 = other->field_0x98;
    this->field_0x9c = other->field_0x9c;
    this->field_0xa0 = other->field_0xa0;
    this->field_0xa4 = other->field_0xa4;
    this->field_0xa8 = other->field_0xa8;
    this->field_0xac = other->field_0xac;
    this->field_0xb0 = other->field_0xb0;
    this->field_0xb4 = other->field_0xb4;
    this->field_0xb8 = other->field_0xb8;
    this->field_0xbc = other->field_0xbc;
}

/* Function start: 0x402420 */
TimedEvent* TimedEventPool::Create(void* callback, void* data)
{
    if (this->m_free_list == 0) {
        int* new_pool = (int*)AllocateMemory(this->m_pool_size * 200 + 4);
        *new_pool = (int)this->m_pool;
        int count = this->m_pool_size;
        this->m_pool = (TimedEvent*)new_pool;

        int* current = new_pool + count * 50 - 49;
        count--;
        if (count >= 0) {
            do {
                count--;
                *current = (int)this->m_free_list;
                this->m_free_list = (TimedEvent*)current;
                current = current - 50;
            } while (count >= 0);
        }
    }

    TimedEvent* event = this->m_free_list;
    Message* ebx = (Message*)((int*)event + 2);
    this->m_free_list = *(TimedEvent**)event;
    *(void**)((char*)event + 4) = callback;
    *(void**)event = data;
    this->m_count++;

    __asm {
        mov edi, ebx
        xor eax, eax
        mov ecx, 0x30
        rep stosd
    }

    int edi = 0;
    if (ebx == 0) goto done;
loop:
    {
        Message* tmp = ebx;
        tmp->Message::Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    ebx = (Message*)((char*)ebx + 0xc0);
    {
        int eax = edi;
        edi--;
        if (eax != 0) goto loop;
    }
done:
    return event;
}

/* Function start: 0x401910 */
TimedEvent::~TimedEvent()
{
    if (m_next_event_data) {
        TimedEvent_dtor(m_next_event_data);
        FreeFromGlobalHeap(m_next_event_data);
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
        sprintf(DAT_00436960, s__3_3d____2_2d_004350d0, remaining_time / 60000, (remaining_time / 1000) % 60);
        FUN_0041c000(g_SoundManager, DAT_00436960, 0x208, 0x1c2, 10000);

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
        ShowError(s_illegal_type__d__TimedEvent__Upd_004350ac, m_type);
    }
    return 0;
}
