#include "Handler9.h"
#include "string.h"
#include "MouseControl.h"
#include "Sprite.h"
#include "Queue.h"
#include "Memory.h"
#include "Parser.h"
#include "SC_Question.h"
#include "Message.h"
#include "globals.h"
#include "InputManager.h"
#include "FlagArray.h"
#include <string.h>
#include <stdio.h>

// SC_Question::~SC_Question() at 0x4067e0
// SC_Question::Finalize() at 0x4069b0
// SC_Question::SC_Question(int) at 0x4066d0
// All implemented in SC_Question.cpp

// QueueNode constructor (0x408860)
extern QueueNode* __fastcall QueueNode_Constructor(void* mem, void* data);

// Structure passed via userPtr from Handler11
struct DialogInitData {
    void* ptr1;
    void* ptr2;
};

/* Function start: 0x406FC0 */
Handler9::Handler9() {
    // Zero 0x14 dwords (80 bytes) at offset 0x600
    memset(&mouseControl, 0, 0x14 * sizeof(int));

    WriteToMessageLog("\"declaring DialogPlayer\"");

    handlerId = 9;
}

/* Function start: 0x407070 */
Handler9::~Handler9() {
    MouseControl* mc;
    SC_Dialog* dlg;
    Sprite* spr;
    Queue* queue;
    QueueNode* node;
    QueueNode* nextNode;
    DialogQuestion* dq;

    mc = mouseControl;
    if (mc != 0) {
        delete mc;
        mouseControl = 0;
    }

    dlg = dialog;
    if (dlg != 0) {
        delete dlg;
        dialog = 0;
    }

    spr = buttonSprite;
    if (spr != 0) {
        delete spr;
        buttonSprite = 0;
    }

    spr = hiliteSprite;
    if (spr != 0) {
        delete spr;
        hiliteSprite = 0;
    }

    queue = dialogQueue;
    if (queue != 0) {
        if (queue->m_head != 0) {
            queue->m_current = queue->m_head;
            while (queue->m_head != 0) {
                node = (QueueNode*)queue->m_current;
                if (node == 0) {
                    dq = 0;
                } else {
                    if (queue->m_head == node) {
                        queue->m_head = node->next;
                    }
                    if (queue->m_tail == queue->m_current) {
                        queue->m_tail = ((QueueNode*)queue->m_current)->prev;
                    }
                    nextNode = ((QueueNode*)queue->m_current)->prev;
                    if (nextNode != 0) {
                        nextNode->next = ((QueueNode*)queue->m_current)->next;
                    }
                    nextNode = ((QueueNode*)queue->m_current)->next;
                    if (nextNode != 0) {
                        nextNode->prev = ((QueueNode*)queue->m_current)->prev;
                    }
                    node = (QueueNode*)queue->m_current;
                    dq = 0;
                    if (node != 0) {
                        dq = (DialogQuestion*)node->data;
                        delete node;
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (dq != 0) {
                    delete dq;
                }
            }
        }
        delete queue;
        dialogQueue = 0;
    }
}

/* Function start: 0x407240 */
void Handler9::Init(SC_Message* msg) {
    char buffer[32];
    Queue* local_14;
    DialogInitData* initData;
    Queue* queue;

    WriteToMessageLog("\nENTER DIALOG");

    IconBar::InitIconBar(msg);

    initData = (DialogInitData*)msg->userPtr;
    if (initData != 0 && msg->targetAddress == 0xb) {
        msg->userPtr = 0;
        mouseControl = (MouseControl*)initData->ptr1;
        dialog = (SC_Dialog*)initData->ptr2;
        initData->ptr1 = 0;
        initData->ptr2 = 0;
        if (initData != 0) {
            delete initData;
        }
    }

    // Fill ambientState with 0x1010101
    memset(ambientState, 0x01, 10 * sizeof(int));

    sprintf(buffer, "mis\\dialog%2.2d.mis", msg->param1);

    // Allocate and initialize Queue - using new triggers SEH
    local_14 = new Queue();
    if (local_14 == 0) {
        queue = 0;
    } else {
        queue = local_14;
    }

    dialogQueue = queue;

    // Disable choice and mainmenu buttons
    Handler9::buttons[0].enabled = 0;
    Handler9::buttons[4].enabled = 0;

    ParseFile(this, buffer, "[DIALOG%4.4d]", msg->sourceAddress);
}

/* Function start: 0x407380 */
int Handler9::Update(SC_Message* msg) {
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* node;
    QueueNode* nextNode;

    WriteToMessageLog("\"SCI_Dialog::ShutDown\"");

    if (buttonSprite != 0) {
        buttonSprite->StopAnimationSound();
    }

    if (hiliteSprite != 0) {
        hiliteSprite->StopAnimationSound();
    }

    dq = currentDialog;
    mouseControl = 0;
    dialog = 0;
    if (dq != 0) {
        delete dq;
        currentDialog = 0;
    }

    queue = dialogQueue;
    if (queue != 0) {
        if (queue->m_head != 0) {
            queue->m_current = queue->m_head;
            while (queue->m_head != 0) {
                node = (QueueNode*)queue->m_current;
                if (node == 0) {
                    dq = 0;
                } else {
                    if (queue->m_head == node) {
                        queue->m_head = node->next;
                    }
                    if (queue->m_tail == queue->m_current) {
                        queue->m_tail = ((QueueNode*)queue->m_current)->prev;
                    }
                    nextNode = ((QueueNode*)queue->m_current)->prev;
                    if (nextNode != 0) {
                        nextNode->next = ((QueueNode*)queue->m_current)->next;
                    }
                    nextNode = ((QueueNode*)queue->m_current)->next;
                    if (nextNode != 0) {
                        nextNode->prev = ((QueueNode*)queue->m_current)->prev;
                    }
                    node = (QueueNode*)queue->m_current;
                    dq = 0;
                    if (node != 0) {
                        dq = (DialogQuestion*)node->data;
                        delete node;
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (dq != 0) {
                    delete dq;
                }
            }
        }
        delete queue;
        dialogQueue = 0;
    }

    Handler9::buttons[0].enabled = 1;
    Handler9::buttons[4].enabled = 1;

    IconBar::CleanupIconBar();

    WriteToMessageLog("\"EXIT DIALOG\\n\"");

    return 0;
}

/* Function start: 0x4074E0 */
int Handler9::HandleMessage(SC_Message* msg) {
    int result;
    DialogQuestion* dq;
    InputState* pMouse;

    result = IconBar::CheckButtonClick(msg);
    if (result != 0) {
        return 1;
    }

    if (currentDialog != 0) {
        if (msg->field_b4 == 0x1b) {
            currentDialog->Finalize();
            dq = currentDialog;
            if (dq->state == 2) {
                if (dq != 0) {
                    delete dq;
                    currentDialog = 0;
                    return 1;
                }
            }
        } else {
            if (msg->mouseY > 1) {
                SC_Message_Send(Handler9::field_90, Handler9::field_94, 9, Handler9::field_8C, 5, 6, 0, 0, 0, 0);
                return 1;
            }
        }
    } else {
        if (msg->mouseX > 1) {
            msg->command = 9;
            pMouse = g_InputManager_00436968->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                msg->sourceAddress = -1;
            } else if (pMouse == 0) {
                msg->sourceAddress = 0;
            } else {
                msg->sourceAddress = (pMouse->y - 10) / 0x22;
            }
            msg->priority = 3;
            return 1;
        }
        if (msg->field_b4 == 0x1b) {
            SC_Message_Send(Handler9::field_90, Handler9::field_94, 9, Handler9::field_8C, 5, 6, 0, 0, 0, 0);
        }
    }

    return 1;
}

void Handler9::Draw(int param1, int param2) {
}

/* Function start: 0x407650 */
int Handler9::Exit(SC_Message* msg) {
    void* mem;
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* node;
    QueueNode* newNode;
    int curNode;

    if (Handler9::handlerId != msg->command) {
        return 0;
    }

    switch (msg->priority) {
    case 0:
        break;

    case 3:
        IconBar::PlayButtonSound(-1);
        if (currentDialog == 0) {
            dq = GetDialogByIndex(msg->sourceAddress);
            currentDialog = dq;
        }
        if (currentDialog != 0) {
            currentDialog->state = 1;
        }
        break;

    case 0x13:
        dq = FindDialogById(msg->sourceAddress);
        if (dq == 0) {
            mem = new SC_Question(msg->sourceAddress);
            if (mem == 0) {
                dq = 0;
            } else {
                dq = (DialogQuestion*)mem;
            }

            if (dq->state == 2) {
                if (dq != 0) {
                    delete dq;
                }
            } else {
                queue = dialogQueue;
                if (dq == 0) {
                    ShowError("queue fault 0101");
                }
                queue->m_current = queue->m_head;
                if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2) {
                    if (queue->m_head == 0) {
                        if (dq == 0) {
                            ShowError("queue fault 0102");
                        }
                        mem = new QueueNode(dq);
                        newNode = 0;
                        if (mem != 0) {
                            newNode = (QueueNode*)mem;
                        }
                        if (queue->m_current == 0) {
                            queue->m_current = queue->m_head;
                        }
                        if (queue->m_head == 0) {
                            queue->m_head = newNode;
                            queue->m_tail = newNode;
                            queue->m_current = newNode;
                        } else {
                            newNode->prev = (QueueNode*)queue->m_current;
                            newNode->next = ((QueueNode*)queue->m_current)->next;
                            if (((QueueNode*)queue->m_current)->next == 0) {
                                queue->m_head = newNode;
                                ((QueueNode*)queue->m_current)->next = newNode;
                            } else {
                                ((QueueNode*)queue->m_current)->next->prev = newNode;
                                ((QueueNode*)queue->m_current)->next = newNode;
                            }
                        }
                    } else {
                        while (1) {
                            curNode = (int)queue->m_current;
                            if (((DialogQuestion*)((QueueNode*)curNode)->data)->questionId < dq->questionId) {
                                if (dq == 0) {
                                    ShowError("queue fault 0102");
                                }
                                mem = new QueueNode(dq);
                                newNode = 0;
                                if (mem != 0) {
                                    newNode = (QueueNode*)mem;
                                }
                                if (queue->m_current == 0) {
                                    queue->m_current = queue->m_head;
                                }
                                if (queue->m_head == 0) {
                                    queue->m_head = newNode;
                                    queue->m_tail = newNode;
                                    queue->m_current = newNode;
                                } else {
                                    newNode->prev = (QueueNode*)queue->m_current;
                                    newNode->next = ((QueueNode*)queue->m_current)->next;
                                    if (((QueueNode*)queue->m_current)->next == 0) {
                                        queue->m_head = newNode;
                                        ((QueueNode*)queue->m_current)->next = newNode;
                                    } else {
                                        ((QueueNode*)queue->m_current)->next->prev = newNode;
                                        ((QueueNode*)queue->m_current)->next = newNode;
                                    }
                                }
                                break;
                            }
                            if (queue->m_tail == (void*)curNode) {
                                if (dq == 0) {
                                    ShowError("queue fault 0112");
                                }
                                mem = new QueueNode(dq);
                                if (mem == 0) {
                                    newNode = 0;
                                } else {
                                    newNode = (QueueNode*)mem;
                                }
                                if (queue->m_current == 0) {
                                    queue->m_current = queue->m_tail;
                                }
                                if (queue->m_head == 0) {
                                    queue->m_head = newNode;
                                    queue->m_tail = newNode;
                                    queue->m_current = newNode;
                                } else {
                                    if (queue->m_tail == 0 || ((QueueNode*)queue->m_tail)->prev != 0) {
                                        ShowError("queue fault 0113");
                                    }
                                    newNode->prev = 0;
                                    newNode->next = (QueueNode*)queue->m_tail;
                                    ((QueueNode*)queue->m_tail)->prev = newNode;
                                    queue->m_tail = newNode;
                                }
                                break;
                            }
                            if (curNode != 0) {
                                queue->m_current = ((QueueNode*)curNode)->prev;
                            }
                            if (queue->m_current == 0) {
                                break;
                            }
                        }
                    }
                } else {
                    queue->InsertAtCurrent(dq);
                }
            }
        }
        break;

    case 0x14:
        dq = FindDialogById(msg->sourceAddress);
        g_Manager_00435a84->SetFlag(msg->sourceAddress, 1);
        if (dq != 0) {
            delete dq;
        }
        break;

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x407C50 */
DialogQuestion* Handler9::GetDialogByIndex(int index)
{
    int counter;
    QueueNode* node;
    QueueNode* prevNode;
    QueueNode* nextNode;
    DialogQuestion* result;

    counter = 0;
    Queue* queue = dialogQueue;
    if (queue == 0) {
        return 0;
    }
    queue->m_current = queue->m_head;
    if (queue->m_head == (void*)counter) {
        return 0;
    }

    do {
        queue = dialogQueue;
        node = (QueueNode*)queue->m_current;
        if (counter == index) {
            if (node == 0) {
                return 0;
            }
            if (queue->m_head == node) {
                queue->m_head = node->next;
            }
            if (queue->m_tail == queue->m_current) {
                queue->m_tail = ((QueueNode*)queue->m_current)->prev;
            }
            prevNode = ((QueueNode*)queue->m_current)->prev;
            if (prevNode != 0) {
                prevNode->next = ((QueueNode*)queue->m_current)->next;
            }
            nextNode = ((QueueNode*)queue->m_current)->next;
            if (nextNode != 0) {
                nextNode->prev = ((QueueNode*)queue->m_current)->prev;
            }
            node = (QueueNode*)queue->m_current;
            result = 0;
            if (node != 0) {
                result = (DialogQuestion*)node->data;
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                FreeMemory(node);
                queue->m_current = 0;
            }
            queue->m_current = queue->m_head;
            return result;
        }
        if (queue->m_tail == node) {
            return 0;
        }
        if (node != 0) {
            queue->m_current = node->next;
        }
        counter++;
        queue = dialogQueue;
    } while (queue->m_head != 0);

    return 0;
}

/* Function start: 0x407D20 */
DialogQuestion* Handler9::FindDialogById(int id)
{
    DialogQuestion* searchQuestion;
    Queue* queue;
    QueueNode* node;
    QueueNode* prevNode;
    QueueNode* nextNode;
    DialogQuestion* nodeData;
    DialogQuestion* result;

    searchQuestion = new SC_Question(id);

    queue = dialogQueue;
    if (queue == 0) {
        goto LAB_cleanup;
    }

    if (searchQuestion == 0) {
        ShowError("queue fault 0103");
    }

    queue->m_current = queue->m_head;
    if (queue->m_head == 0) {
        goto LAB_cleanup;
    }

    queue = dialogQueue;
    do {
        node = (QueueNode*)queue->m_current;
        nodeData = 0;
        if (node != 0) {
            nodeData = (DialogQuestion*)node->data;
        }
        if (nodeData->questionId == searchQuestion->questionId) {
            if (searchQuestion != 0) {
                searchQuestion->~SC_Question();
                FreeMemory(searchQuestion);
            }

            node = (QueueNode*)queue->m_current;
            if (node == 0) {
                return 0;
            }
            if (queue->m_head == node) {
                queue->m_head = node->next;
            }
            if (queue->m_tail == queue->m_current) {
                queue->m_tail = ((QueueNode*)queue->m_current)->prev;
            }
            prevNode = ((QueueNode*)queue->m_current)->prev;
            if (prevNode != 0) {
                prevNode->next = ((QueueNode*)queue->m_current)->next;
            }
            nextNode = ((QueueNode*)queue->m_current)->next;
            if (nextNode != 0) {
                nextNode->prev = ((QueueNode*)queue->m_current)->prev;
            }
            node = (QueueNode*)queue->m_current;
            result = 0;
            if (node != 0) {
                result = (DialogQuestion*)node->data;
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                FreeMemory(node);
                queue->m_current = 0;
            }
            queue->m_current = queue->m_head;
            return result;
        }

        if (queue->m_tail == node) {
            break;
        }
        if (node != 0) {
            queue->m_current = node->next;
        }
    } while (queue->m_current != 0);

LAB_cleanup:
    if (searchQuestion != 0) {
        searchQuestion->~SC_Question();
        FreeMemory(searchQuestion);
    }
    return 0;
}
