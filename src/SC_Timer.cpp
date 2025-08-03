#include "SC_Timer.h"
#include "Queue.h"
#include "Memory.h"

// Forward declarations
class TimedEvent {
public:
    static int Update(TimedEvent* pThis);
    static void Delete(TimedEvent* pThis);
};

extern "C" {
    void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);
    void FreeFromGlobalHeap(void*);
    void ShowError(const char*);
    void* TimedEvent__Init(void*);
    void TimedEvent__SetData(void*, int);
}

SC_Timer::SC_Timer()
{
}

SC_Timer::~SC_Timer()
{
}

/* Function start: 0x401E30 */
void SC_Timer::Update(int param_1, int param_2)
{
    if ((timer.Update() > 10000) && (*(int**)m_eventList == 0)) {
        SC_Message_Send(3, field_0x88, field_0x88, field_0x8c, 0x14, 0, 0, 0, 0, 0);
    }

    timer.Reset();

    int** eventList = (int**)m_eventList;
    eventList[2] = *eventList;

    while (eventList[2] != 0) {
        if (TimedEvent::Update((TimedEvent*)eventList[2])) {
            int* to_delete = eventList[2];
            if (*eventList == to_delete) {
                *eventList = (int*)to_delete[1];
            }
            if (eventList[1] == to_delete) {
                eventList[1] = (int*)*to_delete;
            }
            if (*to_delete) {
                ((int*)*to_delete)[1] = to_delete[1];
            }
            if (to_delete[1]) {
                *(int**)(to_delete[1]) = (int*)*to_delete;
            }

            void* data = (void*)to_delete[2];
            to_delete[2] = 0;
            *to_delete = 0;
            to_delete[1] = 0;
            FreeFromGlobalHeap(to_delete);

            if (data) {
                TimedEvent::Delete((TimedEvent*)data);
                FreeFromGlobalHeap(data);
            }
            eventList[2] = *eventList;
        } else {
            if (eventList[1] == eventList[2]) {
                break;
            }
            if (eventList[2]) {
                eventList[2] = (int*)((int*)eventList[2])[1];
            }
        }
    }

    if (field_0x88 != param_2) {
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
    if (*(int*)((char*)param_1 + 0x88) != field_0x88) {
        return 0;
    }

    timer.Reset();

    int message = *(int*)((char*)param_1 + 0x98);

    if (message == 0xe) {
    }
    else if (message == 0xf) {
        int** eventList = (int**)m_eventList;
        if (*eventList != 0) {
            eventList[2] = *eventList;
            while (eventList[2] != 0) {
                void* data = (void*)((int*)eventList[2])[2];
                if (data) {
                    TimedEvent::Delete((TimedEvent*)data);
                    FreeFromGlobalHeap(data);
                }
                int* to_delete = eventList[2];
                if (*eventList == to_delete) {
                    *eventList = (int*)to_delete[1];
                }
                if (eventList[1] == to_delete) {
                    eventList[1] = (int*)*to_delete;
                }
                if (*to_delete) {
                    ((int*)*to_delete)[1] = to_delete[1];
                }
                if (to_delete[1]) {
                    *(int**)(to_delete[1]) = (int*)*to_delete;
                }
                FreeFromGlobalHeap(to_delete);
                eventList[2] = *eventList;
            }
        }
    }
    else if (message == 0x13) {
        if (*(int*)((char*)param_1 + 0xbc) == 0) {
            ShowError("SC_Timer::Input");
        }
        TimedEvent* new_event = (TimedEvent*)AllocateMemory_Wrapper(0x28);
        if (new_event) {
            TimedEvent__Init((void*)new_event);
        }
        // TimedEvent has a size of 0x28
        *(int*)((char*)new_event + 0xc) = *(int*)((char*)param_1 + 0xb8);
        *(int*)((char*)new_event + 0x8) = *(int*)((char*)param_1 + 0x8c);
        *(int*)((char*)new_event + 0x10) = *(int*)((char*)param_1 + 0xbc);
        *(int*)((char*)param_1 + 0xbc) = 0;
        TimedEvent__SetData((void*)new_event, *(int*)((char*)param_1 + 0x9c));

            Queue* eventList = (Queue*)m_eventList;
            if (new_event == 0) {
                ShowError("queue fault 0101");
            }

            eventList->Insert(new_event);
    }
    else if (message == 0x14) {
        TimedEvent* new_event = (TimedEvent*)AllocateMemory_Wrapper(0x28);
        if (new_event) {
            TimedEvent__Init((void*)new_event);
        }
        *(int*)((char*)new_event + 0x8) = *(int*)((char*)param_1 + 0x8c);

            Queue* eventList = (Queue*)m_eventList;
            if (new_event == 0) {
                ShowError("queue fault 0103");
            }

            void* data = eventList->Pop();
            if (data) {
                TimedEvent::Delete((TimedEvent*)data);
                FreeFromGlobalHeap(data);
            }
    }
    else if (message == 0x1b) {
        if (**(int***)m_eventList == 0) {
            SC_Message_Send(3, field_0x88, field_0x88, field_0x8c, 0x14, 0, 0, 0, 0, 0);
        }
    }
    else {
        return 0;
    }

    return 1;
}
