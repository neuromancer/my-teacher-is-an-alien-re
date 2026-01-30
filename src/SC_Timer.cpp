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
  QueueNode *pNode;
  void *pData;

  pQueue = m_eventList;
  if (pQueue != 0) {
    if (pQueue->m_head != 0) {
      pQueue->m_current = pQueue->m_head;
      while (pQueue->m_head != 0) {
        pNode = (QueueNode *)pQueue->m_current;
        if (pNode == 0) {
          pData = 0;
        } else {
          if (pQueue->m_head == pNode) {
            pQueue->m_head = pNode->next;
          }
          pNode = (QueueNode *)pQueue->m_current;
          if (pQueue->m_tail == pNode) {
            pQueue->m_tail = pNode->prev;
          }
          pNode = (QueueNode *)pQueue->m_current;
          QueueNode *pPrev = pNode->prev;
          if (pPrev != 0) {
            pPrev->next = pNode->next;
          }
          pNode = (QueueNode *)pQueue->m_current;
          QueueNode *pNext = pNode->next;
          if (pNext != 0) {
            pNext->prev = pNode->prev;
          }
          pNode = (QueueNode *)pQueue->m_current;
          pData = 0;
          if (pNode != 0) {
            pData = pNode->data;
          }
          if (pNode != 0) {
            delete pNode;
            pQueue->m_current = 0;
          }
          pQueue->m_current = pQueue->m_head;
        }
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
      }
    }
    delete pQueue;
    m_eventList = 0;
  }
}

/* Function start: 0x401E30 */
void SC_Timer::Update(int param_1, int param_2) {
  Queue *pQueue;
  QueueNode *pNode;
  void *pData;

  if ((timer1.Update() > 10000) && (m_eventList->m_head == 0)) {
    SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                    0, 0);
  }

  timer1.Reset();

  pQueue = m_eventList;
  pQueue->m_current = pQueue->m_head;

  if (pQueue->m_head == 0)
    goto end_loop;

loop: {
  pNode = (QueueNode *)pQueue->m_current;
  TimedEvent *pEvent = 0;
  if (pNode != 0) {
    pEvent = (TimedEvent *)pNode->data;
  }
  if (pEvent->Update()) {
    pQueue = m_eventList;
    pNode = (QueueNode *)pQueue->m_current;
    if (pNode == 0) {
      pData = 0;
    } else {
      if (pQueue->m_head == pNode) {
        pQueue->m_head = pNode->next;
      }
      pNode = (QueueNode *)pQueue->m_current;
      if (pQueue->m_tail == pNode) {
        pQueue->m_tail = pNode->prev;
      }
      pNode = (QueueNode *)pQueue->m_current;
      QueueNode *pPrev = pNode->prev;
      if (pPrev != 0) {
        pPrev->next = pNode->next;
      }
      pNode = (QueueNode *)pQueue->m_current;
      QueueNode *pNext = pNode->next;
      if (pNext != 0) {
        pNext->prev = pNode->prev;
      }
      pNode = (QueueNode *)pQueue->m_current;
      pData = 0;
      if (pNode != 0) {
        pData = pNode->data;
      }
      if (pNode != 0) {
        delete pNode;
        pQueue->m_current = 0;
      }
      pQueue->m_current = pQueue->m_head;
    }
    if (pData != 0) {
      delete (TimedEvent *)pData;
    }
    goto check_loop;
  }
  pQueue = m_eventList;
  pNode = (QueueNode *)pQueue->m_current;
  if (pQueue->m_tail == pNode) {
    goto end_loop;
  }
  if (pNode != 0) {
    pQueue->m_current = pNode->next;
  }
}
check_loop:
  pQueue = m_eventList;
  if (pQueue->m_head != 0)
    goto loop;

end_loop:
  if (m_messageId == param_2) {
    ShowError("SC_Timer::Update - Timeout waiting for event");
  }
}

/* Function start: 0x401F90 */
int SC_Timer::AddMessage(int param_1) {
  WriteMessageAddress((Message *)param_1);
  ShowError("SC_Timer::AddMessage");
  return 1;
}

/* Function start: 0x401FB0 */
int SC_Timer::Input(void *param_1) {
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
    if (pQueue->m_head != 0) {
      pQueue->m_current = pQueue->m_head;
      pNode = (QueueNode *)pQueue->m_head;
      while (pNode != 0) {
        pNode = (QueueNode *)pQueue->m_current;
        if (pNode == 0) {
          pData = 0;
        } else {
          if (pQueue->m_head == pNode) {
            pQueue->m_head = pNode->next;
          }
          if (pQueue->m_tail == pQueue->m_current) {
            pQueue->m_tail = ((QueueNode *)pQueue->m_current)->prev;
          }
          pNode = (QueueNode *)pQueue->m_current;
          if (pNode->prev != 0) {
            pNode->prev->next = pNode->next;
          }
          QueueNode *pNext = ((QueueNode *)pQueue->m_current)->next;
          if (pNext != 0) {
            pNext->prev = ((QueueNode *)pQueue->m_current)->prev;
          }
          QueueNode *pCurr = (QueueNode *)pQueue->m_current;
          pData = 0;
          if (pCurr != 0) {
            pData = pCurr->data;
            delete pCurr;
            pQueue->m_current = 0;
          }
          pQueue->m_current = pQueue->m_head;
        }
        if (pData != 0) {
          delete (TimedEvent *)pData;
        }
        pNode = (QueueNode *)pQueue->m_head;
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
      pQueue->m_current = pQueue->m_head;
      if ((pQueue->m_field_0xc == 1) || (pQueue->m_field_0xc == 2)) {
        if (pQueue->m_head == 0) {
          pQueue->Insert(pNewEvent);
        } else {
          do {
            pNode = (QueueNode *)pQueue->m_current;
            if (((TimedEvent *)pNode->data)->m_duration <
                (unsigned int)pNewEvent->m_duration) {
              pQueue->Insert(pNewEvent);
              goto done_0x13;
            }
            if (pQueue->m_tail == pNode) {
              pQueue->Push(pNewEvent);
              goto done_0x13;
            }
            if (pNode != 0) {
              pQueue->m_current = pNode->next;
            }
          } while (pQueue->m_current != 0);
        }
      } else {
        pQueue->Insert(pNewEvent);
      }
    } else {
      ((SC_Message *)msg)->Dump(0);
      ShowError("SC_Timer::Input");
    }
  done_0x13:
    break;
  case 0x14:
    pNewEvent = new TimedEvent();
    pNewEvent->m_sourceAddress = msg->sourceAddress;
    pQueue = m_eventList;
    if (pNewEvent == 0) {
      ShowError("queue fault 0103");
    }
    pNode = (QueueNode *)pQueue->m_head;
    pQueue->m_current = pNode;
    while (pNode != 0) {
      pNode = (QueueNode *)pQueue->m_current;
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
      if (pQueue->m_tail == pNode)
        break;
      if (pNode != 0) {
        pQueue->m_current = pNode->next;
      }
      pNode = (QueueNode *)pQueue->m_current;
    }
    break;
  case 0x1b:
    if (m_eventList->m_head == 0) {
      SC_Message_Send(3, m_messageId, m_messageId, m_messageData, 0x14, 0, 0, 0,
                      0, 0);
    }
    break;
  default:
    return 0;
  }

  return 1;
}
