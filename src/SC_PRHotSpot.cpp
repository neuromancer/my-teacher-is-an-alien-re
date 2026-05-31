#include "SC_PRHotSpot.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include "LinkedList.h"
#include "Queue.h"
#include "Memory.h"
#include "GameState.h"
#include <string.h>
#include <stdio.h>

#include "InputManager.h"
extern "C" void ShowError(const char* format, ...);
extern "C" char* GetSoundFilename(int handle);
extern void __fastcall PracticeRoomNotify(void*);
extern void ParseSpriteAction(void* param_1, void* param_2);
#include "globals.h"


/* Function start: 0x429B60 */
SC_PRHotSpot::SC_PRHotSpot(int param_1, int param_2) : Parser()
{
    memset(&state, 0, 0x30);
    state = 1;
    hotspotId = param_1;
    owner = param_2;
}

/* Function start: 0x429C10 */
SC_PRHotSpot::~SC_PRHotSpot()
{
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
    {
        Sample* snd = hoverSound;
        if (snd != 0) {
            snd->Unload();
            operator delete(snd);
            hoverSound = 0;
        }
    }
    {
        Sample* snd = clickSound;
        if (snd != 0) {
            snd->Unload();
            operator delete(snd);
            clickSound = 0;
        }
    }
    {
        Sample* snd = exitSound;
        if (snd != 0) {
            snd->Unload();
            operator delete(snd);
            exitSound = 0;
        }
    }
    if (actionList != 0) {
        if (actionList->head != 0) {
            actionList->current = actionList->head;
            while (actionList->head != 0) {
                void* data = actionList->RemoveCurrent();
                if (data != 0) {
                    ((SpriteAction*)data)->~SpriteAction();
                    operator delete(data);
                }
            }
        }
        operator delete(actionList);
        actionList = 0;
    }
}

/* Function start: 0x42A0C0 */
int SC_PRHotSpot::LBLParse(char* param_1) {
    char local_bc[128];
    char local_3c[32];
    int local_18;
    int local_1c;

    local_18 = 0;
    local_1c = 0;
    local_bc[0] = 0;
    local_3c[0] = 0;

    sscanf(param_1, "%s", local_3c);

    if (strcmp(local_3c, "SPRITE") == 0) {
        if (sprite != 0) {
            delete sprite;
            sprite = 0;
        }
        Sprite* newSprite = new Sprite(0);
        sprite = newSprite;
        Parser::ProcessFile(sprite, this, 0);
    } else if (strcmp(local_3c, "LOC") == 0) {
        sscanf(param_1, " %s %d %d %d %d", local_3c, &bounds.left, &bounds.top, &bounds.right, &bounds.bottom);
    } else if (strcmp(local_3c, "ROLLON") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        {
            Sample* old = hoverSound;
            if (old != 0) {
                old->Unload();
                operator delete(old);
                hoverSound = 0;
            }
        }
        Sample* snd = new Sample();
        hoverSound = snd;
        snd->Load(filename);
    } else if (strcmp(local_3c, "ROLLOFF") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        {
            Sample* old = exitSound;
            if (old != 0) {
                old->Unload();
                operator delete(old);
                exitSound = 0;
            }
        }
        Sample* snd = new Sample();
        exitSound = snd;
        snd->Load(filename);
    } else if (strcmp(local_3c, "SELECTED") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        {
            Sample* old = clickSound;
            if (old != 0) {
                old->Unload();
                operator delete(old);
                clickSound = 0;
            }
        }
        Sample* snd = new Sample();
        clickSound = snd;
        snd->Load(filename);
    } else if (strcmp(local_3c, "MESSAGE") == 0) {
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(action, this);
        if (actionList == 0) {
            LinkedList* newList = new LinkedList();
            actionList = newList;
        }
        {
            LinkedList* list = actionList;
            list->ResetForSortedAdd(action);
            if (list->type == 1 || list->type == 2) {
                if (list->head == 0) {
                    ((Queue*)list)->InsertAtCurrent(action);
                } else {
                    do {
                        int cur = (int)list->current;
                        if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                            ((Queue*)list)->InsertAtCurrent(action);
                            break;
                        }
                        if ((int)list->tail == cur) {
                            ((Queue*)list)->Push(action);
                            break;
                        }
                        if (cur != 0) {
                            list->current = (ListNode*)*(int*)(cur + 4);
                        }
                    } while (list->current != 0);
                }
            } else {
                ((Queue*)list)->InsertAtCurrent(action);
            }
        }
    } else if (strcmp(local_3c, "GAMESTATE") == 0) {
        char local_13c[128];
        int result;
        result = sscanf(param_1, "%s %s %s %d", local_3c, local_bc, local_13c, &local_1c);
        if (actionList == 0) {
            LinkedList* newList = new LinkedList();
            actionList = newList;
        }
        SpriteAction* action = new SpriteAction(
            2,
            g_GameState_0046aa30->FindState(local_bc),
            0, 0,
            g_StringState_0046aa38->FindState(local_13c),
            local_1c, 0, 0, 0, 0);
        if ((action->instruction == 0x11 || action->instruction == 0x12) && result < 4) {
            action->extra1 = 1;
        }
        if (action->instruction == 0x11 && result < 4) {
            ReportUnknownLabel("SC_Simple_Hotspot in gamestate");
        }
        {
            LinkedList* list = actionList;
            list->ResetForSortedAdd(action);
            if (list->type == 1 || list->type == 2) {
                if (list->head == 0) {
                    ((Queue*)list)->InsertAtCurrent(action);
                } else {
                    do {
                        int cur = (int)list->current;
                        if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                            ((Queue*)list)->InsertAtCurrent(action);
                            break;
                        }
                        if ((int)list->tail == cur) {
                            list->PushNode(action);
                            break;
                        }
                        if (cur != 0) {
                            list->current = (ListNode*)*(int*)(cur + 4);
                        }
                    } while (list->current != 0);
                }
            } else {
                ((Queue*)list)->InsertAtCurrent(action);
            }
        }
    } else if (strcmp(local_3c, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_PRHotSpot");
    }

    return 0;
}

extern int __stdcall AIL_sample_status(void*);

/* Function start: 0x429DF0 */
void SC_PRHotSpot::Update()
{
    int mouseX;
    int mouseY;
    InputState* pMouse;

    switch (state) {
    case 0:
        return;
    case 1:
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
            state = 2;
            if (sprite != 0) {
                sprite->ResetAnimation(2, 0);
            }
            if (hoverSound != 0) {
                hoverSound->Play(100, 1);
            }
        }
        if (sprite != 0) {
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        return;
    case 2:
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
        if (bounds.left > mouseX || bounds.right < mouseX ||
            bounds.top > mouseY || bounds.bottom < mouseY) {
            state = 1;
            if (sprite != 0) {
                sprite->ResetAnimation(1, 0);
            }
            if (exitSound != 0) {
                exitSound->Play(100, 1);
            }
        }
        if (sprite != 0) {
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        return;
    case 3: {
        HSAMPLE hsamp;
        if (clickSound != 0 && (hsamp = clickSound->m_sample) != 0 &&
            clickSound->m_size == *(int*)((char*)hsamp + 0xc) &&
            AIL_sample_status(hsamp) == 4) {
            goto do_sprite;
        }
        if (actionList != 0) {
            actionList->current = actionList->head;
            if (actionList->head != 0) {
                do {
                    void* nodeData = 0;
                    if (actionList->current != 0) {
                        nodeData = actionList->current->data;
                    }
                    EnqueueSpriteAction(nodeData);
                    if (actionList->tail == actionList->current) break;
                    if (actionList->current != 0) {
                        actionList->current = actionList->current->next;
                    }
                } while (actionList->head != 0);
            }
        }
        state = 1;
    do_sprite:
        if (sprite != 0) {
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        return;
    }
    default:
        ShowError("SC_PRHotSpot::Update()");
        return;
    }
}

/* Function start: 0x42A010 */
int SC_PRHotSpot::CheckCollision(void* param_1)
{
    int* msg = (int*)param_1;
    int mx = msg[7];

    int inBounds = (bounds.left <= mx && bounds.right >= mx &&
                    bounds.top <= msg[8] && bounds.bottom >= msg[8]);
    if (inBounds == 0 || state == 0) {
        return 0;
    }
    if (msg[9] > 1) {
        if ((unsigned int)hotspotId >= 0x14 && (unsigned int)hotspotId <= 0x16) {
            PracticeRoomNotify((void*)owner);
        }
        state = 3;
        if (sprite != 0) {
            sprite->ResetAnimation(3, 0);
        }
        if (clickSound != 0) {
            clickSound->Play(100, 1);
        }
    }
    return 1;
}
