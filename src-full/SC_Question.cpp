#include "SC_Question.h"
#include "SpriteAction.h"
#include "Queue.h"
#include "Memory.h"
#include "MMPlayer.h"
#include "Sprite.h"
#include "FlagArray.h"
#include "GameState.h"
#include "globals.h"
#include "ZBufferManager.h"
#include <stdio.h>
#include "string.h"
#include <string.h>
#include <stdlib.h>
#include <new.h>

#include "StringTable.h"
// FUN_00425cb0 = ShowMessage in string.h
// FUN_00413e10 = ParseFile in Parser.h
extern void __fastcall FUN_00404230(void*, int, char*, int, int, int, int); // ZBufferManager::ShowText
extern void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int); // SpriteAction ctor (__cdecl bridge)

extern StringTable* DAT_0046a6e0;
extern void* DAT_0046a6e8;           // MMPlayer*
extern "C" int DAT_0046a6ec;         // GameEngine instance
extern "C" GameState* DAT_0046aa30;
extern ZBufferManager* DAT_0046aa24;
extern char* DAT_0046aa00;           // temp string buffer
extern char DAT_00468108[];          // GameState key (quest level)
extern char DAT_004690e4[];          // format string for quest
extern GameState* DAT_0046aa38;      // secondary GameState (string table)

/* Function start: 0x414780 */
SC_Question::SC_Question(int id, int dialog)
{
    int gsIndex;
    char questFile[32];

    memset(&messageQueue, 0, 0x2A * sizeof(int));

    questionId = id;
    state = 0;
    dialogPtr = (void*)dialog;

    if (DAT_0046a6e0->GetString( id, label) == 0) {
        ShowMessage("SC_Question::SC_Question missing label %d", id);
        sprintf(label, "Missing Label %d", questionId);
    }

    gsIndex = (DAT_0046aa30)->FindState(DAT_00468108);
    if (gsIndex < 0 || DAT_0046aa30->maxStates - 1 < gsIndex) {
        ShowError("Invalid gamestate %d", gsIndex);
    }

    sprintf(questFile, "mis\\quest%2.2d.mis",
        DAT_0046aa30->stateValues[gsIndex]);

    ParseFile(this, questFile, "[QUESTION%d]", questionId);

    if (((FlagArray*)DAT_0046a6e8)->GetFlag( questionId, 2) != 0) {
        state = 2;
    } else if (((FlagArray*)DAT_0046a6e8)->GetFlag( questionId, 1) != 0) {
        state = 2;
    }
}

/* Function start: 0x4148F0 */
SC_Question::~SC_Question()
{
    void* mc;
    Queue* queue;
    QueueNode* current;
    void* msgData;

    mc = mouseControl;
    if (mc != 0) {
        ((MMPlayer*)mc)->~MMPlayer();
        free(mc);
        mouseControl = 0;
    }

    queue = messageQueue;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                current = (QueueNode*)queue->current;
                if (current != 0) {
                    if (queue->head == current) {
                        queue->head = current->next;
                    }
                    if (queue->tail == current) {
                        queue->tail = current->prev;
                    }
                    if (current->next != 0) {
                        current->next->prev = current->prev;
                    }
                    if (current->prev != 0) {
                        current->prev->next = current->next;
                    }
                    msgData = 0;
                    if (current != 0) {
                        msgData = current->data;
                    }
                    if (current != 0) {
                        current->data = 0;
                        current->prev = 0;
                        current->next = 0;
                        free(current);
                        queue->current = 0;
                    }
                    queue->current = queue->head;
                } else {
                    msgData = 0;
                }
                if (msgData != 0) {
                    delete (SpriteAction*)msgData;
                }
            }
        }
        free(queue);
        messageQueue = 0;
    }
    dialogPtr = 0;
}

/* Function start: 0x414A50 */
void SC_Question::Update(int x, int y)
{
    GameState* gs;

    gs = DAT_0046aa30;
    if (gs->maxStates - 1 < 4) {
        ShowError("Invalid gamestate %d", 4);
    }

    if (gs->stateValues[4] == 1) {
        sprintf(DAT_0046aa00, DAT_004690e4, questionId);
        FUN_00404230(DAT_0046aa24, 0, DAT_0046aa00, 0x22b, y + 0x17, 10000, -1);
    }

    switch (state) {
    case 0:
        FUN_00404230(DAT_0046aa24, 0, label, x + 10, y + 0x17, 10000, -1);
        return;
    case 1:
        if (mouseControl != 0) {
            if ((field_94 & 8) == 0) {
                if (((MMPlayer*)mouseControl)->Draw() == 0) {
                    Finalize();
                    return;
                }
            } else {
                if (((MMPlayer*)mouseControl)->Draw() == 0) {
                    ((MMPlayer*)mouseControl)->ResetAnimations(1);
                    return;
                }
            }
        } else {
            Finalize();
        }
        break;
    case 2:
        break;
    default:
        ShowError("illegal state");
        return;
    }
}

/* Function start: 0x414B70 */
int SC_Question::OnInput(SC_Message* msg)
{
    int edx;
    int key;

    SpriteAction* action = (SpriteAction*)msg;
    key = action->lastKey;
    if (key == 0x1b || key == 0x20 || key == 0x77 ||
        action->button2 == 2) {
        goto do_finalize;
    }
    edx = field_94 & 8;
    if (edx != 0 && key != 0) {
        goto do_finalize;
    }
    if (edx != 0 && action->button1 == 2) {
do_finalize:
        Finalize();
    }

    return 1;
}

/* Function start: 0x414BC0 */
void SC_Question::InitState()
{
    int i;
    int val;
    GameState* gs;
    int gsIndex;

    state = 1;

    for (i = 0; i < 3; i++) {
        val = actionIndex[i];
        if (val != 0) {
            gs = DAT_0046aa30;
            if (val < 0 || gs->maxStates - 1 < val) {
                ShowError("Invalid gamestate %d", val);
            }
            gs->stateValues[val] = 1;
        }
    }

    gs = DAT_0046aa30;
    gsIndex = gs->FindState("NUM_ACTIONS");
    if (gsIndex < 0 || gs->maxStates - 1 < gsIndex) {
        ShowError("Invalid gamestate %d", gsIndex);
    }
    gs->stateValues[gsIndex] += 3;
}

/* Function start: 0x414C60 */
void SC_Question::Finalize()
{
    Queue* queue;
    QueueNode* current;
    void* msgData;
    int queueType;

    if (((FlagArray*)DAT_0046a6e8)->GetFlag( questionId, 4) == 0) {
        ((FlagArray*)DAT_0046a6e8)->SetFlag( questionId, 2);
    }

    state = 2;
    ((int*)dialogPtr)[0x33] = 1;

    queue = messageQueue;
    if (queue == 0) return;
    if (queue->head == 0) return;

    do {
        queue = messageQueue;
        msgData = 0;
        queueType = queue->type;

        if (queueType == 1 || queueType == 4) {
            queue->current = queue->head;
        }
        else if (queueType == 2 || queueType == 0) {
            queue->current = queue->tail;
        }
        else {
            ShowError("bad queue type %lu", queueType);
        }

        current = (QueueNode*)queue->current;
        if (current != 0) {
            if (queue->head == current) {
                queue->head = current->next;
            }
            if (queue->tail == current) {
                queue->tail = current->prev;
            }
            if (current->next != 0) {
                current->next->prev = current->prev;
            }
            if (current->prev != 0) {
                current->prev->next = current->next;
            }

            if (current != 0) {
                msgData = current->data;
            } else {
                msgData = 0;
            }

            if (current != 0) {
                current->data = 0;
                current->prev = 0;
                current->next = 0;
                free(current);
                queue->current = 0;
            }
            queue->current = queue->head;
        }

        EnqueueSpriteAction(msgData);
        if (msgData != 0) {
            delete (SpriteAction*)msgData;
        }
    } while (*(int*)messageQueue != 0);
}

/* Function start: 0x414DA0 */
int SC_Question::LBLParse(char* param_1)
{
    char buf2[64];
    char buf1[64];
    int val;
    char keyword[32];
    unsigned int id;
    void* mem;
    int result;
    void* action;
    void* sprite;
    int* queue;
    int cur;
    int* framePtr;
    int frameCount;

    sscanf(param_1, "%s", keyword);

    if (strcmp(keyword, "OVERLAYS") == 0) {
        if (mouseControl == 0) {
            mem = malloc(0xa0);
            void* mc = 0;
            if (mem != 0) {
                mc = new (mem) MMPlayer();
            }
            mouseControl = mc;
        }
        Parser::ProcessFile((Parser*)mouseControl, this, (char*)0);
    }
    else if (strncmp(keyword, "OVE", 3) == 0) {
        if (mouseControl == 0) {
            mem = malloc(0xa0);
            void* mc = 0;
            if (mem != 0) {
                mc = new (mem) MMPlayer();
            }
            mouseControl = mc;
        }
        mem = malloc(0xf8);
        sprite = 0;
        if (mem != 0) {
            sprite = new (mem) Sprite((char*)0);
        }
        ((Sprite*)sprite)->flags |= 0x400;
        ((Parser*)sprite)->LBLParse(param_1);
        ((MMPlayer*)mouseControl)->AddSprite((Sprite*)sprite);
    }
    else if (strcmp(keyword, "STANDARD_SPR") == 0) {
        sscanf(param_1, " %s %d %d", keyword, &id, &val);
        if (mouseControl == 0) {
            mem = malloc(0xa0);
            void* mc = 0;
            if (mem != 0) {
                mc = new (mem) MMPlayer();
            }
            mouseControl = mc;
        }
        sprintf(buf1, "questions\\idi%4.4d.smk", questionId);
        mem = malloc(0xf8);
        sprite = 0;
        if (mem != 0) {
            sprite = new (mem) Sprite((char*)buf1);
        }
        ((Sprite*)sprite)->flags |= 0x400;
        ((Sprite*)sprite)->num_states = id;
        ((Sprite*)sprite)->field_0xb0 = val;
        ((Sprite*)sprite)->flags |= 0x40;
        ((Sprite*)sprite)->priority = 0x14;
        if ((field_94 & 8) != 0) {
            ((Sprite*)sprite)->ConfigStates(2);
            ((Sprite*)sprite)->InitAnimation();
            framePtr = (int*)((int)((Sprite*)sprite)->ranges + ((Sprite*)sprite)->handle * 0x10);
            ((Sprite*)sprite)->ConfigRange(0, 1, (framePtr[1] - framePtr[0]) + 1, 1);
            framePtr = (int*)((int)((Sprite*)sprite)->ranges + ((Sprite*)sprite)->handle * 0x10);
            frameCount = (framePtr[1] - framePtr[0]) + 1;
            ((Sprite*)sprite)->ConfigRange(1, frameCount, frameCount, 1);
            ((Sprite*)sprite)->StopAnimationSound();
        }
        ((MMPlayer*)mouseControl)->AddSprite((Sprite*)sprite);
    }
    else if (strcmp(keyword, "TEXT") == 0) {
        sscanf(param_1, " %s %d", keyword, &id);
        result = DAT_0046a6e0->GetString( id, label);
        if (result == 0) {
            Parser::LBLParse("SC_Question");
        }
    }
    else if (strcmp(keyword, "ACTIVATEQ") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0x1f, id, 0, 0, 0x17, 0, 0, 0, 0, 0);
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "ENABLEACTIVATEQ") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0x1f, id, 0, 0, 0x13, 0, 0, 0, 0, 0);
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "DEACTIVATEQ") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0x1f, id, 0, 0, 0x18, 0, 0, 0, 0, 0);
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "ENABLEQ") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0x1f, id, 0, 0, 0x0f, 0, 0, 0, 0, 0);
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "DISABLEQ") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0x1f, id, 0, 0, 0x10, 0, 0, 0, 0, 0);
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "DISABLESPRITE") == 0) {
        result = sscanf(param_1, "%s %d", keyword, &id);
        if (result != 2) {
            ShowError("Error in SCIdilog.cpp: %s in parse file is incorrect");
        }
        result = 0;
        do {
            if ((result == 2) && (questionId != 0)) {
                ShowError("Error in SCIdilog.cpp: Cannot disable more than three sprites per question");
            }
            if (actionIndex[result] == 0) {
                sprintf(DAT_0046aa00, "SPRITE%d", id);
                actionIndex[result] = (DAT_0046aa30)->FindState(DAT_0046aa00);
                break;
            }
            result = result + 1;
        } while (result < 3);
    }
    else if (strcmp(keyword, "GAMESTATE") == 0) {
        result = sscanf(param_1, "%s %s %s %d", keyword, buf1, buf2, &id);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            int gsIdx1;
            int gsIdx2;
            gsIdx1 = DAT_0046aa38->FindState(buf2);
            gsIdx2 = (DAT_0046aa30)->FindState(buf1);
            action = FUN_00444a40(mem, 2, gsIdx2, 0, 0, gsIdx1, id, 0, 0, 0, 0);
        }
        if ((((SpriteAction*)action)->instruction == 0x11 || ((SpriteAction*)action)->instruction == 0x12) && result < 4) {
            ((SpriteAction*)action)->extra1 = 1;
        }
        if (((SpriteAction*)action)->instruction == 0x11 && result < 4) {
            Parser::LBLParse("SC_Question");
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "GIVELATE") == 0) {
        sscanf(param_1, "%s %s %s %d", keyword, buf1, buf2, &id);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 0x2c, 0, 0, 0, 0x3c, 0, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "GIVECUT") == 0) {
        sscanf(param_1, "%s %s %s %d", keyword, buf1, buf2, &id);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 0x2c, 0, 0, 0, 0x3d, 0, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "GIVELATE") == 0) {
        sscanf(param_1, "%s %s %s %d", keyword, buf1, buf2, &id);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 0x2c, 0, 0, 0, 0x3e, 0, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "SWITCHROOM") == 0) {
        result = sscanf(param_1, " %s %d %d", keyword, &id, &val);
        if (result != 3) {
            ShowError("Error in ThotsLvl.cpp: %s in parse file is incomplete");
        }
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 1, id, 0x1f, 0, 4, 0, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 0x20, val, 0x1f, 0, 4, 0, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "MESSAGE") == 0) {
        mem = malloc(0x38);
        action = 0;
        if (mem != 0) {
            action = FUN_00444a40(mem, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
        ParseSpriteAction(action, this);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "PLAYSOUND") == 0) {
        sscanf(param_1, " %s %d", keyword, &id);
        if (messageQueue == 0) {
            mem = malloc(0x10);
            queue = 0;
            if (mem != 0) {
                ((int*)mem)[3] = 0;
                ((int*)mem)[0] = 0;
                ((int*)mem)[1] = 0;
                ((int*)mem)[2] = ((int*)mem)[0];
                queue = (int*)mem;
            }
            messageQueue = (Queue*)queue;
        }
        mem = malloc(0x38);
        if (mem == 0) {
            action = 0;
        } else {
            action = FUN_00444a40(mem, 4, id, 0, 0, 2, id, 0, 0, 0, 0);
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (action == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            action = 0;
                        } else {
                            ((int*)mem)[2] = (int)action;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            action = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)action;
                            queue[1] = (int)action;
                            queue[2] = (int)action;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)action)[1] = 0;
                            ((int*)action)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)action;
                            queue[1] = (int)action;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(action);
        }
    }
    else if (strcmp(keyword, "CONSTANT") == 0) {
        ((FlagArray*)DAT_0046a6e8)->SetFlag( questionId, 4);
    }
    else if (strcmp(keyword, "SINGLE_PLAY") == 0) {
        ((FlagArray*)DAT_0046a6e8)->ClearFlag(questionId, 4);
    }
    else if (strcmp(keyword, "HOLD") == 0) {
        field_94 |= 8;
    }
    else if (strcmp(keyword, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SC_Question");
    }

    return 0;
}
