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
    memset(&field_600, 0, 0x14 * sizeof(int));

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

    mc = field_600;
    if (mc != 0) {
        delete mc;
        field_600 = 0;
    }

    dlg = field_604;
    if (dlg != 0) {
        delete dlg;
        field_604 = 0;
    }

    spr = field_608;
    if (spr != 0) {
        delete spr;
        field_608 = 0;
    }

    spr = field_60C;
    if (spr != 0) {
        delete spr;
        field_60C = 0;
    }

    queue = field_610;
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
        field_610 = 0;
    }
}

/* Function start: 0x407240 */
void SCI_Dialog::Init(SC_Message* msg) {
    char buffer[32];
    Queue* local_14;
    Queue* queue;

    WriteToMessageLog("\nENTER DIALOG");

    IconBar::InitIconBar(msg);

    // Structure passed via userPtr from SCI_SearchScreen
    struct DialogInitData {
        void* ptr1;
        void* ptr2;
    };

    DialogInitData* initData = (DialogInitData*)msg->userPtr;
    if (initData != 0 && msg->targetAddress == 0xb) {
        msg->userPtr = 0;
        field_600 = (MouseControl*)initData->ptr1;
        field_604 = (SC_Dialog*)initData->ptr2;
        initData->ptr1 = 0;
        initData->ptr2 = 0;
        delete initData;
    }

    // Fill ambientState with 0x01010101
    memset(field_618, 0x01, 10 * sizeof(int));

    sprintf(buffer, "mis\\dialog%2.2d.mis", msg->param1);

    local_14 = new Queue();
    if (local_14 == 0) {
        queue = 0;
    } else {
        queue = local_14;
    }

    field_610 = queue;

    // Disable choice and mainmenu buttons
    buttons[0].enabled = 0;
    buttons[4].enabled = 0;

    ParseFile(this, buffer, "[DIALOG%4.4d]", msg->sourceAddress);
}

/* Function start: 0x407380 */
int SCI_Dialog::ShutDown(SC_Message* msg) {
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* node;
    QueueNode* nextNode;

    WriteToMessageLog("\"SCI_Dialog::ShutDown\"");

    if (field_608 != 0) {
        field_608->StopAnimationSound();
    }

    if (field_60C != 0) {
        field_60C->StopAnimationSound();
    }

    dq = field_614;
    field_600 = 0;
    field_604 = 0;
    if (dq != 0) {
        delete dq;
        field_614 = 0;
    }

    queue = field_610;
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
        field_610 = 0;
    }

    buttons[0].enabled = 1;
    buttons[4].enabled = 1;

    IconBar::CleanupIconBar();

    WriteToMessageLog("\"EXIT DIALOG\\n\"");

    return 0;
}

/* Function start: 0x4074E0 */
int SCI_Dialog::AddMessage(SC_Message* msg) {
    DialogQuestion* dq;
    InputState* pMouse;

    if (IconBar::CheckButtonClick(msg)) {
        return 1;
    }

    if (field_614 != 0) {
        if (msg->field_b4 == 0x1b) {
            field_614->Finalize();
            dq = field_614;
            if (dq->state == 2) {
                delete dq;
                field_614 = 0;
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
        if (field_614 == 0) {
            field_614 = GetDialogByIndex(msg->sourceAddress);
        }
        if (field_614 != 0) {
            field_614->state = 1;
        }
        break;

    case 0x13:
        dq = FindDialogById(msg->sourceAddress);
        if (dq == 0) {
            dq = new SC_Question(msg->sourceAddress);
            if (dq->state == 2) {
                delete dq;
            } else {
                queue = field_610;
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
                            newNode->next = (QueueNode*)queue->m_current;
                            newNode->prev = ((QueueNode*)queue->m_current)->prev;
                            if (((QueueNode*)queue->m_current)->prev == 0) {
                                queue->m_head = newNode;
                            } else {
                                ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                            }
                            ((QueueNode*)queue->m_current)->prev = newNode;
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
                                    newNode->next = (QueueNode*)queue->m_current;
                                    newNode->prev = ((QueueNode*)queue->m_current)->prev;
                                    if (((QueueNode*)queue->m_current)->prev == 0) {
                                        queue->m_head = newNode;
                                    } else {
                                        ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                                    }
                                    ((QueueNode*)queue->m_current)->prev = newNode;
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
                                    if (queue->m_tail == 0 || ((QueueNode*)queue->m_tail)->next != 0) {
                                        ShowError("queue fault 0113");
                                    }
                                    newNode->next = 0;
                                    newNode->prev = (QueueNode*)queue->m_tail;
                                    ((QueueNode*)queue->m_tail)->next = newNode;
                                    queue->m_tail = newNode;
                                }
                                break;
                            }
                            if (curNode != 0) queue->m_current = ((QueueNode*)curNode)->next;
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
void SCI_Dialog::Update(int param1, int param2) {
    int y = 10;
    int counter = 0;

    if (handlerId != param2) return;

    IconBar::Update(param1, param2);
    if (field_600 != 0) field_600->DoAll();
    g_Mouse_00436978->DrawCursor();

    if (field_614 != 0) {
        field_614->Update(18, 10);
        if (field_604 != 0) field_604->DrawWithStates(field_618);
        if (field_614->state == 2) {
            delete field_614;
            field_614 = 0;
        }
        return;
    }

    if (field_610 == 0 || field_610->m_head == 0) {
        if (field_604 != 0) field_604->Draw();
        SC_Message_Send(field_90, field_94, 9, field_8C, 5, 6, 0, 0, 0, 0);
        return;
    }

    if (field_604 != 0) field_604->Draw();

    int mouseIndex;
    InputState* pMouse = g_InputManager_00436968->pMouse;
    if (pMouse == 0 || pMouse->y < 10) {
        mouseIndex = -1;
    } else {
        mouseIndex = (pMouse->y - 10) / 34;
    }

    int x = 10;
    field_610->m_current = field_610->m_head;
    if (field_610->m_head != 0) {
        do {
            QueueNode* node = (QueueNode*)field_610->m_current;
            DialogQuestion* dq = (node != 0) ? (DialogQuestion*)node->data : 0;
            
            if (counter == mouseIndex) {
                dq->Update(18, y);
                if (field_60C != 0) field_60C->Do(18, x, 1.0);
            } else {
                dq->Update(18, y);
                if (field_608 != 0) field_608->Do(18, x, 1.0);
            }

            counter++;
            y += 34;
            x += 34;

            if (field_610->m_tail == field_610->m_current) return;
            if (node != 0) field_610->m_current = node->next;
        } while (field_610->m_head != 0);
    }
}

/* Function start: 0x407C50 */
DialogQuestion* SCI_Dialog::GetDialogByIndex(int index) {
    int counter = 0;
    QueueNode* node;
    QueueNode* prevNode;
    QueueNode* nextNode;
    DialogQuestion* result;
    Queue* queue = field_610;

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
    Queue* queue = field_610;
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

    sscanf(line, "%s", token);

    if (strcmp(token, "BACKGROUND") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, a background already exists", token);
    } else if (strcmp(token, "PALETTE") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, a palette already exists", token);
        sscanf(line, "%s %s", token, arg1);
    } else if (strcmp(token, "BUTTON") == 0) {
        field_608 = new Sprite(0);
        Parser::ProcessFile(field_608, this, 0);
    } else if (strcmp(token, "AMBIENT_SPRS") == 0) {
        WriteToMessageLog("SCI_Dialog::LBLParse %s, ambients already exist", token);
    } else if (strcmp(token, "HILITE") == 0) {
        field_60C = new Sprite(0);
        Parser::ProcessFile(field_60C, this, 0);
    } else if (strcmp(token, "HANDLE") == 0) {
        sscanf(line, "%s %d", token, &field_8C);
    } else if (strcmp(token, "AMBIENTSCONTROLLER") == 0) {
        int ambientVal;
        sscanf(line, "%s %d", token, &ambientVal);
        int temp = ambientVal;
        int count = 1;
        int i_digits = 0;
        do {
            if (temp < 10) break;
            temp /= 10;
            count++;
            i_digits++;
        } while (i_digits < 10);
        temp = ambientVal;
        for (int i = 0; i < count; i++) {
            field_618[temp % 10] = 0;
            temp /= 10;
        }
    } else if (strcmp(token, "QUESTION") == 0) {
        int questionId;
        sscanf(line, "%s %d ", token, &questionId);
        dq = new SC_Question(questionId);
        if (dq->state == 2) {
            delete dq;
        } else {
            queue = field_610;
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
                        newNode->next = (QueueNode*)queue->m_current;
                        newNode->prev = ((QueueNode*)queue->m_current)->prev;
                        if (((QueueNode*)queue->m_current)->prev == 0) {
                            queue->m_head = newNode;
                        } else {
                            ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                        }
                        ((QueueNode*)queue->m_current)->prev = newNode;
                    }
                } else {
                    while (1) {
                        QueueNode* node = (QueueNode*)queue->m_current;
                        if (((DialogQuestion*)node->data)->questionId < dq->questionId) {
                            if (dq == 0) ShowError("queue fault 0102");
                            newNode = new QueueNode(dq);
                            if (queue->m_current == 0) queue->m_current = queue->m_head;
                            if (queue->m_head == 0) {
                                queue->m_head = newNode;
                                queue->m_tail = newNode;
                                queue->m_current = newNode;
                            } else {
                                newNode->next = (QueueNode*)queue->m_current;
                                newNode->prev = ((QueueNode*)queue->m_current)->prev;
                                if (((QueueNode*)queue->m_current)->prev == 0) {
                                    queue->m_head = newNode;
                                } else {
                                    ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                                }
                                ((QueueNode*)queue->m_current)->prev = newNode;
                            }
                            break;
                        }
                        if (queue->m_tail == (void*)node) {
                            if (dq == 0) ShowError("queue fault 0112");
                            newNode = new QueueNode(dq);
                            if (queue->m_current == 0) queue->m_current = queue->m_tail;
                            if (queue->m_head == 0) {
                                queue->m_head = newNode;
                                queue->m_tail = newNode;
                                queue->m_current = newNode;
                            } else {
                                if (queue->m_tail == 0 || ((QueueNode*)queue->m_tail)->next != 0) {
                                    ShowError("queue fault 0113");
                                }
                                newNode->next = 0;
                                newNode->prev = (QueueNode*)queue->m_tail;
                                ((QueueNode*)queue->m_tail)->next = newNode;
                                queue->m_tail = newNode;
                            }
                            break;
                        }
                        if (node != 0) queue->m_current = node->next;
                        if (queue->m_current == 0) break;
                    }
                }
            } else {
                if (dq == 0) ShowError("queue fault 0102");
                newNode = new QueueNode(dq);
                if (queue->m_current == 0) queue->m_current = queue->m_head;
                if (queue->m_head == 0) {
                    queue->m_head = newNode;
                    queue->m_tail = newNode;
                    queue->m_current = newNode;
                } else {
                    newNode->next = (QueueNode*)queue->m_current;
                    newNode->prev = ((QueueNode*)queue->m_current)->prev;
                    if (((QueueNode*)queue->m_current)->prev == 0) {
                        queue->m_head = newNode;
                    } else {
                        ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                    }
                    ((QueueNode*)queue->m_current)->prev = newNode;
                }
            }
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_Dialog");
    }

    return 0;
}

