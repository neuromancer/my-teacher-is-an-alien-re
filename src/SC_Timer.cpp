#include "SC_Timer.h"
#include "Memory.h"
#include "Message.h"
#include "Queue.h"
#include "TimedEvent.h"
#include "Timer.h"
#include "SC_Question.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function start: 0x401B60 */
SC_Timer::SC_Timer() {
  m_messageId = 0xd;
  timer1.Reset();

  m_eventList = new Queue();
}

/* Function start: 0x401CA0 */
SC_Timer::~SC_Timer() {
  Queue *pQueue;
  void *pData;

  pQueue = m_eventList;
  if (pQueue != 0) {
    if (pQueue->head != 0) {
      pQueue->current = pQueue->head;
      while (pQueue->head != 0) {
        pData = pQueue->RemoveCurrent();
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
      }
    }
    delete pQueue;
    m_eventList = 0;
  }
}

/* Function start: 0x401DF0 */
void SC_Timer::Init(SC_Message* msg) {
  Handler::Init(msg);
  if (msg != 0) {
    sourceAddress = msg->sourceAddress;
  }
}

/* Function start: 0x401E20 */
int SC_Timer::ShutDown(SC_Message* msg) {
  return 0;
}

/* Function start: 0x401E30 */
void SC_Timer::Update(int param_1, int param_2) {
  Queue *pQueue;
  QueueNode *pNode;
  void *pData;

  if ((timer1.Update() > 10000) && (m_eventList->head == 0)) {
    SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                    0, 0);
  }

  timer1.Reset();

  pQueue = m_eventList;
  pQueue->current = pQueue->head;

  if (pQueue->head != 0) {
    do {
      pQueue = m_eventList;
      pNode = (QueueNode *)pQueue->current;
      TimedEvent *pEvent = 0;
      if (pNode != 0) {
        pEvent = (TimedEvent *)pNode->data;
      }
      if (pEvent->Update()) {
        pQueue = m_eventList;
        pData = pQueue->RemoveCurrent();
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
      }
      pQueue = m_eventList;
      pNode = (QueueNode *)pQueue->current;
      if (pQueue->tail == pNode) {
        break;
      }
      if (pNode != 0) {
        pQueue->current = pNode->next;
      }

      pQueue = m_eventList;
    } while (pQueue->head != 0);
  }

  if (m_messageId == param_2) {
    ShowError("SC_Timer::Update - Timeout waiting for event");
  }
}

/* Function start: 0x401F90 */
int SC_Timer::AddMessage(SC_Message* msg) {
  Handler::AddMessage(msg);
  ShowError("SC_Timer::AddMessage");
  return 1;
}

/* Function start: 0x401FB0 */
int SC_Timer::Exit(SC_Message* param_1) {
  Queue *pQueue;
  QueueNode *pNode;
  void *pData;
  TimedEvent *pNewEvent;

  Message *msg = (Message *)param_1;

  if (msg->targetAddress != m_messageId) {
    return 0;
  }

  timer1.Reset();

  switch (msg->priority) {
  case 0xe:
    break;
  case 0xf:
    pQueue = m_eventList;
    if (pQueue->head != 0) {
      pQueue->current = pQueue->head;
      pNode = (QueueNode *)pQueue->head;
      while (pNode != 0) {
        pData = pQueue->RemoveCurrent();
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
        pNode = (QueueNode *)pQueue->head;
      }
    }
    break;
  case 0x13:
    if (msg->userPtr != 0) {
      pNewEvent = new TimedEvent();
      pNewEvent->m_duration = msg->field_0xb8;
      pNewEvent->m_sourceAddress = msg->sourceAddress;
      pNewEvent->m_eventData = (SC_Message*)msg->userPtr;
      msg->userPtr = 0;
      pNewEvent->SetType(msg->param1);
      pQueue = m_eventList;
      if (pNewEvent == 0) {
        ShowError("queue fault 0101");
      }
      pQueue->current = pQueue->head;
      if ((pQueue->type == 1) || (pQueue->type == 2)) {
        if (pQueue->head == 0) {
          pQueue->Insert(pNewEvent);
        } else {
          do {
            pNode = (QueueNode *)pQueue->current;
            if (((TimedEvent *)pNode->data)->m_duration <
                (unsigned int)pNewEvent->m_duration) {
              pQueue->Insert(pNewEvent);
              break;
            }
            if (pQueue->tail == pNode) {
              pQueue->Push(pNewEvent);
              break;
            }
            if (pNode != 0) {
              pQueue->current = pNode->next;
            }
          } while (pQueue->current != 0);
        }
      } else {
        pQueue->Insert(pNewEvent);
      }
    } else {
      ((SC_Message *)msg)->Dump(0);
      ShowError("SC_Timer::Input");
    }
    break;
  case 0x14:
    pNewEvent = new TimedEvent();
    pNewEvent->m_sourceAddress = msg->sourceAddress;
    pQueue = m_eventList;
    if (pNewEvent == 0) {
      ShowError("queue fault 0103");
    }
    pNode = (QueueNode *)pQueue->head;
    pQueue->current = pNode;
    while (pNode != 0) {
      pNode = (QueueNode *)pQueue->current;
      TimedEvent *pListEvent = 0;
      if (pNode != 0) {
        pListEvent = (TimedEvent *)pNode->data;
      }
      if (pListEvent->m_duration == pNewEvent->m_duration) {
        pData = m_eventList->Pop();
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
        break;
      }
      if (pQueue->tail == pNode)
        break;
      if (pNode != 0) {
        pQueue->current = pNode->next;
      }
      pNode = (QueueNode *)pQueue->current;
    }
    break;
  case 0x1b:
    if (m_eventList->head == 0) {
      SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                      0, 0);
    }
    break;
  default:
    return 0;
  }

  return 1;
}
