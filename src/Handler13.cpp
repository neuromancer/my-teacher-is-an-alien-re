#include "Handler13.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "TimedEvent.h"
#include "Queue.h"

// Queue methods used: Insert (0x4024d0), Push (0x4025a0), Pop (0x402680)

/* Function start: 0x401B60 */
Handler13::Handler13() {
    // Set handlerId to 13
    handlerId = 13;

    // Call Timer::Reset() on first timer
    timer1.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
}

/* Function start: 0x401CA0 */
Handler13::~Handler13() {
    MessageList* pList;
    TimedEvent* data;

    pList = list;
    if (pList != 0) {
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                data = (TimedEvent*)pList->Pop();
                if (data != 0) {
                    delete data;
                }
            }
        }
        delete pList;
        list = 0;
    }
}

/* Function start: 0x401DF0 */
void Handler13::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = msg->data;
    }
}

int Handler13::ShutDown(SC_Message* msg) {
    return 0;
 }

/* Function start: 0x401E30 */
void Handler13::Update(int param1, int param2) {
    MessageNode* node;
    MessageNode* prevNode;
    unsigned int uVar3;
    int iVar4;
    TimedEvent* timedEvent;
    void* pvVar6;

    uVar3 = timer1.Update();
    if ((uVar3 > 10000) && (list->head == 0)) {
        SC_Message_Send(3, handlerId, handlerId, moduleParam, 0x14, 0, 0, 0, 0, 0);
    }

    timer1.Reset();

    list->current = list->head;
    iVar4 = (int)list->head;

    while (iVar4 != 0) {
        timedEvent = 0;
        node = (MessageNode*)list->current;
        if (node != 0) {
            timedEvent = (TimedEvent*)node->data;
        }
        iVar4 = timedEvent->Update();
        if (iVar4 != 0) {
            pvVar6 = list->Pop();
            if (pvVar6 != 0) {
                delete (TimedEvent*)pvVar6;
            }
        }
        node = (MessageNode*)list->current;
        if (list->tail == node) {
            break;
        }
        if (node != 0) {
            list->current = node->next;
        }
        iVar4 = (int)list->head;
    }

    if (handlerId == param2) {
        ShowError("SC_Timer::Update");
    }
}

/* Function start: 0x401F90 */
int Handler13::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    ShowError("SC_Timer::AddMessage");
    return 1;
}

/* Function start: 0x401FB0 */
int Handler13::Exit(SC_Message* msg) {
    TimedEvent* pTimedEvent;
    TimedEvent* eventData;
    TimedEvent* timedEvent;
    MessageNode* node;
    MessageList* pList;

    if (msg->targetAddress != Handler13::handlerId) {
        return 0;
    }

    timer1.Reset();

    switch (msg->priority) {
    case 0xe:
        break;

    case 0xf:
        // Clear entire list
        pList = list;
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                eventData = (TimedEvent*)pList->Pop();
                if (eventData != 0) {
                    delete eventData;
                }
            }
        }
        break;

    case 0x13:
        if (msg->userPtr != 0) {
            pTimedEvent = new TimedEvent();
            pTimedEvent->m_duration = msg->time;
            pTimedEvent->m_sourceAddress = msg->sourceAddress;
            pTimedEvent->m_eventData = (SC_Message*)msg->userPtr;
            msg->userPtr = 0;
            pTimedEvent->SetType(msg->param1);
            pList = list;
            if (pTimedEvent == 0) {
                ShowError("queue fault 0101");
            }
            node = (MessageNode*)pList->head;
            pList->current = node;
            if ((pList->type == 1) || (pList->type == 2)) {
                if (node == 0) {
                    ((Queue*)pList)->Insert(pTimedEvent);
                } else {
                    do {
                        node = (MessageNode*)pList->current;
                        timedEvent = (TimedEvent*)node->data;
                        if ((unsigned int)timedEvent->m_duration < (unsigned int)pTimedEvent->m_duration) {
                            ((Queue*)pList)->Insert(pTimedEvent);
                            break;
                        }
                        if (pList->tail == node) {
                            ((Queue*)pList)->Push(pTimedEvent);
                            break;
                        }
                        if (node != 0) {
                            pList->current = node->next;
                        }
                    } while (pList->current != 0);
                }
            } else {
                ((Queue*)pList)->Insert(pTimedEvent);
            }
        } else {
            msg->Dump(0);
            ShowError("SC_Timer::Input");
        }
        break;

    case 0x14:
        pTimedEvent = new TimedEvent();
        pTimedEvent->m_sourceAddress = msg->sourceAddress;
        pList = list;
        if (pTimedEvent == 0) {
            ShowError("queue fault 0103");
        }
        pList->current = pList->head;
        if (pList->head != 0) {
            do {
                node = (MessageNode*)pList->current;
                timedEvent = 0;
                if (node != 0) {
                    timedEvent = (TimedEvent*)node->data;
                }
                if (timedEvent->m_duration == pTimedEvent->m_duration) {
                    eventData = (TimedEvent*)((Queue*)pList)->Pop();
                    if (eventData != 0) {
                        delete eventData;
                    }
                    break;
                }
                if (pList->tail == node) {
                    break;
                }
                if (node != 0) {
                    pList->current = node->next;
                }
            } while (pList->current != 0);
        }
        break;

    case 0x1b:
        if (list->head == 0) {
            SC_Message_Send(3, handlerId, handlerId, moduleParam, 0x14, 0, 0, 0, 0, 0);
        }
        break;

    default:
        return 0;
    }

    return 1;
}
