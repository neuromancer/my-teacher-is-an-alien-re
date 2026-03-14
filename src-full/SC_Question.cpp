#include "SC_Question.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "MMPlayer.h"
#include "Sprite.h"
#include "FlagArray.h"
#include "GameState.h"
#include "globals.h"
#include "ZBufferManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <new.h>

extern int __fastcall FUN_0044c350(void*, int, char*);
extern void __cdecl FUN_00425cb0(char*, ...);
extern int __fastcall FUN_00420a00(void*, int, int);
extern void __fastcall FUN_00420a50(void*, int, int);
extern "C" void FUN_00413e10(void*, char*, char*, ...);
extern void __fastcall FUN_00404230(void*, int, char*, int, int, int, int);
extern void __cdecl FUN_00413e70(void*, int, char*);
extern void __cdecl FUN_00444e40(void*);
extern void __fastcall FUN_00425550(void*, int, int);
extern void __fastcall FUN_0044c880(void*);
extern void __fastcall FUN_00420ac0(void*, int, int);
extern void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int);
extern void __cdecl ParseSpriteAction(void*, void*);
extern void __cdecl FUN_00412a50();
extern void __fastcall FUN_00406fd0(void*, int, int);

extern void* DAT_0046a6e0;
extern void* DAT_0046a6e8;
extern "C" int DAT_0046a6ec;
extern "C" void* DAT_0046aa30;
extern void* DAT_0046aa24;
extern char* DAT_0046aa00;
extern char DAT_00468108[];
extern char DAT_00468150[];
extern char DAT_00468168[];
extern char DAT_00468464[];
extern char DAT_004690e4[];
extern void* DAT_0046aa38;

/* Function start: 0x414780 */
SC_Question::SC_Question(int id, int dialog)
{
    int gsIndex;
    char questFile[32];

    memset(&messageQueue, 0, 0x2A * sizeof(int));

    questionId = id;
    state = 0;
    dialogPtr = (void*)dialog;

    if (FUN_0044c350(DAT_0046a6e0, id, label) == 0) {
        FUN_00425cb0("SC_Question::SC_Question missing label %d", id);
        sprintf(label, "Missing Label %d", questionId);
    }

    gsIndex = ((GameState*)DAT_0046aa30)->FindLabel(DAT_00468108);
    if (gsIndex < 0 || *(int*)((char*)DAT_0046aa30 + 0x98) - 1 < gsIndex) {
        ShowError("Invalid gamestate %d", gsIndex);
    }

    sprintf(questFile, "mis\\quest%2.2d.mis",
        *(int*)(*(int*)((char*)DAT_0046aa30 + 0x90) + gsIndex * 4));

    FUN_00413e10(this, questFile, "[QUESTION%d]", questionId);

    if (FUN_00420a00(DAT_0046a6e8, questionId, 2) != 0) {
        state = 2;
    } else if (FUN_00420a00(DAT_0046a6e8, questionId, 1) != 0) {
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
    void* gs;

    gs = DAT_0046aa30;
    if (*(int*)((char*)gs + 0x98) - 1 < 4) {
        ShowError("Invalid gamestate %d", 4);
    }

    if (*(int*)(*(int*)((char*)gs + 0x90) + 0x10) == 1) {
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

    key = *(int*)((char*)msg + 0x2c);
    if (key == 0x1b || key == 0x20 || key == 0x77 ||
        *(int*)((char*)msg + 0x28) == 2) {
        goto do_finalize;
    }
    edx = field_94 & 8;
    if (edx != 0 && key != 0) {
        goto do_finalize;
    }
    if (edx != 0 && *(int*)((char*)msg + 0x24) == 2) {
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
    void* gs;
    int gsIndex;

    state = 1;

    for (i = 0; i < 3; i++) {
        val = actionIndex[i];
        if (val != 0) {
            gs = DAT_0046aa30;
            if (val < 0 || *(int*)((char*)gs + 0x98) - 1 < val) {
                ShowError("Invalid gamestate %d", val);
            }
            *(int*)(*(int*)((char*)gs + 0x90) + val * 4) = 1;
        }
    }

    gs = DAT_0046aa30;
    gsIndex = ((GameState*)gs)->FindLabel(DAT_00468168);
    if (gsIndex < 0 || *(int*)((char*)gs + 0x98) - 1 < gsIndex) {
        ShowError("Invalid gamestate %d", gsIndex);
    }
    *(int*)(*(int*)((char*)gs + 0x90) + gsIndex * 4) += 3;
}

/* Function start: 0x414C60 */
void SC_Question::Finalize()
{
    Queue* queue;
    QueueNode* current;
    void* msgData;
    int queueType;

    if (FUN_00420a00(DAT_0046a6e8, questionId, 4) == 0) {
        FUN_00420a50(DAT_0046a6e8, questionId, 2);
    }

    state = 2;
    *(int*)((char*)dialogPtr + 0xcc) = 1;

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

        FUN_00444e40(msgData);
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
        FUN_00413e70(mouseControl, (int)this, (char*)0);
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
        *(int*)((char*)sprite + 0x94) |= 0x400;
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
        *(int*)((char*)sprite + 0x94) |= 0x400;
        *(int*)((char*)sprite + 0xac) = id;
        *(int*)((char*)sprite + 0xb0) = val;
        *(int*)((char*)sprite + 0x94) |= 0x40;
        *(int*)((char*)sprite + 0x9c) = 0x14;
        if ((field_94 & 8) != 0) {
            ((Sprite*)sprite)->ConfigStates(2);
            FUN_0044c880(sprite);
            framePtr = (int*)(*(int*)((char*)sprite + 0x90) + *(int*)((char*)sprite + 0x98) * 0x10);
            ((Sprite*)sprite)->ConfigRange(0, 1, (framePtr[1] - framePtr[0]) + 1, 1);
            framePtr = (int*)(*(int*)((char*)sprite + 0x90) + *(int*)((char*)sprite + 0x98) * 0x10);
            frameCount = (framePtr[1] - framePtr[0]) + 1;
            ((Sprite*)sprite)->ConfigRange(1, frameCount, frameCount, 1);
            ((Sprite*)sprite)->StopAnimationSound();
        }
        ((MMPlayer*)mouseControl)->AddSprite((Sprite*)sprite);
    }
    else if (strcmp(keyword, "TEXT") == 0) {
        sscanf(param_1, " %s %d", keyword, &id);
        result = FUN_0044c350(DAT_0046a6e0, id, (char*)((int)this + 0x9c));
        if (result == 0) {
            FUN_00412a50();
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                actionIndex[result] = ((GameState*)DAT_0046aa30)->FindLabel(DAT_0046aa00);
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
            gsIdx1 = ((GameState*)DAT_0046aa38)->FindLabel(buf2);
            gsIdx2 = ((GameState*)DAT_0046aa30)->FindLabel(buf1);
            action = FUN_00444a40(mem, 2, gsIdx2, 0, 0, gsIdx1, id, 0, 0, 0, 0);
        }
        if ((((int*)action)[4] == 0x11 || ((int*)action)[4] == 0x12) && result < 4) {
            ((int*)action)[5] = 1;
        }
        if (((int*)action)[4] == 0x11 && result < 4) {
            FUN_00412a50();
        }
        queue = (int*)messageQueue;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
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
                FUN_00406fd0(queue, 0, (int)action);
            } else {
                do {
                    cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        FUN_00406fd0(queue, 0, (int)action);
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
            FUN_00406fd0(queue, 0, (int)action);
        }
    }
    else if (strcmp(keyword, "CONSTANT") == 0) {
        FUN_00420a50(DAT_0046a6e8, questionId, 4);
    }
    else if (strcmp(keyword, "SINGLE_PLAY") == 0) {
        FUN_00420ac0(DAT_0046a6e8, questionId, 4);
    }
    else if (strcmp(keyword, "HOLD") == 0) {
        field_94 |= 8;
    }
    else if (strcmp(keyword, "END") == 0) {
        return 1;
    }
    else {
        FUN_00412a50();
    }

    return 0;
}
