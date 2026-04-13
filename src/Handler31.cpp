#include "Handler31.h"
#include <string.h>
#include <stdio.h>
#include "Sprite.h"
#include "Palette.h"
#include "SpriteAction.h"
#include "SC_Question.h"
#include "LinkedList.h"
#include "globals.h"
#include "Queue.h"
#include "ZBufferManager.h"
#include "Memory.h"
#include "StringTable.h"
#include "AnimatedAsset.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void ShowMessage(char *param_1, ...);
extern "C" void WriteToMessageLog(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

#include "InputManager.h"
#include "MouseControl.h"
#include "GameState.h"
#include "FlagArray.h"


extern void ResetSpriteStates();

/* Function start: 0x418086 */
int Handler31::LBLParse(char* line) { // prologue at 0x418060
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
        if (palette != 0) {
            delete palette;
            palette = 0;
        }
        palette = new Palette();
        palette->Load(arg2);
    } else if (strcmp(token, "QUESTION") == 0) {
        sscanf(line, " %s %d ", token, &id);
        question = new SC_Question(id, (SCI_Dialog*)this);

        if (question->state != 2) {
            list = questionQueue;
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
                list = questionQueue;
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
            delete question;
        }
    } else if (strcmp(token, "PLACEHOLDER") == 0) {
        sscanfResult = sscanf(line, " %s %d %s", token, &id, arg2);

        if ((questionQueue)->head == 0) {
            placeholder = new SC_Question(id, (SCI_Dialog*)this);
        }

        g_FlagManager_0046a6e8->SetFlag(id, 4);

        if (sscanfResult == 3) {
            if (strcmp(arg2, "CONSTANT") != 0) {
                g_FlagManager_0046a6e8->SetFlag(id, 4);
            }
            if (strcmp(arg2, "SINGLE_PLAY") != 0) {
                g_FlagManager_0046a6e8->ClearFlag(id, 4);
            }
        }

        question = placeholder;
        if (question != 0 && question->state == 2 && question != 0) {
            delete question;
            placeholder = 0;
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

    list = questionQueue;
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
                        delete question;
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
        delete question;
    }
    return 1;
}

/* Function start: 0x416E70 */
Handler31::Handler31() {
    memset(&questionCount, 0, 40);
    handlerId = 0x1F;

    optionSprite = new Sprite("\"elements\\option2.smk\"");
    optionSprite->priority = 0x3E9;
    optionSprite->flags |= 0x40;

    optionHiSprite = new Sprite("\"elements\\option2h.smk\"");
    optionHiSprite->priority = 0x3E9;
    optionHiSprite->flags |= 0x40;
}

/* Function start: 0x416FC0 */
Handler31::~Handler31() {
    Sprite* spr;
    SC_Question* question;
    Palette* pal;

    spr = optionSprite;
    if (spr != 0) {
        delete spr;
        optionSprite = 0;
    }

    spr = optionHiSprite;
    if (spr != 0) {
        delete spr;
        optionHiSprite = 0;
    }

    if (questionQueue != 0) {
        Queue* list = questionQueue;
        if (list->head != 0) {
            list->current = list->head;
            if (list->head != 0) {
                do {
                    question = (SC_Question*)list->Pop();
                    if (question != 0) {
                        delete question;
                    }
                } while (list->head != 0);
            }
        }
        delete list;
        questionQueue = 0;
    }

    question = placeholder;
    if (question != 0) {
        delete question;
        placeholder = 0;
    }

    pal = palette;
    if (pal != 0) {
        delete pal;
        palette = 0;
    }
}

/* Function start: 0x4173D0 */
int Handler31::ShutDown(SC_MessageParser* msg) {
    SC_Question* question;

    if (optionSprite != 0) {
        optionSprite->StopAnimationSound();
    }
    if (optionHiSprite != 0) {
        optionHiSprite->StopAnimationSound();
    }

    if (activeQuestion != 0) {
        activeQuestion->Finalize();
        question = activeQuestion;
        if (question != 0) {
            delete question;
            activeQuestion = 0;
        }
    }

    if (questionQueue != 0) {
        Queue* list = questionQueue;
        if (list->head != 0) {
            list->current = list->head;
            if (list->head != 0) {
                do {
                    question = (SC_Question*)list->Pop();
                    if (question != 0) {
                        delete question;
                    }
                } while (list->head != 0);
            }
        }
        delete list;
        questionQueue = 0;
    }

    question = placeholder;
    if (question != 0) {
        delete question;
        placeholder = 0;
    }

    {
        Palette* pal = palette;
        if (pal != 0) {
            delete pal;
            palette = 0;
        }
    }

    ResetSpriteStates();

    int* arr = &g_IconBarState_00473334;
    do {
        *arr = 1;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&g_InventoryState_004733e8);
    g_InventoryState_004733e8 = 1;

    IconBar::ShutDown(msg);

    return 0;
}

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

    yPos = 0xA;
    count = 0;

    if (handlerId != param2) {
        return;
    }

    IconBar::Update(param1, param2);

    if (resetSprites != 0) {
        i = 1;
        do {
            sprintf(g_Buffer_0046aa00, "SPRITE%d", i);
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState(g_Buffer_0046aa00);
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            i++;
        } while (i < 6);
        resetSprites = 0;
    }

    if (activeQuestion == 0 && questionQueue != 0 && (questionQueue)->head == 0 && placeholder != 0) {
        activeQuestion = placeholder;
        activeQuestion->InitState();
        placeholder = 0;
    }

    if (activeQuestion != 0) {
        activeQuestion->Update(0x12, 0xA);
        question = activeQuestion;
        if (question->state == 2 && question != 0) {
            delete question;
            activeQuestion = 0;
        }
    } else {
        list = questionQueue;
        if (list != 0 && list->head != 0) {
            pMouse = g_InputManager_0046aa08->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                buttonIdx = -1;
            } else if (pMouse != 0) {
                buttonIdx = (pMouse->y - 10) / 0x22;
            } else {
                buttonIdx = 0;
            }

            list->current = list->head;
            if (list->head != 0) {
                do {
                    node = list->current;
                    nodeData = 0;
                    if (node != 0) {
                        nodeData = (SC_Question*)node->data;
                    }
                    nodeData->Update(0x12, yPos);

                    if (count == buttonIdx) {
                        optionHiSprite->Do(0x12, yPos, 1.0);
                    } else {
                        optionSprite->Do(0x12, yPos, 1.0);
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

    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x417750 */
int Handler31::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action;
    SC_Question* question;
    InputState* pMouse;

    action = (SpriteAction*)msg;
    if (IconBar::AddMessage(msg) != 0) {
        return 1;
    }

    if (activeQuestion != 0) {
        action->addressType = savedCommand;
        action->addressValue = savedCommand;
        activeQuestion->OnInput((SC_MessageParser*)action);
        question = activeQuestion;
        if (question->state == 2 && question != 0) {
            delete question;
            activeQuestion = 0;
            return 1;
        }
    } else {
        if (action->button1 >= 2) {
            action->addressType = 0x1F;
            pMouse = g_InputManager_0046aa08->pMouse;
            if (pMouse == 0 || pMouse->y < 10) {
                action->addressValue = -1;
            } else if (pMouse == 0) {
                action->addressValue = 0;
            } else {
                action->addressValue = (pMouse->y - 10) / 0x22;
            }
            action->instruction = 2;
            return 1;
        }
        if (action->lastKey == 0x1B || action->lastKey == 0x77 || action->button2 == 2) {
            SendGameMessage(savedCommand, savedMsgData, 0x1F, moduleParam, 4, 0, 0, 0, 0, 0);
        }
    }

    return 1;
}

/* Function start: 0x417880 */
int Handler31::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    SC_Question* question;

    if (handlerId != action->addressType) {
        return 0;
    }

    if (questionQueue == 0) {
        questionQueue = new Queue(4);
    }

    switch (action->instruction) {
    case 0:
        break;

    case 2:
        if (activeQuestion == 0) {
            activeQuestion = SelectQuestion(action->addressValue);
        }
        if (activeQuestion != 0) {
            activeQuestion->InitState();
        }
        break;

    case 0xF:
        g_FlagManager_0046a6e8->SetFlag(action->addressValue, 1);
        break;

    case 0x10:
        question = RemoveQuestion(action->addressValue);
        g_FlagManager_0046a6e8->ClearFlag(action->addressValue, 1);
        if (question != 0) {
            delete question;
        }
        break;

    case 0x13:
        g_FlagManager_0046a6e8->SetFlag(action->addressValue, 1);
        /* fall through */

    case 0x17:
        if (CheckDuplicateQuestion(action->addressValue) != 0) {
            question = new SC_Question(action->addressValue, (SCI_Dialog*)this);
            if (question->state == 2) {
                if (question != 0) {
                    delete question;
                }
            } else {
                Queue* list = questionQueue;
                if (question == 0) {
                    ShowError("queue fault 0101");
                }
                list->current = list->head;
                if (list->type == 1 || list->type == 2) {
                    if (list->head == 0) {
                        list->InsertAtCurrent(question);
                    } else {
                        while (1) {
                            ListNode* cur = list->current;
                            if (((SC_Question*)cur->data)->questionId < question->questionId) {
                                list->InsertAtCurrent(question);
                                break;
                            }
                            if (list->tail == cur) {
                                list->PushNode(question);
                                break;
                            }
                            if (cur != 0) {
                                list->current = cur->next;
                            }
                            if (list->current == 0) break;
                        }
                    }
                } else {
                    list->InsertAtCurrent(question);
                }
            }
        }
        break;

    case 0x18:
        question = RemoveQuestion(action->addressValue);
        g_FlagManager_0046a6e8->ClearFlag(action->addressValue, 2);
        if (question != 0) {
            delete question;
        }
        break;

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x417C80 */
SC_Question* Handler31::SelectQuestion(int idx) {
    int count;
    LinkedList* list;
    ListNode* node;
    void* data;

    count = 0;
    list = questionQueue;
    if (list == 0) {
        return 0;
    }
    list->current = list->head;
    if (*(int*)questionQueue == 0) {
        return 0;
    }

    do {
        list = questionQueue;
        node = list->current;
        if (count == idx) {
            if (node == 0) {
                return 0;
            }
            if (list->head == node) {
                list->head = node->next;
            }
            if (list->tail == list->current) {
                list->tail = list->current->prev;
            }
            node = list->current;
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            node = list->current;
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            node = list->current;
            data = 0;
            if (node != 0) {
                data = node->data;
            }
            if (node != 0) {
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                FreeMemory(node);
                list->current = 0;
            }
            list->current = list->head;
            return (SC_Question*)data;
        }
        if (list->tail == node) {
            return 0;
        }
        if (node != 0) {
            list->current = node->next;
        }
        count++;
    } while (*(int*)questionQueue != 0);

    return 0;
}

/* Function start: 0x417E60 */
SC_Question* Handler31::RemoveQuestion(int id) {
    SC_Question* temp;
    LinkedList* list;
    ListNode* node;
    void* nodeData;
    void* data;

    temp = new SC_Question(id, (SCI_Dialog*)this);

    list = questionQueue;
    if (list == 0) {
        goto not_found;
    }

    if (temp == 0) {
        ShowError("queue fault 0103");
    }

    list->current = list->head;
    if (list->head == 0) {
        goto not_found;
    }

    do {
        node = list->current;
        nodeData = 0;
        if (node != 0) {
            nodeData = node->data;
        }

        if (((SC_Question*)nodeData)->questionId == temp->questionId) {
            if (temp != 0) {
                delete temp;
            }

            list = questionQueue;
            node = list->current;
            if (node == 0) {
                return 0;
            }
            if (list->head == node) {
                list->head = node->next;
            }
            if (list->tail == list->current) {
                list->tail = list->current->prev;
            }
            node = list->current;
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            node = list->current;
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            node = list->current;
            data = 0;
            if (node != 0) {
                data = node->data;
            }
            if (node != 0) {
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                FreeMemory(node);
                list->current = 0;
            }
            list->current = list->head;
            return (SC_Question*)data;
        }

        if (list->tail == node) break;
        if (node != 0) {
            list->current = node->next;
        }
    } while (list->current != 0);

not_found:
    if (temp != 0) {
        delete temp;
    }
    return 0;
}

/* Function start: 0x417120 */
void Handler31::Init(SC_MessageParser* msg) {
    SC_Question* question;
    char dialogFile[32];
    int periodIdx;
    int periodVal;
    int roomInstIdx;
    GameState* gs;

    IconBar::Init(msg);

    int* arr = &g_IconBarState_00473334;
    do {
        *arr = 0;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&g_InventoryState_004733e8);
    g_InventoryState_004733e8 = 0;

    gs = g_GameState_0046aa30;
    periodIdx = gs->FindState(g_QuestLevelKey_00468108);
    if (periodIdx < 0 || gs->maxStates - 1 < periodIdx) {
        ShowError("Invalid gamestate %d", periodIdx);
    }
    sprintf(dialogFile, "mis\\dialog%2.2d.mis", gs->stateValues[periodIdx]);

    if (questionQueue != 0) {
        Queue* list = questionQueue;
        if (list->head != 0) {
            list->current = list->head;
            if (list->head != 0) {
                do {
                    question = (SC_Question*)list->Pop();
                    if (question != 0) {
                        delete question;
                    }
                } while (list->head != 0);
            }
        }
        FreeMemory(questionQueue);
        questionQueue = 0;
    }

    questionQueue = new Queue(4);

    gs = g_GameState_0046aa30;
    periodIdx = gs->FindState("PERIOD");
    if (periodIdx < 0 || gs->maxStates - 1 < periodIdx) {
        ShowError("Invalid gamestate %d", periodIdx);
    }
    periodVal = gs->stateValues[periodIdx];

    gs = g_GameState_0046aa30;
    roomInstIdx = gs->FindState("ROOMINSTANCE");
    if (roomInstIdx < 0 || gs->maxStates - 1 < roomInstIdx) {
        ShowError("Invalid gamestate %d", roomInstIdx);
    }

    gs = g_GameState_0046aa30;
    if (g_PeriodStateIdx_0046cb90 < 0 || gs->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
        ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
    }
    if (gs->stateValues[g_PeriodStateIdx_0046cb90] == -1 &&
        (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90)) {
        ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
    }

    {
        int _charIdx = g_GameState_0046aa30->stateValues[g_PeriodStateIdx_0046cb90];
        char _ch = g_PeriodCharTable_0046cb94[_charIdx];
        int _extra1 = ((SpriteAction*)msg)->extra1;
        int _roomInst = gs->stateValues[roomInstIdx];
        ParseFile(this, dialogFile, "[PERIOD%2.2d_SS%d_DIALOG%d_%c]",
                  periodVal, _roomInst, _extra1, (int)_ch);
    }

    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }

    g_TextManager_00436990->LoadFont("elements\\text1.smk");
}
