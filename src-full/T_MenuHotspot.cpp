#include "T_MenuHotspot.h"
#include "Memory.h"
#include "InputManager.h"
#include "SpriteAction.h"
#include "LinkedList.h"
#include "globals.h"
#include "GameState.h"
#include <string.h>
#include <stdio.h>
#include <new>

extern InputManager* g_InputManager_0046aa08;

/* Function start: 0x420CE0 */
T_MenuHotspot::T_MenuHotspot(int param) : Parser()
{
    memset(&sprite, 0, 0x46 * 4);
    sprite = (Sprite*)1;
    field_1A4 = param;
}

/* Function start: 0x421A50 */
T_MenuHotspot::T_MenuHotspot(char* name, int* rect)
{
    memset(this, 0, 0x2a * 4);

    if (name != 0) {
        sprite = new Sprite(name);
        sprite->priority = 2;
        sprite->loc_x = rect[0];
        sprite->loc_y = rect[1];
        sprite->ConfigStates(2);
        sprite->flags |= 0x40;
        sprite->ConfigRange(0, 1, 1, 1);
        sprite->ConfigRange(1, 2, 2, 1);
    }

    bounds.left = rect[0];
    bounds.top = rect[1];
    bounds.right = rect[2];
    bounds.bottom = rect[3];
}

/* Function start: 0x420D90 */
T_MenuHotspot::~T_MenuHotspot()
{
    if (cursor != 0) {
        delete cursor;
        cursor = 0;
    }
    if (field_1A0 != 0) {
        LinkedList* list = (LinkedList*)field_1A0;
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        operator delete((void*)field_1A0);
        field_1A0 = 0;
    }
}

/* Function start: 0x421BC0 */
void T_MenuHotspot::Update()
{
    if (cursor == 0) {
        return;
    }

    int mouseX = 0;
    int* pMouse = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
    if (pMouse != 0) {
        mouseX = pMouse[1];
    }
    int mouseY;
    if (pMouse != 0) {
        mouseY = pMouse[0];
    } else {
        mouseY = 0;
    }

    if (bounds.left <= mouseY && bounds.right >= mouseY &&
        bounds.top <= mouseX && bounds.bottom >= mouseX) {
        cursor->ResetAnimation(1, 0);
    } else {
        cursor->ResetAnimation(0, 0);
    }

    cursor->Do(cursor->loc_x, cursor->loc_y, 1.0);
}

extern "C" void ShowError(const char* format, ...);

extern void ParseSpriteAction(void*, void*);
extern void* __fastcall FUN_0040cf40(void*, int, void*);

/* Function start: 0x421080 */
int T_MenuHotspot::LBLParse(char* line) {
    char local_90[32];
    char local_70[32];
    char local_50[32];
    int local_30;
    int local_2c;
    int local_28;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    int* queue;
    SpriteAction* action;

    local_18 = 0;
    local_1c = 0;
    local_24 = 0;
    local_20 = 0;
    sscanf(line, " %s ", local_50);

    if (strcmp(local_50, "HOTSPOT") == 0) {
        local_30 = 0;
        local_2c = 0;
        local_28 = 0;
        int local_tmp = 0;
        sscanf(line, " %s %d %d %d %d", local_50, &local_30, &local_2c, &local_28, &local_tmp);
        bounds.left = local_30;
        bounds.top = local_2c;
        bounds.left = local_28;
        bounds.top = local_tmp;
    } else if (strcmp(local_50, "SPRITE") == 0) {
        cursor = new Sprite((char*)0);
        Parser::ProcessFile(cursor, this, (char*)0);
    } else if (strcmp(local_50, "MOUSE") == 0) {
        sscanf(line, " %s %d", local_50, &bounds.top);
    } else if (strcmp(local_50, "HSOUND") == 0) {
        sscanf(line, " %s %d %d %d %d", local_50, &local_18, &local_1c, &local_24, &local_20);
        *(int*)((char*)this + local_18 * 0xC + 0xB4) = local_1c;
        *(int*)((char*)this + local_18 * 0xC + 0xB0) = local_24;
        *(int*)((char*)this + local_18 * 0xC + 0xAC) = local_20;
    } else if (strcmp(local_50, "MESSAGE") == 0) {
        if (field_1A0 == 0) {
            field_1A0 = (int)new Queue();
        }
        action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(action, this);
        queue = (int*)field_1A0;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    int cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        ((Queue*)queue)->Push(action);
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
    } else if (strcmp(local_50, "SWITCHFOCUS") == 0) {
        int result = sscanf(line, " %s %s %d %s %d", local_50, local_70, &local_18, local_90, &local_1c);
        if (result != 4) {
            ShowError("Error in ThotsMen.cpp: %s in parse file is incomplete");
        }
        if (field_1A0 == 0) {
            field_1A0 = (int)new Queue();
        }
        {
            int key1 = g_StringTable_0046aa34->FindState((char*)local_70);
            int key2 = g_StringTable_0046aa34->FindState((char*)local_90);
            action = new SpriteAction(key1, local_18, key2, local_1c, 4, 0, 0, 0, 0, 0);
        }
        queue = (int*)field_1A0;
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(action);
            } else {
                do {
                    int cur = queue[2];
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        ((Queue*)queue)->InsertAtCurrent(action);
                        break;
                    }
                    if (queue[1] == cur) {
                        ((Queue*)queue)->Push(action);
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
    } else if (strcmp(local_50, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("T_MenuHotspot");
    }

    return 0;
}

void T_MenuHotspot::Cleanup() {}

