#include "Handler15.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_Question.h"
#include "GameState.h"
#include "StringTable.h"
#include "OnScreenMessage.h"

// External functions
extern int __fastcall FUN_0040a290(OnScreenMessage* item, int param);  // Check if message should be removed
extern int FUN_00420d90(void* stringTable, unsigned int index, char* outBuffer);  // Get string from StringTable

/* Function start: 0x40A2E0 */
Handler15::Handler15() {
    // Set handlerId to 15
    handlerId = 15;

    // Initialize timer
    timer.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
    list->flags = 1;
}

/* Function start: 0x40A410 */
Handler15::~Handler15() {
    MessageList* pList;
    MessageNode* node;
    OnScreenMessage* data;

    pList = list;
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
                    delete data;
                }
            }
        }
        FreeMemory(pList);
        list = 0;
    }
}

/* Function start: 0x40A5A0 */
void Handler15::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
    }
}

/* Function start: 0x40A7C0 */
int Handler15::Update(SC_Message* msg) {
    return 0;
}

/* Function start: 0x40A7D0 */
int Handler15::HandleMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_OnScreenMessage::AddMessage");
    return 1;
}

/* Function start: 0x40A5E0 */
void Handler15::Draw(int param1, int param2) {
    MessageNode* node;
    unsigned int uVar3;
    int iVar4;
    OnScreenMessage* msgItem;
    OnScreenMessage* pvVar6;
    MessageList* pList;
    int counter;

    pList = list;
    pList->current = pList->head;
    counter = 0;

    if (pList->head == 0) {
        goto check_timer;
    }

    do {
        msgItem = 0;
        pList = list;
        node = (MessageNode*)pList->current;
        if (node != 0) {
            msgItem = (OnScreenMessage*)node->data;
        }

        iVar4 = FUN_0040a290(msgItem, counter);
        if (iVar4 != 0) {
            pList = list;
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
                delete pvVar6;
            }
        }

        counter++;
        pList = list;
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
        pList = list;
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

/* Function start: 0x40A7E0 */
int Handler15::Exit(SC_Message* msg) {
    MessageList* pList;
    MessageNode* node;
    OnScreenMessage* newItem;
    OnScreenMessage* pvVar6;
    int count;

    if (msg->command != handlerId) {
        return 0;
    }

    timer.Reset();

    if (msg->priority == 0xf) {
        goto send_remove_message;
    }
    if (msg->priority == 0x13) {
        goto process_message;
    }
    if (msg->priority == 0x1b) {
        goto send_remove_message;
    }
    return 0;

process_message:
    // Priority 0x13 - add message
    newItem = 0;
    if (msg->data == 0) {
        if (msg->userPtr != 0) {
            newItem = new OnScreenMessage((char*)msg->userPtr, msg->param1);
            msg->userPtr = 0;
        }
    } else {
        if (g_Strings_00435a70 != 0) {
            int result = FUN_00420d90(g_Strings_00435a70, msg->data, g_Buffer_00436960);
            if (result != 0) {
                newItem = new OnScreenMessage(g_Buffer_00436960, msg->param1);
            }
        }
    }

    if (newItem != 0) {
        // Insert into list with priority ordering
        pList = list;
        pList->current = pList->head;
        if ((pList->flags == 1 || pList->flags == 2) && pList->head != 0) {
            // Priority-based insertion
            while (pList->current != 0) {
                node = (MessageNode*)pList->current;
                // Compare timer values
                unsigned int existingTime = ((OnScreenMessage*)node->data)->timer.Update();
                unsigned int newTime = newItem->timer.Update();
                if (existingTime < newTime) {
                    // Insert before current
                    goto insert_node;
                }
                if (pList->tail == node) {
                    // Append at end
                    goto append_node;
                }
                if (node != 0) {
                    pList->current = node->next;
                }
            }
        } else {
insert_node:
            // Insert at current position
            if (newItem == 0) {
                WriteToMessageLog("queue fault 0102");
            }
            MessageNode* newNode = (MessageNode*)AllocateMemory(0xc);
            if (newNode != 0) {
                newNode->data = newItem;
                newNode->prev = 0;
                newNode->next = 0;
            }
            if (pList->current == 0) {
                pList->current = pList->head;
            }
            if (pList->head == 0) {
                pList->head = newNode;
                pList->tail = newNode;
                pList->current = newNode;
            } else {
                newNode->next = (MessageNode*)pList->current;
                newNode->prev = ((MessageNode*)pList->current)->prev;
                if (((MessageNode*)pList->current)->prev == 0) {
                    pList->head = newNode;
                    ((MessageNode*)pList->current)->prev = newNode;
                } else {
                    ((MessageNode*)pList->current)->prev->next = newNode;
                    ((MessageNode*)pList->current)->prev = newNode;
                }
            }
            goto count_and_trim;
        }
append_node:
        if (newItem == 0) {
            WriteToMessageLog("queue fault 0112");
        }
        MessageNode* newNode = (MessageNode*)AllocateMemory(0xc);
        if (newNode != 0) {
            newNode->data = newItem;
            newNode->prev = 0;
            newNode->next = 0;
        }
        if (pList->current == 0) {
            pList->current = pList->tail;
        }
        if (pList->head == 0) {
            pList->head = newNode;
            pList->tail = newNode;
            pList->current = newNode;
        } else {
            if (pList->tail == 0 || ((MessageNode*)pList->tail)->next != 0) {
                WriteToMessageLog("queue fault 0113");
            }
            newNode->next = 0;
            newNode->prev = (MessageNode*)pList->tail;
            ((MessageNode*)pList->tail)->next = newNode;
            pList->tail = newNode;
        }
    }

count_and_trim:
    // Count items and trim if over 10
    count = 0;
    pList = list;
    pList->current = pList->head;
    while (pList->current != 0) {
        count++;
        node = (MessageNode*)pList->current;
        if (pList->tail == node) {
            break;
        }
        if (node != 0) {
            pList->current = node->next;
        }
    }

    while (count > 10) {
        // Remove oldest item
        pList = list;
        if (pList->flags == 1 || pList->flags == 4) {
            pList->current = pList->head;
        } else if (pList->flags == 2 || pList->flags == 0) {
            pList->current = pList->tail;
        } else {
            WriteToMessageLog("bad queue type %lu", pList->flags);
        }

        node = (MessageNode*)pList->current;
        if (node != 0) {
            // Unlink node
            if (pList->head == node) {
                pList->head = node->next;
            }
            if (pList->tail == node) {
                pList->tail = node->prev;
            }
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            pvVar6 = 0;
            if (node != 0) {
                pvVar6 = (OnScreenMessage*)node->data;
                delete node;
                pList->current = 0;
            }
            pList->current = pList->head;

            if (pvVar6 != 0) {
                delete pvVar6;
            }
        }

        // Recount
        count = 0;
        pList->current = pList->head;
        while (pList->current != 0) {
            count++;
            node = (MessageNode*)pList->current;
            if (pList->tail == node) {
                break;
            }
            if (node != 0) {
                pList->current = node->next;
            }
        }
    }

    return 1;

send_remove_message:
    SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    return 1;
}
