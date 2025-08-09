#include "SC_Timer.h"
#include "Queue.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"
#include "TimedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
extern "C" {
    void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);
    void* TimedEvent__Init(void*);
    void TimedEvent__SetData(void*, int);
    void TimedEvent__delete(int);
    void ShowError(const char*, ...);
}

/* Function start: 0x401B60 */
SC_Timer::SC_Timer()
{
    m_messageId = 0;
    m_messageData = 0;
    field_0x90 = 0;
    field_0x94 = 0;
    field_0x98 = 0;
    field_0x9c = 0;

    timer1.Init();
    timer2.Init();

    m_messageId = 0xd;
    timer1.Reset();

    m_eventList = (Queue*)AllocateMemory(0x10);
    if (m_eventList) {
        m_eventList->Init();
    }
}

SC_Timer::~SC_Timer()
{
}

/* Function start: 0x401E30 */
void SC_Timer::Update(int param_1, int param_2)
{
    if ((timer1.Update() > 10000) && (m_eventList->m_head == 0)) {
        SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0, 0, 0);
    }

    timer1.Reset();

    m_eventList->m_current = m_eventList->m_head;

    while (m_eventList->m_current != 0) {
        if (((TimedEvent*)((QueueNode*)m_eventList->m_current)->data)->Update()) {
            void* data = m_eventList->Pop();
            if (data) {
                TimedEvent__delete((int)data);
                FreeMemory(data);
            }
            m_eventList->m_current = m_eventList->m_head;
        } else {
            if (m_eventList->m_tail == m_eventList->m_current) {
                break;
            }
            if (m_eventList->m_current) {
                m_eventList->m_current = ((QueueNode*)m_eventList->m_current)->next;
            }
        }
    }

    if (m_messageId != param_2) {
        return;
    }

    ShowError("SC_Timer::Update - Timeout waiting for event");
}

/* Function start: 0x401F90 */
void SC_Timer::AddMessage(int param_1)
{
    // This is a virtual function call, but we don't have the vtable yet.
    // We'll just call the function directly.
    ((void (*)(void*, int))0x4171b0)(this, param_1);
    ShowError("SC_Timer::AddMessage");
}

/* Function start: 0x401FB0 */
int SC_Timer::Input(void* param_1)
{
    if (*(int*)((char*)param_1 + 0x88) != m_messageId) {
        return 0;
    }

    timer1.Reset();

    void* data;
    TimedEvent* new_event;

    switch (*(int*)((char*)param_1 + 0x98)) {
        case 0xe:
            break;
        case 0xf:
            m_eventList->m_current = m_eventList->m_head;
            while (m_eventList->m_current != 0) {
                data = m_eventList->Pop();
                if (data != 0) {
                    TimedEvent__delete((int)data);
                    FreeMemory(data);
                }
                m_eventList->m_current = m_eventList->m_head;
            }
            break;
        case 0x13:
            if (*(int*)((char*)param_1 + 0xbc) == 0) {
                ShowError("SC_Timer::Input");
            }
            new_event = (TimedEvent*)AllocateMemory(0x28);
            if (new_event) {
                TimedEvent__Init((void*)new_event);
            }
            *(int*)((char*)new_event + 0xc) = *(int*)((char*)param_1 + 0xb8);
            *(int*)((char*)new_event + 0x8) = *(int*)((char*)param_1 + 0x8c);
            *(int*)((char*)new_event + 0x10) = *(int*)((char*)param_1 + 0xbc);
            *(int*)((char*)param_1 + 0xbc) = 0;
            TimedEvent__SetData((void*)new_event, *(int*)((char*)param_1 + 0x9c));

            if (new_event == 0) {
                ShowError("queue fault 0101");
            }

            m_eventList->Insert(new_event);
            break;
        case 0x14:
            TimedEvent* timed_event;
            timed_event = (TimedEvent*)AllocateMemory(0x28);
            if (timed_event) {
                TimedEvent__Init((void*)timed_event);
            }
            *(int*)((char*)timed_event + 0x8) = *(int*)((char*)param_1 + 0x8c);

            if (timed_event == 0) {
                ShowError("queue fault 0103");
            }

            data = m_eventList->Pop();
            if (data) {
                TimedEvent__delete((int)data);
                FreeMemory(data);
            }
            break;
        case 0x1b:
            if (m_eventList->m_head == 0) {
                //SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0, 0, 0);
            }
            break;
        default:
            return 0;
    }

    return 1;
}
