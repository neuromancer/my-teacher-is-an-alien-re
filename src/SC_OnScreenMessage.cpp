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
        m_messageList->type = 1;
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
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        ::operator delete(node);
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
        ::operator delete(pList);
        m_messageList = 0;
    }
}

/* Function start: 0x40A5A0 */
void SC_OnScreenMessage::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != (SC_Message*)0) {
        field_8C = msg->sourceAddress;
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
                // Get data from node
                node = (MessageNode*)pList->current;
                pvVar6 = 0;
                if (node != 0) {
                    pvVar6 = (OnScreenMessage*)node->data;
                }
                // Free node
                if (node != 0) {
                    node->data = 0;
                    node->prev = 0;
                    node->next = 0;
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
        ShowError("SC_OnScreenMessage::Update");
    }
}

/* Function start: 0x40A7C0 */
int SC_OnScreenMessage::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    ShowError("SC_OnScreenMessage::AddMessage");
    return 1;
}

/* Function start: 0x40A7E0 */
int SC_OnScreenMessage::Exit(SC_Message* msg) {
    OnScreenMessage* newItem;
    MessageList* list;

    newItem = 0;

    if (msg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    if (msg->priority == 0xF) goto send_remove_msg;
    if (msg->priority == 0x13) goto create_message;
    if (msg->priority == 0x1B) goto send_remove_msg;
    return 0;

create_message:
    if (msg->sourceAddress != 0) {
        if (g_Strings_00435a70 != (StringTable*)0) {
            if (g_Strings_00435a70->GetString(msg->sourceAddress, g_Buffer_00436960)) {
                newItem = new OnScreenMessage(g_Buffer_00436960, msg->param1);
            }
        }
        goto check_newitem;
    }
    if (msg->userPtr != 0) {
        newItem = new OnScreenMessage((char*)msg->userPtr, msg->param1);
        msg->userPtr = 0;
    }

check_newitem:
    if (newItem == 0) {
        goto count_loop_start;
    }

    list = m_messageList;
    list->current = list->head;
    if (list->type == 1) goto insertion_loop;
    if (list->type == 2) goto insertion_loop;

call_insert:
    list->InsertNode(newItem);

count_loop_start:
    {
        int count;
        list = m_messageList;
        count = 0;
        list->current = list->head;
        if (list->current != 0) {
            do {
                count++;
                if (list->current == list->tail) break;
                if (list->current != 0) {
                    list->current = ((MessageNode*)list->current)->next;
                }
            } while (list->current != 0);
        }

        if (count <= 10) {
            return 1;
        }
    }

    {
        OnScreenMessage* deletedItem;
        MessageNode* node;

        deletedItem = 0;
        list = m_messageList;
        if (list->type == 1 || list->type == 4) {
            list->current = list->head;
        } else if (list->type == 2 || list->type == 0) {
            list->current = list->tail;
        } else {
            ShowError("bad queue type %lu", list->type);
        }

        node = (MessageNode*)list->current;
        if (node != 0) {
            if (list->head == node) {
                list->head = node->next;
            }
            if (list->tail == (MessageNode*)list->current) {
                list->tail = ((MessageNode*)list->current)->prev;
            }
            node = (MessageNode*)list->current;
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            node = (MessageNode*)list->current;
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            node = (MessageNode*)list->current;
            if (node == 0) {
                deletedItem = 0;
            } else {
                deletedItem = (OnScreenMessage*)node->data;
            }
            if (node != 0) {
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                delete node;
                list->current = 0;
            }
            list->current = list->head;
        }

        if (deletedItem != 0) {
            delete deletedItem;
        }
    }
    goto count_loop_start;

insertion_loop:
    if (((OnScreenMessage*)((MessageNode*)list->current)->data)->timer.Update() < newItem->timer.Update()) {
        goto call_insert;
    }
    if (list->tail == list->current) {
        goto push_to_tail;
    }
    if (list->current != 0) {
        list->current = ((MessageNode*)list->current)->next;
    }
    if (list->current != 0) {
        goto insertion_loop;
    }
    goto count_loop_start;

push_to_tail:
    if (newItem == 0) ShowError("queue fault 0112");
    {
        MessageNode* newNode = new MessageNode(newItem);
        if (newNode != 0) {
            if (list->current == 0) {
                list->current = list->tail;
            }
            if (list->head == 0) {
                list->head = newNode;
                list->tail = newNode;
                list->current = newNode;
            } else {
                if (list->tail == 0 || ((MessageNode*)list->tail)->next != 0) {
                    ShowError("queue fault 0113");
                }
                newNode->next = 0;
                newNode->prev = (MessageNode*)list->tail;
                ((MessageNode*)list->tail)->next = newNode;
                list->tail = newNode;
            }
        }
    }
    goto count_loop_start;

send_remove_msg:
    SC_Message_Send(3, handlerId, handlerId, field_8C, 20, 0, 0, 0, 0, 0);
    return 1;
}

/* Function start: 0x40ABD0 */
void MessageList::InsertNode(void* data) {
    if (data == 0) {
        ShowError("queue fault 0102");
    }
    MessageNode* newNode = new MessageNode(data);
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

