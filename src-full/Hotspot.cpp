#include "Hotspot.h"
#include "SpriteAction.h"
#include "SC_Question.h"
#include "string.h"
#include "Message.h"
#include "Memory.h"
#include "Sprite.h"
#include "MMPlayer.h"
#include "TimedEvent.h"
#include "globals.h"
#include "string.h"
#include <stdio.h>
#include <string.h>

// Helper for Miles Sound System
typedef void* HSAMPLE;
extern "C" int __stdcall AIL_sample_status(HSAMPLE);

extern void FreeMemory(void*);

#include "HotspotAction.h"
#include "GameState.h"

/* Function start: 0x445830 */
T_Hotspot::T_Hotspot(int param_1) : Parser()
{
    hotspotPos.x = 0;
    hotspotPos.y = 0;
    hotspotRight = 0;
    hotspotBottom = 0;
    memset(&parentHandlerId, 0, 0x12 * 4);
    hotspotHandle = param_1;
}

/* Function start: 0x4458D0 */
T_Hotspot::~T_Hotspot()
{
    HotspotAction* ptr;
    int i;
    HotspotAction** p = &items[0];

    for (i = 0; i < 8; i++) {
        ptr = *p;
        if (ptr != 0) {
            delete ptr;
            *p = 0;
        }
        p++;
    }
}


/* Function start: 0x4459A0 */
void T_Hotspot::DoItem(int param)
{
    T_Hotspot::SelectItem();
    HotspotAction* item = items[currentIndex];
    if (item != 0) {
        item->Update(param);
    }
}

/* Function start: 0x4459D0 */
void T_Hotspot::SelectItem()
{
    int i;
    HotspotAction** p;

    p = &items[0];
    for (i = 0; i < 8; i++) {
        if (*p != 0) {
            int status = (*p)->state;
            if (status != 0 && status == 1) {
                goto done;
            }
        }
        p++;
    }

    p = &items[0];
    for (i = 0; i < 8; i++) {
        if (*p != 0) {
            if ((*p)->CheckConditions() != 0) {
                break;
            }
        }
        p++;
    }

    T_Hotspot::currentIndex = i;
    if (i > 7) {
        T_Hotspot::currentIndex = 7;
    }

done:
    ;
}

/* Function start: 0x445970 */
void T_Hotspot::StopAll()
{
    HotspotAction** p = &items[0];
    int i = 8;
    do {
        if (*p != 0) {
            (*p)->Reset();
        }
        p++;
        i--;
    } while (i != 0);
}

/* Function start: 0x445A30 */
int T_Hotspot::LBLParse(char* param_1)
{
    int iVar5;
    HotspotAction* action;
    int* list;
    SpriteAction* msg;
    char value[32];
    char token[32];
    int param;
    int param2;

    param = 0;
    param2 = 0;
    token[0] = '\0';
    sscanf(param_1, " %s ", token);

    if (strcmp(token, "HANDLE") == 0) {
        sscanf(param_1, " %s %d", token, &hotspotHandle);
    }
    else if (strcmp(token, "HOTSPOT") == 0) {
        iVar5 = sscanf(param_1, "%s %s %d", token, value, &param);
        if (iVar5 != 3) {
            ShowError("Error in Thotspot.cpp : %s - missing parameters in parse file", param_1);
        }

        action = new HotspotAction(hotspotHandle);
        currentAction = action;
        action->parentHandlerId = parentHandlerId;
        currentAction->parentModuleParam = parentModuleParam;

        if (strstr(value, "LEFT") != 0) {
            hotspotPos.x = 0;
            hotspotPos.y = 100;
            hotspotRight = 0x4B;
            hotspotBottom = 300;
            currentAction->rolloverStateIdx = 4;
        } else if (strstr(value, "RIGHT") != 0) {
            hotspotPos.x = 0x234;
            hotspotPos.y = 100;
            hotspotRight = 0x27F;
            hotspotBottom = 300;
            currentAction->rolloverStateIdx = 3;
        }

        // Ensure action list exists
        action = currentAction;
        if (action->messagesQueue == 0) {
            action->messagesQueue = (Queue*)new LinkedList();
        }

        // Create SpriteAction
        msg = new SpriteAction(parentHandlerId, param, parentHandlerId, parentModuleParam, 4, 0, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        // Sorted insert into action list
        list = (int*)currentAction->messagesQueue;
        list[2] = list[0];
        if (list[3] == 1 || list[3] == 2) {
            if (list[0] == 0) {
                ((LinkedList*)list)->InsertNode(msg);
            } else {
                while (list[2] != 0) {
                    int nodeData = *(int*)(list[2] + 8);
                    if (*(int*)nodeData < *(int*)msg) {
                        ((LinkedList*)list)->InsertNode(msg);
                        goto done;
                    }
                    if (list[1] == list[2]) {
                        ((LinkedList*)list)->PushNode(msg);
                        goto done;
                    }
                    if (list[2] != 0) {
                        list[2] = *(int*)(list[2] + 4);
                    }
                }
            }
        } else {
            ((Queue*)list)->InsertAtCurrent((void*)msg);
        }
    }
    else if (strcmp(token, "PLAYSOUND") == 0) {
        iVar5 = sscanf(param_1, " %s %d", token, &param);
        if (iVar5 != 2) {
            ShowError("Error in ThotsLvl.cpp: %s in parse file is incomplete", param_1);
        }

        action = new HotspotAction(hotspotHandle);
        currentAction = action;
        action->parentHandlerId = parentHandlerId;
        currentAction->parentModuleParam = parentModuleParam;
        currentAction->rolloverStateIdx = 2;

        if (currentAction->messagesQueue == 0) {
            currentAction->messagesQueue = (Queue*)new LinkedList();
        }

        msg = new SpriteAction(4, param, parentHandlerId, parentModuleParam, 2, 0, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = (int*)currentAction->messagesQueue;
        list[2] = list[0];
        if (list[3] == 1 || list[3] == 2) {
            if (list[0] == 0) {
                ((LinkedList*)list)->InsertNode(msg);
            } else {
                while (list[2] != 0) {
                    if (*(int*)(*(int*)(list[2] + 8)) < *(int*)msg) {
                        ((LinkedList*)list)->InsertNode(msg);
                        goto done;
                    }
                    if (list[1] == list[2]) {
                        ((LinkedList*)list)->PushNode(msg);
                        goto done;
                    }
                    if (list[2] != 0) {
                        list[2] = *(int*)(list[2] + 4);
                    }
                }
            }
        } else {
            ((Queue*)list)->InsertAtCurrent((void*)msg);
        }
    }
    else if (strcmp(token, "SWITCHROOM") == 0) {
        iVar5 = sscanf(param_1, " %s %d %d", token, &param, &param2);
        if (iVar5 != 3) {
            ShowError("Error in ThotsLvl.cpp: %s in parse file is incomplete", param_1);
        }

        action = new HotspotAction(hotspotHandle);
        currentAction = action;
        action->parentHandlerId = parentHandlerId;
        currentAction->parentModuleParam = parentModuleParam;
        currentAction->rolloverStateIdx = 2;

        if (currentAction->messagesQueue == 0) {
            currentAction->messagesQueue = (Queue*)new LinkedList();
        }

        msg = new SpriteAction(0x20, param2, parentHandlerId, parentModuleParam, 4, param, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = (int*)currentAction->messagesQueue;
        list[2] = list[0];
        if (list[3] == 1 || list[3] == 2) {
            if (list[0] == 0) {
                ((LinkedList*)list)->InsertNode(msg);
            } else {
                while (list[2] != 0) {
                    if (*(int*)(*(int*)(list[2] + 8)) < *(int*)msg) {
                        ((LinkedList*)list)->InsertNode(msg);
                        goto done;
                    }
                    if (list[1] == list[2]) {
                        ((LinkedList*)list)->PushNode(msg);
                        goto done;
                    }
                    if (list[2] != 0) {
                        list[2] = *(int*)(list[2] + 4);
                    }
                }
            }
        } else {
            ((Queue*)list)->InsertAtCurrent((void*)msg);
        }
    }
    else if (strcmp(token, "DIALOG") == 0) {
        sscanf(param_1, " %s %d", token, &param);

        action = new HotspotAction(hotspotHandle);
        currentAction = action;
        action->parentHandlerId = parentHandlerId;
        currentAction->parentModuleParam = parentModuleParam;
        currentAction->rolloverStateIdx = 6;

        if (currentAction->messagesQueue == 0) {
            currentAction->messagesQueue = (Queue*)new LinkedList();
        }

        // Look up ROOMINSTANCE gamestate
        GameState* gs = (GameState*)g_GameState_0046aa30;
        int gsIdx = gs->FindState("ROOMINSTANCE");
        if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
            ShowError("Invalid gamestate %d", gsIdx);
        }
        int roomInstance = gs->stateValues[gsIdx];

        msg = new SpriteAction(0x1F, roomInstance, parentHandlerId, parentModuleParam, 4, param, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = (int*)currentAction->messagesQueue;
        list[2] = list[0];
        if (list[3] == 1 || list[3] == 2) {
            if (list[0] == 0) {
                ((Queue*)list)->InsertAtCurrent((void*)msg);
            } else {
                while (list[2] != 0) {
                    if (*(int*)(*(int*)(list[2] + 8)) < *(int*)msg) {
                        ((LinkedList*)list)->InsertNode(msg);
                        goto done;
                    }
                    if (list[1] == list[2]) {
                        ((LinkedList*)list)->PushNode(msg);
                        goto done;
                    }
                    if (list[2] != 0) {
                        list[2] = *(int*)(list[2] + 4);
                    }
                }
            }
        } else {
            ((Queue*)list)->InsertAtCurrent((void*)msg);
        }
    }
    else if (strcmp(token, "RECT") == 0) {
        int l, t, r, b;
        l = 0; t = 0; r = 0; b = 0;
        sscanf(param_1, " %s %d %d %d %d", token, &l, &t, &r, &b);
        hotspotPos.x = l;
        hotspotPos.y = t;
        hotspotRight = r;
        hotspotBottom = b;
    }
    else if (strcmp(token, "CONTROLCAPTUREOFF") == 0) {
        flags |= 2;
    }
    else if (strcmp(token, "TUCKER") == 0) {
        flags |= 4;
        flags |= 8;
    }
    else if (strcmp(token, "MARSH") == 0) {
        flags |= 4;
        flags |= 0x10;
    }
    else if (strcmp(token, "DANIELS") == 0) {
        flags |= 4;
        flags |= 0x20;
    }
    else if (strcmp(token, "LEVEL") == 0) {
        sscanf(param_1, " %s %d", token, &currentIndex);
        if (currentIndex >= 8) {
            ShowError("illegal index %d %s", currentIndex, param_1);
        }

        action = new HotspotAction(hotspotHandle);
        items[currentIndex] = action;
        items[currentIndex]->parentHandlerId = parentHandlerId;
        items[currentIndex]->parentModuleParam = parentModuleParam;
        Parser::ProcessFile((Parser*)items[currentIndex], this, (char*)0);
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("T_Hotspot");
    }

done:
    return 0;
}
