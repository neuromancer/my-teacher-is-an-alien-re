#include "SC_Timer.h"
#include "Memory.h"
#include "Message.h"
#include "Queue.h"
#include "TimedEvent.h"
#include "Timer.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
extern "C" {
void FUN_004171b0(int);
void FUN_00419fd0(void *);
TimedEvent *TimedEvent_Init(TimedEvent *);
}

/* Function start: 0x401B60 */
SC_Timer::SC_Timer() : Parser(), timer1(), timer2() {
  int *ptr = &m_messageId;
  for (int i = 6; i != 0; i--) {
    *ptr++ = 0;
  }

  m_messageId = 0xd;
  timer1.Reset();

  m_eventList = (Queue *)AllocateMemory(0x10);
  if (m_eventList != 0) {
    m_eventList->Init();
  }
}

SC_Timer::~SC_Timer() {}

/* Function start: 0x401E30 */
void SC_Timer::Update(int param_1, int param_2) {
  if ((timer1.Update() > 10000) && (m_eventList->m_head == 0)) {
    SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                    0, 0);
  }

  timer1.Reset();

  m_eventList->m_current = m_eventList->m_head;

  if (m_eventList->m_head == 0)
    goto end_loop;

loop: {
  TimedEvent *event = (TimedEvent *)((QueueNode *)m_eventList->m_current)->data;
  if (event->Update()) {
    TimedEvent *data = (TimedEvent *)m_eventList->Pop();
    if (data) {
      data->~TimedEvent();
      FreeFromGlobalHeap(data);
    }
    m_eventList->m_current = m_eventList->m_head;
  } else {
    if (m_eventList->m_tail == m_eventList->m_current) {
      goto end_loop;
    }
    if (m_eventList->m_current) {
      m_eventList->m_current = ((QueueNode *)m_eventList->m_current)->next;
    }
  }
}
  if (m_eventList->m_current != 0)
    goto loop;

end_loop:
  if (m_messageId == param_2) {
    ShowError("SC_Timer::Update - Timeout waiting for event");
  }
}

/* Function start: 0x401F90 */
void SC_Timer::AddMessage(int param_1) {
  FUN_004171b0(param_1);
  ShowError("SC_Timer::AddMessage");
}

/* Function start: 0x401FB0 */
int SC_Timer::Input(void *param_1) {
  int *piVar1;
  int iVar2;
  void *pvVar6;
  TimedEvent *puVar7;
  TimedEvent *puVar4;

  Message *param = (Message *)param_1;

  if (param->field_0x88 != m_messageId) {
    return 0;
  }

  timer1.Reset();

  switch (param->field_0x98) {
  case 0xe:
    break;
  case 0xf:
    piVar1 = (int *)m_eventList;
    if (*piVar1 != 0) {
      piVar1[2] = *piVar1;
      iVar2 = *piVar1;
      while (iVar2 != 0) {
        iVar2 = piVar1[2];
        if (iVar2 == 0) {
          pvVar6 = 0;
        } else {
          if (*piVar1 == iVar2) {
            *piVar1 = *(int *)(iVar2 + 4);
          }
          if (piVar1[1] == piVar1[2]) {
            piVar1[1] = *(int *)piVar1[2];
          }
          iVar2 = *(int *)piVar1[2];
          if (iVar2 != 0) {
            *(int *)(iVar2 + 4) = ((int *)piVar1[2])[1];
          }
          int *puVar4_temp = (int *)((int *)piVar1[2])[1];
          if (puVar4_temp != 0) {
            *puVar4_temp = *(int *)piVar1[2];
          }
          void *this_00 = (void *)piVar1[2];
          pvVar6 = 0;
          if (this_00 != 0) {
            pvVar6 = *(void **)((int)this_00 + 8);
            ((Queue *)this_00)->Destroy(1);
            piVar1[2] = 0;
          }
          piVar1[2] = *piVar1;
        }
        if (pvVar6 != 0) {
          ((TimedEvent *)pvVar6)->~TimedEvent();
          FreeMemory(pvVar6);
        }
        iVar2 = *piVar1;
      }
    }
    break;
  case 0x13:
    if (param->field_0xbc == 0) {
      // Write(param); // FIXME: Undefined function
      ShowError("SC_Timer::Input");
    }
    puVar7 = 0;
    puVar4 = (TimedEvent *)AllocateMemory(0x28);
    if (puVar4 != 0) {
      puVar7 = TimedEvent_Init(puVar4);
    }
    puVar7->m_duration = *(int *)((char *)param_1 + 0xb8);
    puVar7->field_0x8 = *(int *)((char *)param_1 + 0x8c);
    puVar7->m_next_event_data = (void *)*(int *)((char *)param_1 + 0xbc);
    *(int *)((char *)param_1 + 0xbc) = 0;
    puVar7->SetData(*(int *)((char *)param_1 + 0x9c));

    piVar1 = (int *)m_eventList;
    if (puVar7 == 0) {
      ShowError("queue fault 0101");
    }
    piVar1[2] = *piVar1;
    if ((piVar1[3] == 1) || (piVar1[3] == 2)) {
      if (*piVar1 == 0) {
        m_eventList->Insert(puVar7);
      } else {
        do {
          iVar2 = piVar1[2];
          if (*(unsigned int *)(*(int *)(iVar2 + 8) + 0xc) <
              (unsigned int)*(int *)((char *)puVar7 + 0xc)) {
            m_eventList->Insert(puVar7);
            goto done_0x13;
          }
          if (piVar1[1] == iVar2) {
            m_eventList->Push(puVar7);
            goto done_0x13;
          }
          if (iVar2 != 0) {
            piVar1[2] = *(int *)(iVar2 + 4);
          }
        } while (piVar1[2] != 0);
      }
    } else {
      m_eventList->Insert(puVar7);
    }
  done_0x13:
    break;
  case 0x14:
    puVar7 = 0;
    puVar4 = (TimedEvent *)AllocateMemory(0x28);
    if (puVar4 != 0) {
      puVar7 = TimedEvent_Init(puVar4);
    }
    puVar7->field_0x8 = *(int *)((char *)param_1 + 0x8c);
    piVar1 = (int *)m_eventList;
    if (puVar7 == 0) {
      ShowError("queue fault 0103");
    }
    iVar2 = *piVar1;
    piVar1[2] = iVar2;
    while (iVar2 != 0) {
      iVar2 = piVar1[2];
      int iVar5 = 0;
      if (iVar2 != 0) {
        iVar5 = *(int *)(iVar2 + 8);
      }
      if (*(int *)(iVar5 + 0xc) == puVar7->m_duration) {
        pvVar6 = m_eventList->Pop();
        if (pvVar6 != 0) {
          ((TimedEvent *)pvVar6)->~TimedEvent();
          FreeMemory(pvVar6);
        }
        break;
      }
      if (piVar1[1] == iVar2)
        break;
      if (iVar2 != 0) {
        piVar1[2] = *(int *)(iVar2 + 4);
      }
      iVar2 = piVar1[2];
    }
    break;
  case 0x1b:
    if (*(int *)m_eventList == 0) {
      SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                      0, 0);
    }
    break;
  default:
    return 0;
  }

  return 1;
}
