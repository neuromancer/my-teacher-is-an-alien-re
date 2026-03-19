#include "Handler31.h"
#include <string.h>
#include "Sprite.h"
#include "Palette.h"
#include "SC_Question.h"
#include "LinkedList.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

#include "InputManager.h"
#include "MouseControl.h"
#include "GameState.h"
#include "FlagArray.h"
extern InputManager* DAT_0046aa08;
extern MouseControl* DAT_0046aa18;
extern "C" extern GameState* DAT_0046aa30;
extern char* DAT_0046aa00;
extern int DAT_00473334;
extern int DAT_004733e8;
extern FlagArray* DAT_0046a6e8;

extern void ResetSpriteStates();

/* Function start: 0x418060 */
/* Function start: 0x418086 */
int Handler31::LBLParse(char* line) {
    char token[32];
    char arg2[32];
    int id;
    SC_Question* question;
    LinkedList* list;
    ListNode* node;
    int count;
    int sscanfResult;

    sscanf(line, " %s ", token);

    if (strcmp(token, "HANDLE") == 0) {
        sscanf(line, " %s %d", token, &moduleParam);
    } else if (strcmp(token, "PALETTE") == 0) {
        sscanf(line, "%s %s", token, arg2);
        if (field_B4 != 0) {
            delete field_B4;
            field_B4 = 0;
        }
        field_B4 = new Palette();
        field_B4->Load(arg2);
    } else if (strcmp(token, "QUESTION") == 0) {
        sscanf(line, " %s %d ", token, &id);
        question = new SC_Question(id, (SCI_Dialog*)this);

        if (question->state != 2) {
            list = field_C8;
            count = 0;
            node = list->head;
            list->current = node;
            while (node != 0) {
                count++;
                if (list->tail == node) break;
                if (node != 0) {
                    list->current = node->next;
                }
                node = list->current;
            }

            if (count < 0xD) {
                list = field_C8;
                if (question == 0) {
                    ShowError("queue fault 0112");
                }
                ListNode* newNode = new ListNode((void*)question);
                if (list->current == 0) {
                    list->current = list->tail;
                }
                if (list->head == 0) {
                    list->head = newNode;
                    list->tail = newNode;
                    list->current = newNode;
                } else {
                    if (list->tail == 0 || list->tail->next != 0) {
                        ShowError("queue fault 0113");
                    }
                    newNode->next = 0;
                    newNode->prev = list->tail;
                    list->tail->next = newNode;
                    list->tail = newNode;
                }
                return 0;
            }
        }

        if (question != 0) {
            question->~SC_Question();
            FreeMemory(question);
        }
    } else if (strcmp(token, "PLACEHOLDER") == 0) {
        sscanfResult = sscanf(line, " %s %d %s", token, &id, arg2);

        if ((field_C8)->head == 0) {
            field_C4 = new SC_Question(id, (SCI_Dialog*)this);
        }

        DAT_0046a6e8->SetFlag(id, 4);

        if (sscanfResult == 3) {
            if (strcmp(arg2, "CONSTANT") != 0) {
                DAT_0046a6e8->SetFlag(id, 4);
            }
            if (strcmp(arg2, "SINGLE_PLAY") != 0) {
                DAT_0046a6e8->ClearFlag(id, 4);
            }
        }

        question = field_C4;
        if (question != 0 && question->state == 2 && question != 0) {
            question->~SC_Question();
            FreeMemory(question);
            field_C4 = 0;
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SCI_Dialog");
    }

    return 0;
}

/* Function start: 0x417D50 */
int Handler31::CheckDuplicateQuestion(int param) {
    SC_Question* question;
    LinkedList* list;
    ListNode* node;
    SC_Question* nodeData;

    question = new SC_Question(param, (SCI_Dialog*)this);

    list = field_C8;
    if (list != 0) {
        if (question == 0) {
            ShowError("queue fault 0103");
        }
        node = list->head;
        list->current = node;
        if (node != 0) {
            do {
                node = list->current;
                nodeData = 0;
                if (node != 0) {
                    nodeData = (SC_Question*)node->data;
                }
                if (nodeData->questionId == question->questionId) {
                    if (question != 0) {
                        question->~SC_Question();
                        FreeMemory(question);
                    }
                    return 0;
                }
                if (list->tail == node) break;
                if (node != 0) {
                    list->current = node->next;
                }
            } while (list->current != 0);
        }
    }

    if (question != 0) {
        question->~SC_Question();
        FreeMemory(question);
    }
    return 1;
}

/* Function start: 0x416E70 */
Handler31::Handler31() {
    memset(&field_A8, 0, 40);
    handlerId = 0x1F;

    sprite1 = new Sprite("\"elements\\option2.smk\"");
    sprite1->priority = 0x3E9;
    sprite1->flags |= 0x40;

    sprite2 = new Sprite("\"elements\\option2h.smk\"");
    sprite2->priority = 0x3E9;
    sprite2->flags |= 0x40;
}

/* Function start: 0x416FC0 */
Handler31::~Handler31() {
    if (sprite1 != 0) {
        delete sprite1;
        sprite1 = 0;
    }

    if (sprite2 != 0) {
        delete sprite2;
        sprite2 = 0;
    }

    if (field_C8 != 0) {
        if (field_C8->head != 0) {
            field_C8->current = field_C8->head;
            while (field_C8->head != 0) {
                void* item = field_C8->RemoveCurrent();
                if (item != 0) {
                    delete (SC_Question*)item;
                }
            }
        }
        operator delete(field_C8);
        field_C8 = 0;
    }

    if (field_C4 != 0) {
        delete field_C4;
        field_C4 = 0;
    }

    if (field_B4 != 0) {
        delete field_B4;
        field_B4 = 0;
    }
}

/* Function start: 0x4173D0 */
int Handler31::ShutDown(SC_Message* msg) {
    SC_Question* question;
    void* item;

    if (sprite1 != 0) {
        sprite1->StopAnimationSound();
    }
    if (sprite2 != 0) {
        sprite2->StopAnimationSound();
    }

    if (field_C0 != 0) {
        field_C0->Finalize();
        question = field_C0;
        if (question != 0) {
            question->~SC_Question();
            FreeMemory(question);
            field_C0 = 0;
        }
    }

    if (field_C8 != 0) {
        LinkedList* list = field_C8;
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SC_Question*)item;
                }
            }
        }
        FreeMemory(list);
        field_C8 = 0;
    }

    if (field_C4 != 0) {
        delete field_C4;
        field_C4 = 0;
    }

    if (field_B4 != 0) {
        delete field_B4;
        field_B4 = 0;
    }

    ResetSpriteStates();

    int* arr = &DAT_00473334;
    do {
        *arr = 1;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&DAT_004733e8);
    DAT_004733e8 = 1;

    IconBar::ShutDown(msg);

    return 0;
}

extern char DAT_00469b28[];

/* Function start: 0x417500 */
void Handler31::Update(int param1, int param2) {
    int i;
    int buttonIdx;
    SC_Question* question;
    InputState* pMouse;
    LinkedList* list;
    ListNode* node;
    SC_Question* nodeData;
    int yPos;
    int count;

    if (handlerId != param2) {
        return;
    }

    IconBar::Update(param1, param2);

    if (field_CC != 0) {
        i = 1;
        do {
            sprintf(DAT_0046aa00, "SPRITE%d", i);
            GameState* gs = DAT_0046aa30;
            int idx = gs->FindState(DAT_0046aa00);
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            i++;
        } while (i < 6);
        field_CC = 0;
    }

    if (field_C0 == 0 && field_C8 != 0 && (field_C8)->head == 0 && field_C4 != 0) {
        field_C0 = field_C4;
        field_C0->InitState();
        field_C4 = 0;
    }

    if (field_C0 != 0) {
        field_C0->Update(0x12, 0xA);
        question = field_C0;
        if (question->state == 2 && question != 0) {
            question->~SC_Question();
            FreeMemory(question);
            field_C0 = 0;
        }
    } else {
        list = field_C8;
        if (list != 0 && list->head != 0) {
            pMouse = DAT_0046aa08->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                buttonIdx = -1;
            } else if (pMouse == 0) {
                buttonIdx = 0;
            } else {
                buttonIdx = (pMouse->y - 10) / 0x22;
            }

            list->current = list->head;
            if (list->head != 0) {
                yPos = 0xA;
                count = 0;
                do {
                    node = list->current;
                    nodeData = 0;
                    if (node != 0) {
                        nodeData = (SC_Question*)node->data;
                    }
                    nodeData->Update(0x12, yPos);

                    if (count == buttonIdx) {
                        sprite2->Do(0x12, yPos, 1.0);
                    } else {
                        sprite1->Do(0x12, yPos, 1.0);
                    }

                    count++;
                    yPos += 0x22;

                    if (list->tail == list->current) break;
                    if (yPos > 0x11A) break;
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->head != 0);
            }
        } else {
            ResetSpriteStates();
            SendGameMessage(savedCommand, savedMsgData, 0x1F, moduleParam, 4, 0, 0, 0, 0, 0);
        }
    }

    DAT_0046aa18->DrawCursor();
}

/* Function start: 0x417750 */
int Handler31::AddMessage(SC_Message* msg) {
    int* msgData;
    SC_Question* question;
    InputState* pMouse;

    msgData = (int*)msg;
    if (IconBar::AddMessage(msg) != 0) {
        return 1;
    }

    if (field_C0 != 0) {
        msgData[0] = savedCommand;
        msgData[1] = savedCommand;
        field_C0->OnInput((SC_Message*)msgData);
        question = field_C0;
        if (question->state == 2 && question != 0) {
            question->~SC_Question();
            FreeMemory(question);
            field_C0 = 0;
            return 1;
        }
    } else {
        if (msgData[9] >= 2) {
            msgData[0] = 0x1F;
            pMouse = DAT_0046aa08->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                msgData[1] = -1;
            } else if (pMouse == 0) {
                msgData[1] = 0;
            } else {
                msgData[1] = (pMouse->y - 10) / 0x22;
            }
            msgData[4] = 2;
            return 1;
        }
        if (msgData[0xB] == 0x1B || msgData[0xB] == 0x77 || msgData[0xA] == 2) {
            SendGameMessage(savedCommand, savedMsgData, 0x1F, moduleParam, 4, 0, 0, 0, 0, 0);
        }
    }

    return 1;
}
