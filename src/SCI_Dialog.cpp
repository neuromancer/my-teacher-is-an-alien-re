#include "SCI_Dialog.h"
#include "MMPlayer2.h"
#include "MouseControl.h"
#include "string.h"
#include "MMPlayer.h"
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

    WriteToMessageLogIfEnabled("\"declaring DialogPlayer\"");

    targetAddress = 9;
}

/* Function start: 0x407070 */
SCI_Dialog::~SCI_Dialog() {
    MMPlayer* mc;
    MMPlayer2* dlg;
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
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                node = (QueueNode*)queue->current;
                if (node == 0) {
                    dq = 0;
                } else {
                    if (queue->head == node) {
                        queue->head = node->next;
                    }
                    if (queue->tail == queue->current) {
                        queue->tail = ((QueueNode*)queue->current)->prev;
                    }
                    nextNode = ((QueueNode*)queue->current)->prev;
                    if (nextNode != 0) {
                        nextNode->next = ((QueueNode*)queue->current)->next;
                    }
                    nextNode = ((QueueNode*)queue->current)->next;
                    if (nextNode != 0) {
                        nextNode->prev = ((QueueNode*)queue->current)->prev;
                    }
                    node = (QueueNode*)queue->current;
                    dq = 0;
                    if (node != 0) {
                        dq = (DialogQuestion*)node->data;
                        delete node;
                        queue->current = 0;
                    }
                    queue->current = queue->head;
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
    Queue* pQueue;
    char buffer[32];

    WriteToMessageLog("\nENTER DIALOG");

    IconBar::Init(msg);

    struct DialogInitData {
        void* ptr1;
        void* ptr2;
    };

    DialogInitData* initData = (DialogInitData*)msg->userPtr;
    if (initData != 0 && msg->command == 0xb) {
        msg->userPtr = 0;
        field_600 = (MMPlayer*)initData->ptr1;
        field_604 = (MMPlayer2*)initData->ptr2;
        initData->ptr1 = 0;
        initData->ptr2 = 0;
        delete initData;
    }

    memset(field_618, 0x01, 10 * sizeof(int));

    sprintf(buffer, "mis\\dialog%2.2d.mis", msg->param1);

    pQueue = new Queue();
    field_610 = pQueue;

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

    WriteToMessageLogIfEnabled("\"SCI_Dialog::ShutDown\"");

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
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                dq = (DialogQuestion*)queue->RemoveCurrent();
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

    IconBar::ShutDown(msg);

    WriteToMessageLog("\"EXIT DIALOG\\n\"");

    return 0;
}

/* Function start: 0x4074E0 */
int SCI_Dialog::AddMessage(SC_Message* msg) {
    DialogQuestion* dq;
    InputState* pMouse;

    if (IconBar::AddMessage(msg)) {
        return 1;
    }

    if (field_614 != 0) {
        if (msg->lastKey == 0x1b) {
            field_614->Finalize();
            dq = field_614;
            if (dq->state == 2) {
                delete dq;
                field_614 = 0;
                return 1;
            }
        } else {
            if (msg->mouseY >= 2) {
                SC_Message_Send(command, data, 9, sourceAddress, 5, 6, 0, 0, 0, 0);
                return 1;
            }
        }
    } else {
        if (msg->mouseX >= 2) {
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
        if (msg->lastKey == 0x1b) {
            SC_Message_Send(command, data, 9, sourceAddress, 5, 6, 0, 0, 0, 0);
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

    if (msg->targetAddress != targetAddress) {
        return 0;
    }

    switch (msg->priority) {
    case 0:
        return 0;
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
                queue->current = queue->head;
                if (queue->type == 1 || queue->type == 2) {
                    if (queue->head == 0) {
                        if (dq == 0) ShowError("queue fault 0102");
                        newNode = new QueueNode(dq);
                        if (queue->current == 0) queue->current = queue->head;
                        if (queue->head == 0) {
                            queue->head = newNode;
                            queue->tail = newNode;
                            queue->current = newNode;
                        } else {
                            newNode->next = (QueueNode*)queue->current;
                            newNode->prev = ((QueueNode*)queue->current)->prev;
                            if (((QueueNode*)queue->current)->prev != 0) {
                                ((QueueNode*)((QueueNode*)queue->current)->prev)->next = newNode;
                            } else {
                                queue->head = newNode;
                            }
                            ((QueueNode*)queue->current)->prev = newNode;
                        }
                    } else {
                        while (1) {
                            curNode = (int)queue->current;
                            if (((DialogQuestion*)((QueueNode*)curNode)->data)->questionId < dq->questionId) {
                                if (dq == 0) ShowError("queue fault 0102");
                                newNode = new QueueNode(dq);
                                if (queue->current == 0) queue->current = queue->head;
                                if (queue->head == 0) {
                                    queue->head = newNode;
                                    queue->tail = newNode;
                                    queue->current = newNode;
                                } else {
                                    newNode->next = (QueueNode*)queue->current;
                                    newNode->prev = ((QueueNode*)queue->current)->prev;
                                    if (((QueueNode*)queue->current)->prev != 0) {
                                        ((QueueNode*)((QueueNode*)queue->current)->prev)->next = newNode;
                                    } else {
                                        queue->head = newNode;
                                    }
                                    ((QueueNode*)queue->current)->prev = newNode;
                                }
                                break;
                            }
                            if (queue->tail == (void*)curNode) {
                                if (dq == 0) ShowError("queue fault 0112");
                                newNode = new QueueNode(dq);
                                if (queue->current == 0) queue->current = queue->tail;
                                if (queue->head == 0) {
                                    queue->head = newNode;
                                    queue->tail = newNode;
                                    queue->current = newNode;
                                } else {
                                    if (queue->tail == 0 || ((QueueNode*)queue->tail)->next != 0) {
                                        ShowError("queue fault 0113");
                                    }
                                    newNode->next = 0;
                                    newNode->prev = (QueueNode*)queue->tail;
                                    ((QueueNode*)queue->tail)->next = newNode;
                                    queue->tail = newNode;
                                }
                                break;
                            }
                            if (curNode != 0) queue->current = ((QueueNode*)curNode)->next;
                            if (queue->current == 0) break;
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
        g_FlagManager_00435a84->SetFlag(msg->sourceAddress, 1);
        if (dq != 0) delete dq;
        break;

    }

    return 1;
}

/* Function start: 0x407A40 */
void SCI_Dialog::Update(int param1, int param2) {
    int y;
    int counter;
    int mouseIndex;
    QueueNode* node;
    DialogQuestion* dq;
    Sprite* spr;
    int x;

    y = 10;
    counter = 0;

    if (targetAddress != param2) return;

    IconBar::Update(param1, param2);
    if (field_600 != 0) field_600->Draw();
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

    if (field_610 == 0 || field_610->head == 0) {
        if (field_604 != 0) field_604->Draw();
        SC_Message_Send(command, data, 9, sourceAddress, 5, 6, 0, 0, 0, 0);
        return;
    }

    if (field_604 != 0) field_604->Draw();

    mouseIndex = -1;
    if (g_InputManager_00436968->pMouse != 0 && g_InputManager_00436968->pMouse->y >= 10) {
        mouseIndex = (g_InputManager_00436968->pMouse->y - 10) / 34;
    }

    field_610->current = field_610->head;
    if (field_610->head == 0) return;

    do {
        node = (QueueNode*)field_610->current;
        if (counter == mouseIndex) {
            dq = 0;
            if (node != 0) dq = (DialogQuestion*)node->data;
            dq->Update(18, y);
            spr = field_60C;
            if (spr != 0) {
                spr->Do(18, y, 1.0);
            }
        } else {
            dq = 0;
            if (node != 0) dq = (DialogQuestion*)node->data;
            dq->Update(18, y);
            spr = field_608;
            if (spr != 0) {
                spr->Do(18, y, 1.0);
            }
        }

        counter++;
        y += 34;

        if (field_610->tail == field_610->current) return;
        if (node != 0) field_610->current = node->next;
    } while (field_610->head != 0);
}

/* Function start: 0x407C50 */
DialogQuestion* SCI_Dialog::GetDialogByIndex(int index) {
    int counter;
    QueueNode* node;
    QueueNode* tmp;
    DialogQuestion* result;
    Queue* queue;

    counter = 0;
    queue = field_610;
    if (queue == 0) goto LAB_ret_zero;
    queue->current = queue->head;
    queue = field_610;
    if (queue->head == 0) goto LAB_ret_zero;

    do {
        queue = field_610;
        node = (QueueNode*)queue->current;
        if (counter != index) goto LAB_advance;
        if (node == 0) goto LAB_ret_zero;
        if (queue->head == node) queue->head = node->next;
        if (queue->tail == queue->current) queue->tail = ((QueueNode*)queue->current)->prev;
        tmp = ((QueueNode*)queue->current)->prev;
        if (tmp != 0) tmp->next = ((QueueNode*)queue->current)->next;
        tmp = ((QueueNode*)queue->current)->next;
        if (tmp != 0) tmp->prev = ((QueueNode*)queue->current)->prev;
        node = (QueueNode*)queue->current;
        result = 0;
        if (node != 0) result = (DialogQuestion*)node->data;
        if (node != 0) {
            node->data = 0;
            node->prev = 0;
            node->next = 0;
            operator delete(node);
            queue->current = 0;
        }
        queue->current = queue->head;
        return result;

    LAB_advance:
        if (queue->tail == node) goto LAB_ret_zero;
        if (node != 0) queue->current = node->next;
        counter++;
        queue = field_610;
    } while (queue->head != 0);

LAB_ret_zero:
    return 0;
}

/* Function start: 0x407D20 */
DialogQuestion* SCI_Dialog::FindDialogById(int id) {
    DialogQuestion* searchQuestion = new SC_Question(id);
    Queue* queue;
    QueueNode* node;
    QueueNode* tmp;
    DialogQuestion* nodeData;
    DialogQuestion* result;

    queue = field_610;
    if (queue == 0) goto LAB_cleanup;
    if (searchQuestion == 0) ShowError("queue fault 0103");

    queue->current = queue->head;
    if (queue->head == 0) goto LAB_cleanup;

    do {
        node = (QueueNode*)queue->current;
        nodeData = (node != 0) ? (DialogQuestion*)node->data : 0;
        if (nodeData->questionId != searchQuestion->questionId) goto LAB_advance;
        delete searchQuestion;

        queue = field_610;
        node = (QueueNode*)queue->current;
        if (node == 0) return 0;
        if (queue->head == node) queue->head = node->next;
        if (queue->tail == queue->current) queue->tail = ((QueueNode*)queue->current)->prev;
        tmp = ((QueueNode*)queue->current)->prev;
        if (tmp != 0) tmp->next = ((QueueNode*)queue->current)->next;
        tmp = ((QueueNode*)queue->current)->next;
        if (tmp != 0) tmp->prev = ((QueueNode*)queue->current)->prev;
        node = (QueueNode*)queue->current;
        result = 0;
        if (node != 0) result = (DialogQuestion*)node->data;
        if (node != 0) {
            node->data = 0;
            node->prev = 0;
            node->next = 0;
            operator delete(node);
            queue->current = 0;
        }
        queue->current = queue->head;
        return result;

    LAB_advance:
        if (queue->tail == node) goto LAB_cleanup;
        if (node != 0) queue->current = node->next;
    } while (queue->current != 0);

LAB_cleanup:
    if (searchQuestion != 0) delete searchQuestion;
    return 0;
}

/* Function start: 0x407EC6 */
int SCI_Dialog::LBLParse(char* line) {
    char token[32];
    char arg1[64];
    int questionId;
    DialogQuestion* dq;
    Queue* queue;
    QueueNode* newNode;
    int curNode;

    sscanf(line, "%s", token);

    if (strcmp(token, "BACKGROUND") == 0) {
        ShowError("SCI_Dialog::LBLParse %s, a background already exists", token);
    } else if (strcmp(token, "PALETTE") == 0) {
        ShowError("SCI_Dialog::LBLParse %s, a palette already exists", token);
        sscanf(line, "%s %s", token, arg1);
    } else if (strcmp(token, "BUTTON") == 0) {
        field_608 = new Sprite(0);
        Parser::ProcessFile(field_608, this, 0);
    } else if (strcmp(token, "AMBIENT_SPRS") == 0) {
        ShowError("SCI_Dialog::LBLParse %s, ambients already exist", token);
    } else if (strcmp(token, "HILITE") == 0) {
        field_60C = new Sprite(0);
        Parser::ProcessFile(field_60C, this, 0);
    } else if (strcmp(token, "HANDLE") == 0) {
        sscanf(line, "%s %d", token, &sourceAddress);
    } else if (strcmp(token, "AMBIENTSCONTROLLER") == 0) {
        int temp;
        int ambientVal;
        sscanf(line, "%s %d", token, &ambientVal);
        int count = 1;
        temp = ambientVal;
        for (int i_digits = 0; i_digits < 10; i_digits++) {
            if (ambientVal < 10) break;
            count++;
            ambientVal /= 10;
        }
        for (; count > 0; count--) {
            field_618[temp % 10] = 0;
            temp /= 10;
        }
    } else if (strcmp(token, "QUESTION") == 0) {
        sscanf(line, "%s %d ", token, &questionId);
        dq = new SC_Question(questionId);
        if (dq->state == 2) {
            delete dq;
        } else {
            queue = field_610;
            queue->ResetForSortedAdd(dq);
            if (queue->type == 1 || queue->type == 2) {
                if (queue->head == 0) {
                    queue->InsertNode(dq);
                } else {
                    do {
                        QueueNode* node = (QueueNode*)queue->current;
                        if (((DialogQuestion*)node->data)->questionId < dq->questionId) {
                            queue->InsertNode(dq);
                            return 0;
                        }
                        if (queue->tail == (void*)node) {
                            queue->PushNode(dq);
                            return 0;
                        }
                        if (node != 0) queue->current = node->next;
                    } while (queue->current != 0);
                }
            } else {
                queue->InsertNode(dq);
            }
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_Dialog");
    }

    return 0;
}

