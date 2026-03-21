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

extern "C" GameState* g_GameState_0046aa30;

#include "HotspotAction.h"
#include "GameState.h"

T_Hotspot::T_Hotspot()
{
    memset(&sprite, 0, 0x2e * 4);
    enabled = 1;
    state = 1;
    dialogParseFileNumber = -1;
    parseFileIndex = -1;
}

/* Function start: 0x445830 */
T_Hotspot::T_Hotspot(int param_1) : Parser()
{
    dim_A4.field_0 = 0;
    dim_A4.field_4 = 0;
    field_AC = 0;
    field_B0 = 0;
    memset(&field_90, 0, 0x12 * 4);
    field_98 = param_1;
}

/* Function start: 0x4458D0 */
T_Hotspot::~T_Hotspot()
{
    void* ptr;
    int i;
    void** p = &items[0];

    for (i = 0; i < 8; i++) {
        ptr = *p;
        if (ptr != 0) {
            delete (HotspotAction*)ptr;
            *p = 0;
        }
        p++;
    }
}


/* Function start: 0x4459A0 */
void T_Hotspot::DoItem(int param)
{
    T_Hotspot::SelectItem();
    void* item = T_Hotspot::items[T_Hotspot::currentIndex];
    if (item != 0) {
        ((HotspotAction*)item)->Update(param);
    }
}

/* Function start: 0x4459D0 */
void T_Hotspot::SelectItem()
{
    int i;
    void** p;

    p = &items[0];
    for (i = 0; i < 8; i++) {
        if (*p != 0) {
            int status = ((HotspotAction*)*p)->state;
            if (status != 0 && status == 1) {
                goto done;
            }
        }
        p++;
    }

    p = &items[0];
    for (i = 0; i < 8; i++) {
        if (*p != 0) {
            if (((HotspotAction*)*p)->CheckConditions() != 0) {
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
    void** p = &items[0];
    int i = 8;
    do {
        if (*p != 0) {
            ((HotspotAction*)*p)->Reset();
        }
        p++;
        i--;
    } while (i != 0);
}

// =========================================================================
// Hotspot class — DEMO ONLY (used by Handler6.cpp, no full game equivalent)
// =========================================================================

Hotspot::Hotspot() { memset(&hotspot, 0, 104); field_D0 = 1; state = 1; }
Hotspot::~Hotspot() {}
int Hotspot::Do() { return 0; }
int Hotspot::LBLParse(char*) { return 0; }

// Demo stubs needed by linker
void T_Hotspot::Exit() {}
int T_Hotspot::Update(int, int, int) { return 0; }

/* Function start: 0x445A30 */
int T_Hotspot::LBLParse(char* param_1)
{
    int iVar5;
    void* action;
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
        sscanf(param_1, " %s %d", token, &field_98);
    }
    else if (strcmp(token, "HOTSPOT") == 0) {
        iVar5 = sscanf(param_1, "%s %s %d", token, value, &param);
        if (iVar5 != 3) {
            ShowError("Error in Thotspot.cpp : %s - missing parameters in parse file", param_1);
        }

        action = (void*)new HotspotAction(field_98);
        field_D4 = (int)action;
        *(int*)((int)action + 0x90) = field_90;
        *(int*)(*(int*)((int)this + 0xD0 + 0x60) + 0x94) = field_94;

        if (strstr(value, "LEFT") != 0) {
            dim_A4.field_0 = 0;
            dim_A4.field_4 = 100;
            field_AC = 0x4B;
            field_B0 = 300;
            *(int*)(*(int*)((int)this + 0xD0 + 0x60) + 0xB0) = 4;
        } else if (strstr(value, "RIGHT") != 0) {
            dim_A4.field_0 = 0x234;
            dim_A4.field_4 = 100;
            field_AC = 0x27F;
            field_B0 = 300;
            *(int*)(*(int*)((int)this + 0xD0 + 0x60) + 0xB0) = 3;
        }

        // Ensure action list exists at [action + 0x100]
        action = (void*)field_D4;
        if (*(int*)((int)action + 0x100) == 0) {
            LinkedList* newList = new LinkedList();
            *(LinkedList**)((int)action + 0x100) = newList;
        }

        // Create SpriteAction
        msg = new SpriteAction(field_90, param, field_90, field_94, 4, 0, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        // Sorted insert into action list
        list = *(int**)((int)field_D4 + 0x100);
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

        action = (void*)new HotspotAction(field_98);
        field_D4 = (int)action;
        *(int*)((int)action + 0x90) = field_90;
        *(int*)((int)field_D4 + 0x94) = field_94;
        *(int*)((int)field_D4 + 0xB0) = 2;

        if (*(int*)((int)field_D4 + 0x100) == 0) {
            *(LinkedList**)((int)field_D4 + 0x100) = new LinkedList();
        }

        msg = new SpriteAction(4, param, field_90, field_94, 2, 0, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = *(int**)((int)field_D4 + 0x100);
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

        action = (void*)new HotspotAction(field_98);
        field_D4 = (int)action;
        *(int*)((int)action + 0x90) = field_90;
        *(int*)((int)field_D4 + 0x94) = field_94;
        *(int*)((int)field_D4 + 0xB0) = 2;

        if (*(int*)((int)field_D4 + 0x100) == 0) {
            *(LinkedList**)((int)field_D4 + 0x100) = new LinkedList();
        }

        msg = new SpriteAction(0x20, param2, field_90, field_94, 4, param, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = *(int**)((int)field_D4 + 0x100);
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

        action = (void*)new HotspotAction(field_98);
        field_D4 = (int)action;
        *(int*)((int)action + 0x90) = field_90;
        *(int*)((int)field_D4 + 0x94) = field_94;
        *(int*)((int)field_D4 + 0xB0) = 6;

        if (*(int*)((int)field_D4 + 0x100) == 0) {
            *(LinkedList**)((int)field_D4 + 0x100) = new LinkedList();
        }

        // Look up ROOMINSTANCE gamestate
        int* gs = (int*)g_GameState_0046aa30;
        int gsIdx = ((GameState*)gs)->FindState("ROOMINSTANCE");
        if (gsIdx < 0 || gs[0x98 / 4] - 1 < gsIdx) {
            ShowError("Invalid gamestate %d", gsIdx);
        }
        int roomInstance = ((int*)gs[0x90 / 4])[gsIdx];

        msg = new SpriteAction(0x1F, roomInstance, field_90, field_94, 4, param, 0, 0, 0, 0);
        if (msg == 0) {
            ShowError("queue fault 0101");
        }

        list = *(int**)((int)field_D4 + 0x100);
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
        dim_A4.field_0 = l;
        dim_A4.field_4 = t;
        field_AC = r;
        field_B0 = b;
    }
    else if (strcmp(token, "CONTROLCAPTUREOFF") == 0) {
        field_A0 |= 2;
    }
    else if (strcmp(token, "TUCKER") == 0) {
        field_A0 |= 4;
        field_A0 |= 8;
    }
    else if (strcmp(token, "MARSH") == 0) {
        field_A0 |= 4;
        field_A0 |= 0x10;
    }
    else if (strcmp(token, "DANIELS") == 0) {
        field_A0 |= 4;
        field_A0 |= 0x20;
    }
    else if (strcmp(token, "LEVEL") == 0) {
        sscanf(param_1, " %s %d", token, &currentIndex);
        if (currentIndex >= 8) {
            ShowError("illegal index %d %s", currentIndex, param_1);
        }

        action = (void*)new HotspotAction(field_98);
        items[currentIndex] = action;
        *(int*)(*(int*)((int)this + currentIndex * 4 + 0xB4) + 0x90) = field_90;
        *(int*)(*(int*)((int)this + currentIndex * 4 + 0xB4) + 0x94) = field_94;
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
