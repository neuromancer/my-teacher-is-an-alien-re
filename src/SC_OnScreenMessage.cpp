#include "SC_OnScreenMessage.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "GameState.h"
#include "StringTable.h"
#include "OnScreenMessage.h"
#include "SC_Question.h"

extern "C" {
    void ShowError(const char*, ...);
}

/* Function start: 0x40A2E0 */
SC_OnScreenMessage::SC_OnScreenMessage() {
    // Set handlerId to 15
    handlerId = 15;

    // Initialize timer
    timer.Reset();

    // Allocate and initialize list header structure
    m_messageList = new MessageList();
    if (m_messageList != 0) {
        m_messageList->flags = 1;
    }
}

/* Function start: 0x40A410 */
SC_OnScreenMessage::~SC_OnScreenMessage() {
    MessageList* pList;
    MessageNode* node;
    OnScreenMessage* data;

    pList = m_messageList;
    if (pList != 0) {
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    data = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    // Get data and free node
                    node = (MessageNode*)pList->current;
                    data = 0;
                    if (node != 0) {
                        data = (OnScreenMessage*)node->data;
                    }
                    if (node != 0) {
                        delete node;
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (data != 0) {
                    Timer_DecrementCounter();
                    delete data;
                }
            }
        }
        delete pList;
        m_messageList = 0;
    }
}

/* Function start: 0x40A5A0 */
void SC_OnScreenMessage::Copy(SC_OnScreenMessage* other) {
    if (other) {
        field_90 = other->field_90;
        field_94 = other->field_94;
        field_8C = other->field_8C;
    }
}

/* Function start: 0x40A5D0 */
int SC_OnScreenMessage::ShutDown(SC_Message* msg) {
    return 0;
}

/* Function start: 0x40A5E0 */
void SC_OnScreenMessage::Update(int param1, int param2) {
    MessageNode* node;
    unsigned int uVar3;
    int iVar4;
    OnScreenMessage* msgItem;
    OnScreenMessage* pvVar6;
    MessageList* pList;
    int counter;

    pList = m_messageList;
    pList->current = pList->head;
    counter = 0;

    if (pList->head == 0) {
        goto check_timer;
    }

    do {
        msgItem = 0;
        pList = m_messageList;
        node = (MessageNode*)pList->current;
        if (node != 0) {
            msgItem = (OnScreenMessage*)node->data;
        }

        iVar4 = msgItem->Update(counter);
        if (iVar4 != 0) {
            pList = m_messageList;
            node = (MessageNode*)pList->current;
            if (node == 0) {
                pvVar6 = 0;
            } else {
                // Unlink node from head
                if (pList->head == node) {
                    pList->head = node->next;
                }
                // Unlink node from tail
                if (pList->tail == pList->current) {
                    pList->tail = ((MessageNode*)pList->current)->prev;
                }
                // Update next node's prev pointer
                node = (MessageNode*)pList->current;
                if (node->prev != 0) {
                    node->prev->next = node->next;
                }
                // Update prev node's next pointer
                node = (MessageNode*)pList->current;
                if (node->next != 0) {
                    node->next->prev = node->prev;
                }
                // Get data and free node
                node = (MessageNode*)pList->current;
                pvVar6 = 0;
                if (node != 0) {
                    pvVar6 = (OnScreenMessage*)node->data;
                    delete node;
                    pList->current = 0;
                }
                pList->current = pList->head;
            }
            // Cleanup data object
            if (pvVar6 != 0) {
                Timer_DecrementCounter();
                delete pvVar6;
            }
        }

        counter++;
        pList = m_messageList;
        node = (MessageNode*)pList->current;
        if (pList->tail == node) {
            break;
        }
        if (counter > 10) {
            break;
        }
        if (node != 0) {
            pList->current = node->next;
        }
        pList = m_messageList;
    } while (pList->head != 0);

check_timer:
    uVar3 = timer.Update();
    if (uVar3 > 60000) {
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    }

    if (handlerId == param2) {
        WriteToMessageLog("SC_OnScreenMessage::Update");
    }
}

/* Function start: 0x40A7AA */
void SC_OnScreenMessage::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
    }
}

/* Function start: 0x40A7C0 */
int SC_OnScreenMessage::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_OnScreenMessage::AddMessage");
    return 1;
}

/* Function start: 0x40A7E0 */
int SC_OnScreenMessage::Exit(SC_Message* msg) {
    MessageList* pList;
    MessageNode* node;
    OnScreenMessage* newItem;
    OnScreenMessage* pvVar6;
    int count;
    SC_Message* pMsg = msg;

    if (pMsg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    if (pMsg->priority == 0xF) {
        goto send_remove_message;
    }
    if (pMsg->priority == 0x13) {
        goto process_message;
    }
    if (pMsg->priority == 0x1B) {
        goto send_remove_message;
    }
    return 0;

process_message:
    newItem = 0;
    if (pMsg->sourceAddress != 0) {
        if (g_Strings_00435a70 != 0) {
            if (g_Strings_00435a70->GetString(pMsg->sourceAddress, g_Buffer_00436960)) {
                newItem = new OnScreenMessage(g_Buffer_00436960, pMsg->param1);
            }
        }
    } else if (pMsg->userPtr != 0) {
        newItem = new OnScreenMessage((char*)pMsg->userPtr, pMsg->param1);
        pMsg->userPtr = 0;
    }

    if (newItem == 0) {
        goto count_items;
    }
    pList = m_messageList;
    pList->current = pList->head;
    if ((pList->flags == 1 || pList->flags == 2) && pList->head != 0) {
        while (pList->current != 0) {
            node = (MessageNode*)pList->current;
            if (((OnScreenMessage*)node->data)->timer.Update() < newItem->timer.Update()) {
                pList->InsertNode(newItem);
                goto count_items;
            }
            if (pList->tail == node) {
                if (newItem == 0) WriteToMessageLog("queue fault 0112");
                MessageNode* newNode = new MessageNode();
                if (newNode != 0) newNode->Init(newItem);
                if (pList->current == 0) pList->current = pList->tail;
                if (pList->head == 0) {
                    pList->head = newNode;
                    pList->tail = newNode;
                    pList->current = newNode;
                } else {
                    if (pList->tail == 0 || ((MessageNode*)pList->tail)->next != 0) WriteToMessageLog("queue fault 0113");
                    newNode->next = 0;
                    newNode->prev = (MessageNode*)pList->tail;
                    ((MessageNode*)pList->tail)->next = newNode;
                    pList->tail = newNode;
                }
                goto count_items;
            }
            pList->current = node->next;
        }
    } else {
        pList->InsertNode(newItem);
    }

count_items:
    count = 0;
    pList = m_messageList;
    pList->current = pList->head;
    if (pList->current != 0) {
        do {
            count++;
            if (pList->current == pList->tail) break;
            if (pList->current != 0) {
                pList->current = ((MessageNode*)pList->current)->next;
            }
        } while (pList->current != 0);
    }

    if (count > 10) {
        pvVar6 = 0;
        pList = m_messageList;
        if (pList->flags == 1 || pList->flags == 4) {
            pList->current = pList->head;
        } else if (pList->flags == 2 || pList->flags == 0) {
            pList->current = pList->tail;
        } else {
            WriteToMessageLog("bad queue type %lu", pList->flags);
            pList->current = pList->tail;
        }

        node = (MessageNode*)pList->current;
        if (node != 0) {
            pList->UnlinkNode(node);
            pvVar6 = (OnScreenMessage*)node->data;
            delete node;
            pList->current = 0;
            pList->current = pList->head;
            if (pvVar6 != 0) {
                Timer_DecrementCounter();
                delete pvVar6;
            }
        }
        goto count_items;
    }

    return 1;

send_remove_message:
    SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    return 1;
}

/* Function start: 0x40ABD0 */
void MessageList::InsertNode(void* data) {
    if (data == 0) {
        WriteToMessageLog("queue fault 0102");
    }
    MessageNode* newNode = new MessageNode();
    if (newNode != 0) {
        newNode->Init(data);
    }
    if (current == 0) {
        current = head;
    }
    if (head == 0) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        newNode->next = (MessageNode*)current;
        newNode->prev = ((MessageNode*)current)->prev;
        if (((MessageNode*)current)->prev == 0) {
            head = newNode;
        } else {
            ((MessageNode*)current)->prev->next = newNode;
        }
        ((MessageNode*)current)->prev = newNode;
    }
}

/* Function start: 0x40AD50 */
void MessageList::UnlinkNode(MessageNode* node) {
    if (head == node) {
        head = node->next;
    }
    if (tail == node) {
        tail = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    current = 0;
}
