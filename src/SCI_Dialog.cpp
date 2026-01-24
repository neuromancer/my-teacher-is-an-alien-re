#include "SCI_Dialog.h"
#include "SC_Dialog.h"
#include "Mouse.h"
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

/* Function start: 0x406FC0 */
SCI_Dialog::SCI_Dialog() {
    // Zero 0x14 dwords (80 bytes) at offset 0x600
    memset(&mouseControl, 0, 0x14 * sizeof(int));

    WriteToMessageLog("\"declaring DialogPlayer\"");

    handlerId = 9;
}

/* Function start: 0x407070 */
SCI_Dialog::~SCI_Dialog() {
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
void SCI_Dialog::Init(SC_Message* msg) {
    char buffer[32];
    Queue* local_14;
    Queue* queue;

    WriteToMessageLog("\nENTER DIALOG");

    IconBar::InitIconBar(msg);

    // Structure passed via userPtr from Handler11
    struct DialogInitData {
        void* ptr1;
        void* ptr2;
    };

    DialogInitData* initData = (DialogInitData*)msg->userPtr;
    if (initData != 0 && msg->targetAddress == 0xb) {
        msg->userPtr = 0;
        mouseControl = (MouseControl*)initData->ptr1;
        dialog = (SC_Dialog*)initData->ptr2;
        initData->ptr1 = 0;
        initData->ptr2 = 0;
        delete initData;
    }

    // Fill ambientState with 0x01010101
    memset(ambientState, 0x01, 10 * sizeof(int));

    sprintf(buffer, "mis\\dialog%2.2d.mis", msg->param1);

    local_14 = new Queue();
    if (local_14 == 0) {
        queue = 0;
    } else {
        queue = local_14;
    }

    dialogQueue = queue;

    // Disable choice and mainmenu buttons
    buttons[0].enabled = 0;
    buttons[4].enabled = 0;

    ParseFile(this, buffer, "[DIALOG%4.4d]", msg->sourceAddress);
}

/* Function start: 0x407380 */
int SCI_Dialog::Update(SC_Message* msg) {
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

    buttons[0].enabled = 1;
    buttons[4].enabled = 1;

    IconBar::CleanupIconBar();

    WriteToMessageLog("\"EXIT DIALOG\\n\"");

    return 0;
}

/* Function start: 0x4074E0 */
int SCI_Dialog::HandleMessage(SC_Message* msg) {
    DialogQuestion* dq;
    InputState* pMouse;

    if (IconBar::CheckButtonClick(msg)) {
        return 1;
    }

    if (currentDialog != 0) {
        if (msg->field_b4 == 0x1b) {
            currentDialog->Finalize();
            dq = currentDialog;
            if (dq->state == 2) {
                delete dq;
                currentDialog = 0;
                return 1;
            }
        } else {
            if (msg->mouseY > 1) {
                SC_Message_Send(field_90, field_94, 9, field_8C, 5, 6, 0, 0, 0, 0);
                return 1;
            }
        }
    } else {
        if (msg->mouseX > 1) {
            msg->targetAddress = 9;
            pMouse = g_InputManager_00436968->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                msg->sourceAddress = -1;
            } else {
                msg->sourceAddress = (pMouse->y - 10) / 0x22;
            }
            msg->priority = 3;
            return 1;
        }
        if (msg->field_b4 == 0x1b) {
            SC_Message_Send(field_90, field_94, 9, field_8C, 5, 6, 0, 0, 0, 0);
        }
    }

    return 1;
}

/* Function start: 0x407650 */
int SCI_Dialog::Exit(SC_Message* msg) {
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* newNode;
    int curNode;

    if (msg->targetAddress != handlerId) {
        return 0;
    }

    switch (msg->priority) {
    case 3:
        IconBar::PlayButtonSound(-1);
        if (currentDialog == 0) {
            currentDialog = GetDialogByIndex(msg->sourceAddress);
        }
        if (currentDialog != 0) {
            currentDialog->state = 1;
        }
        break;

    case 0x13:
        dq = FindDialogById(msg->sourceAddress);
        if (dq == 0) {
            dq = new SC_Question(msg->sourceAddress);
            if (dq->state == 2) {
                delete dq;
            } else {
                queue = dialogQueue;
                if (dq == 0) ShowError("queue fault 0101");
                queue->m_current = queue->m_head;
                if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2) {
                    if (queue->m_head == 0) {
                        if (dq == 0) ShowError("queue fault 0102");
                        newNode = new QueueNode(dq);
                        if (queue->m_current == 0) queue->m_current = queue->m_head;
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
                                if (dq == 0) ShowError("queue fault 0102");
                                newNode = new QueueNode(dq);
                                if (queue->m_current == 0) queue->m_current = queue->m_head;
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
                                if (dq == 0) ShowError("queue fault 0112");
                                newNode = new QueueNode(dq);
                                if (queue->m_current == 0) queue->m_current = queue->m_tail;
                                if (queue->m_head == 0) {
                                    queue->m_head = newNode;
                                    queue->m_tail = newNode;
                                    queue->m_current = newNode;
                                } else {
                                    newNode->prev = 0;
                                    newNode->next = (QueueNode*)queue->m_tail;
                                    ((QueueNode*)queue->m_tail)->prev = newNode;
                                    queue->m_tail = newNode;
                                }
                                break;
                            }
                            if (curNode != 0) queue->m_current = ((QueueNode*)curNode)->prev;
                            if (queue->m_current == 0) break;
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
        if (dq != 0) delete dq;
        break;

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x407A40 */
void SCI_Dialog::Draw(int param1, int param2) {
    void* data;
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* node;
    int counter;
    int y;
    int x;

    if (handlerId != param2) {
        return;
    }

    IconBar::Draw(param1, param2);

    if (mouseControl != 0) {
        mouseControl->DoAll();
    }

    g_Mouse_00436978->DrawCursor();

    if (currentDialog != 0) {
        currentDialog->Update(0x12, 10);
        if (buttonSprite != 0) {
            buttonSprite->Do(0x12, 10, 1.0);
        }

        dq = currentDialog;
        if (dq->state == 2) {
            delete dq;
            currentDialog = 0;
        }
    } else {
        queue = dialogQueue;
        if (queue != 0 && queue->m_head != 0) {
            queue->m_current = queue->m_head;
            counter = 0;
            y = 10;
            x = 10;
            while (1) {
                node = (QueueNode*)queue->m_current;
                data = (node != 0) ? node->data : 0;
                
                // local_4 logic from decompiled - determine if this is the highlighted choice
                int local_4;
                InputState* pMouse = g_InputManager_00436968->pMouse;
                if (pMouse == 0 || pMouse->y < 10) local_4 = -1;
                else local_4 = (pMouse->y - 10) / 0x22;

                if (counter == local_4) {
                    ((DialogQuestion*)data)->Update(0x12, y);
                    data = (void*)hiliteSprite;
                } else {
                    ((DialogQuestion*)data)->Update(0x12, y);
                    data = (void*)buttonSprite;
                }

                if (data != 0) {
                    ((Sprite*)data)->Do(0x12, x, 1.0);
                }

                counter++;
                y += 0x22;
                x += 0x22;
                if (queue->m_tail == queue->m_current) break;
                if (queue->m_current != 0) queue->m_current = ((QueueNode*)queue->m_current)->prev;
                if (queue->m_current == 0) return;
            }
        }
    }
}

/* Function start: 0x407C50 */
DialogQuestion* SCI_Dialog::GetDialogByIndex(int index) {
    int counter = 0;
    QueueNode* node;
    QueueNode* prevNode;
    QueueNode* nextNode;
    DialogQuestion* result;
    Queue* queue = dialogQueue;

    if (queue == 0) return 0;
    queue->m_current = queue->m_head;
    if (queue->m_head == 0) return 0;

    do {
        node = (QueueNode*)queue->m_current;
        if (counter == index) {
            if (node == 0) return 0;
            if (queue->m_head == node) queue->m_head = node->next;
            if (queue->m_tail == queue->m_current) queue->m_tail = node->prev;
            prevNode = node->prev;
            if (prevNode != 0) prevNode->next = node->next;
            nextNode = node->next;
            if (nextNode != 0) nextNode->prev = node->prev;
            
            result = (DialogQuestion*)node->data;
            node->data = 0;
            node->prev = 0;
            node->next = 0;
            FreeMemory(node);
            queue->m_current = 0;
            queue->m_current = queue->m_head;
            return result;
        }
        if (queue->m_tail == node) return 0;
        if (node != 0) queue->m_current = node->next;
        counter++;
    } while (queue->m_head != 0);

    return 0;
}

/* Function start: 0x407D20 */
DialogQuestion* SCI_Dialog::FindDialogById(int id) {
    DialogQuestion* searchQuestion = new SC_Question(id);
    Queue* queue = dialogQueue;
    QueueNode* node;
    QueueNode* prevNode;
    QueueNode* nextNode;
    DialogQuestion* nodeData;
    DialogQuestion* result = 0;

    if (queue == 0) goto LAB_cleanup;
    if (searchQuestion == 0) ShowError("queue fault 0103");

    queue->m_current = queue->m_head;
    if (queue->m_head == 0) goto LAB_cleanup;

    do {
        node = (QueueNode*)queue->m_current;
        nodeData = (node != 0) ? (DialogQuestion*)node->data : 0;
        if (nodeData->questionId == searchQuestion->questionId) {
            delete searchQuestion;
            searchQuestion = 0;

            if (node == 0) return 0;
            if (queue->m_head == node) queue->m_head = node->next;
            if (queue->m_tail == queue->m_current) queue->m_tail = node->prev;
            prevNode = node->prev;
            if (prevNode != 0) prevNode->next = node->next;
            nextNode = node->next;
            if (nextNode != 0) nextNode->prev = node->prev;

            result = (DialogQuestion*)node->data;
            node->data = 0;
            node->prev = 0;
            node->next = 0;
            FreeMemory(node);
            queue->m_current = 0;
            queue->m_current = queue->m_head;
            return result;
        }

        if (queue->m_tail == node) break;
        if (node != 0) queue->m_current = node->next;
    } while (queue->m_current != 0);

LAB_cleanup:
    if (searchQuestion != 0) delete searchQuestion;
    return 0;
}

/* Function start: 0x407EC6 */
int SCI_Dialog::LBLParse(char* line) {
    char token[32];
    char arg1[64];
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* newNode;
    int curNode;

    if (sscanf(line, "%s", token) != 1) return 0;

    if (strcmp(token, "BACKGROUND") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, a background already exists", token);
    } else if (strcmp(token, "PALETTE") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, a palette already exists", token);
        sscanf(line, "%s %s", token, arg1);
    } else if (strcmp(token, "BUTTON") == 0) {
        buttonSprite = new Sprite(0);
        Parser::ProcessFile(buttonSprite, this, 0);
    } else if (strcmp(token, "AMBIENT_SPRS") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, ambients already exist", token);
    } else if (strcmp(token, "HILITE") == 0) {
        hiliteSprite = new Sprite(0);
        Parser::ProcessFile(hiliteSprite, this, 0);
    } else if (strcmp(token, "AMBIENTSCONTROLLER") == 0) {
        int ambientVal;
        sscanf(line, "%s %d", token, &ambientVal);
        int temp = ambientVal;
        do {
            ambientState[temp % 10] = 0;
            temp /= 10;
        } while (temp != 0);
    } else if (strcmp(token, "QUESTION") == 0) {
        int questionId;
        sscanf(line, "%s %d", token, &questionId);
        dq = new SC_Question(questionId);
        if (dq->state == 2) {
            delete dq;
        } else {
            queue = dialogQueue;
            if (dq == 0) ShowError("queue fault 0101");
            queue->m_current = queue->m_head;
            if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2) {
                if (queue->m_head == 0) {
                    if (dq == 0) ShowError("queue fault 0102");
                    newNode = new QueueNode(dq);
                    if (queue->m_current == 0) queue->m_current = queue->m_head;
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
                            if (dq == 0) ShowError("queue fault 0102");
                            newNode = new QueueNode(dq);
                            if (queue->m_current == 0) queue->m_current = queue->m_head;
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
                            if (dq == 0) ShowError("queue fault 0112");
                            newNode = new QueueNode(dq);
                            if (queue->m_current == 0) queue->m_current = queue->m_tail;
                            if (queue->m_head == 0) {
                                queue->m_head = newNode;
                                queue->m_tail = newNode;
                                queue->m_current = newNode;
                            } else {
                                newNode->prev = 0;
                                newNode->next = (QueueNode*)queue->m_tail;
                                ((QueueNode*)queue->m_tail)->prev = newNode;
                                queue->m_tail = newNode;
                            }
                            break;
                        }
                        if (curNode != 0) queue->m_current = ((QueueNode*)curNode)->prev;
                        if (queue->m_current == 0) break;
                    }
                }
            } else {
                queue->InsertAtCurrent(dq);
            }
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        return Parser::LBLParse("SCI_Dialog");
    }

    return 0;
}

