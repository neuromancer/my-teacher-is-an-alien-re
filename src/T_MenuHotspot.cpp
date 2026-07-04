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
#include "string.h"

/* Function start: 0x420CE0 */
T_MenuHotspot::T_MenuHotspot(int param)
{
    memset(&sprite, 0, 0x46 * 4);
    sprite = (Sprite*)1;
    hotspotParam = param;
}

/* Function start: 0x421A50 */
T_MenuButton::T_MenuButton(char* name, int* rect)
{
    // Original bug at 0x421A50: REP STOSD wipes this after setting the vptr.
    memset(this, 0, 0x2a * 4);

    if (name != 0) {
        sprite = new Sprite(name);
        sprite->priority = 2;
        sprite->loc.x = rect[0];
        sprite->loc.y = rect[1];
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
    if (messageQueue != 0) {
        LinkedList* list = (LinkedList*)messageQueue;
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        operator delete((void*)messageQueue);
        messageQueue = 0;
    }
}

/* Function start: 0x420F00 */
void T_MenuButton::Update()
{
    T_MenuHotspot* hs = (T_MenuHotspot*)this;
    if (sprite == 0) {
        return;
    }
    int mouseY;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }
    int mouseX;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    if (sprite != 0 &&
        hs->bounds.left <= mouseX && hs->bounds.right >= mouseX &&
        hs->bounds.top <= mouseY && hs->bounds.bottom >= mouseY) {
        // Inside bounds
        int st = hs->state;
        if (st == 0 || st == 3) {
            hs->state = 3;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(3, 0);
            }
        } else if (st == 1 || st == 2) {
            hs->state = 2;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(2, 0);
            }
        } else {
            return;
        }
    } else {
        // Outside bounds
        int st = hs->state;
        if (st == 0 || st == 3) {
            hs->state = 0;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(0, 0);
            }
        } else if (st == 1 || st == 2) {
            hs->state = 1;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(1, 0);
            }
        } else {
            return;
        }
    }

    if (hs->cursor != 0) {
        hs->cursor->Do(hs->cursor->loc.x, hs->cursor->loc.y, 1.0);
    }
}

/* Function start: 0x421BC0 */
void T_MenuButton::SimpleUpdate()
{
    if (sprite == 0) {
        return;
    }

    int mouseY;
    int mouseX;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    if (bounds.left <= mouseX && bounds.right >= mouseX &&
        bounds.top <= mouseY && bounds.bottom >= mouseY) {
        sprite->ResetAnimation(1, 0);
    } else {
        sprite->ResetAnimation(0, 0);
    }

    sprite->Do(sprite->loc.x, sprite->loc.y, 1.0);
}


extern void ParseSpriteAction(void*, void*);
// FUN_0040cf40 = ListNode::Init — constructor in LinkedList.h

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
    SpriteAction* action;

    local_18 = 0;
    local_1c = 0;
    local_24 = 0;
    local_20 = 0;
    sscanf(line, " %s ", local_50);

    if (strcmp(local_50, "RECT") == 0) {
        local_30 = 0;
        local_2c = 0;
        local_28 = 0;
        local_24 = 0;
        sscanf(line, " %s %d %d %d %d", local_50, &local_30, &local_2c, &local_28, &local_24);
        bounds.left = local_30;
        bounds.top = local_2c;
        bounds.right = local_28;
        bounds.bottom = local_24;
    } else if (strcmp(local_50, "SPRITE") == 0) {
        cursor = new Sprite((char*)0);
        Parser::ProcessFile(cursor, this, (char*)0);
    } else if (strcmp(local_50, "MOUSE") == 0) {
        sscanf(line, "%s %d", local_50, &mouseMode);
    } else if (strcmp(local_50, "HSOUND") == 0) {
        sscanf(line, "%s %d %d %d %d", local_50, &local_18, &local_1c, &local_24, &local_20);
        soundEntries[local_18 * 3 + 2] = local_1c;
        soundEntries[local_18 * 3 + 1] = local_24;
        soundEntries[local_18 * 3] = local_20;
    } else if (strcmp(local_50, "MESSAGE") == 0) {
        if (messageQueue == 0) {
            messageQueue = (int)new Queue();
        }
        action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(action, this);
        {
            LinkedList* q = (LinkedList*)messageQueue;
            q->ResetForSortedAdd(action);
            if (q->type == 1 || q->type == 2) {
                if (q->head == 0) {
                    ((Queue*)q)->InsertAtCurrent(action);
                } else {
                    do {
                        ListNode* cur = q->current;
                        if (*(int*)(cur->data) < *(int*)action) {
                            q->InsertNode(action);
                            break;
                        }
                        if (q->tail == cur) {
                            q->PushNode(action);
                            break;
                        }
                        if (cur != 0) {
                            q->current = cur->next;
                        }
                    } while (q->current != 0);
                }
            } else {
                ((Queue*)q)->InsertAtCurrent(action);
            }
        }
    } else if (strcmp(local_50, "SWITCHFOCUS") == 0) {
        // Original bug at 0x421560: extra output arg is ignored, leaving local_1c at its initialized value.
        int result = sscanf(line, " %s %s %d %s", local_50, local_70, &local_18, local_90, &local_1c);
        if (result != 4) {
            ShowError("Error in ThotsMen.cpp: %s - in parse file is missing parameters", line);
        }
        if (messageQueue == 0) {
            messageQueue = (int)new Queue();
        }
        action = new SpriteAction(
            g_StringTable_0046aa34->FindState((char*)local_70),
            local_18,
            g_StringTable_0046aa34->FindState((char*)local_90),
            local_1c, 4, 0, 0, 0, 0, 0);
        {
            LinkedList* q = (LinkedList*)messageQueue;
            q->ResetForSortedAdd(action);
            if (q->type == 1 || q->type == 2) {
                if (q->head == 0) {
                    ((Queue*)q)->InsertAtCurrent(action);
                } else {
                    do {
                        ListNode* cur = q->current;
                        if (*(int*)(cur->data) < *(int*)action) {
                            q->InsertNode(action);
                            break;
                        }
                        if (q->tail == cur) {
                            q->PushNode(action);
                            break;
                        }
                        if (cur != 0) {
                            q->current = cur->next;
                        }
                    } while (q->current != 0);
                }
            } else {
                ((Queue*)q)->InsertAtCurrent(action);
            }
        }
    } else if (strcmp(local_50, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("T_MenuHotspot");
    }

    return 0;
}

void T_MenuButton::Cleanup() {}

/* Function start: 0x421020 */
void T_MenuHotspot::ProcessSpriteActions() {
    if (messageQueue == 0) return;

    ((LinkedList*)messageQueue)->current = ((LinkedList*)messageQueue)->head;

    if (((LinkedList*)messageQueue)->head == 0) return;

    do {
        ListNode* cur = ((LinkedList*)messageQueue)->current;
        SpriteAction* data;
        if (cur != 0) {
            data = (SpriteAction*)cur->data;
        } else {
            data = 0;
        }
        EnqueueSpriteAction(data);
        ListNode* curNode = ((LinkedList*)messageQueue)->current;
        if (((LinkedList*)messageQueue)->tail == curNode) {
            return;
        }
        if (curNode != 0) {
            ((LinkedList*)messageQueue)->current = curNode->next;
        }
    } while (((LinkedList*)messageQueue)->head != 0);
}

/* Function start: 0x420EF0 */
void T_MenuHotspot::StopCursorSound() {
    if (cursor != 0) {
        ((Sprite*)cursor)->StopAnimationSound();
    }
}
