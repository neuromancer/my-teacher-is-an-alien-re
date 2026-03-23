#include "SC_PRHotSpot.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include "LinkedList.h"
#include "Memory.h"
#include "GameState.h"
#include <string.h>
#include <stdio.h>

#include "InputManager.h"
extern "C" void ShowError(const char* format, ...);
extern "C" char* GetSoundFilename(int handle);
extern void __fastcall PracticeRoomNotify(void*);
extern void ParseSpriteAction(void* param_1, void* param_2);
extern "C" GameState* g_GameState_0046aa30;
extern GameState* g_GameState2_0046aa3c;

extern InputManager* g_InputManager_0046aa08;

/* Function start: 0x429B60 */
SC_PRHotSpot::SC_PRHotSpot(int param_1, int param_2) : Parser()
{
    boundsLeft = 0;
    boundsTop = 0;
    boundsRight = 0;
    boundsBottom = 0;
    memset(&state, 0, 0x30);
    state = 1;
    hotspotId = param_1;
    owner = param_2;
}

/* Function start: 0x429C10 */
SC_PRHotSpot::~SC_PRHotSpot()
{
    if (sprite != 0) {
        sprite->~Sprite();
        FreeMemory(sprite);
        sprite = 0;
    }
    if (hoverSound != 0) {
        hoverSound->~Sample();
        FreeMemory(hoverSound);
        hoverSound = 0;
    }
    if (clickSound != 0) {
        clickSound->~Sample();
        FreeMemory(clickSound);
        clickSound = 0;
    }
    if (exitSound != 0) {
        exitSound->~Sample();
        FreeMemory(exitSound);
        exitSound = 0;
    }
    if (actionList != 0) {
        if (actionList->head != 0) {
            actionList->current = actionList->head;
            while (actionList->head != 0) {
                void* data = actionList->RemoveCurrent();
                if (data != 0) {
                    delete (SpriteAction*)data;
                }
            }
        }
        FreeMemory(actionList);
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
        sscanf(param_1, "%s %d %d %d %d", local_3c, &boundsLeft, &boundsTop, &boundsRight, &boundsBottom);
    } else if (strcmp(local_3c, "ROLLON") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        if (hoverSound != 0) {
            hoverSound->Unload();
            operator delete(hoverSound);
            hoverSound = 0;
        }
        Sample* snd = new Sample();
        hoverSound = snd;
        snd->Load(filename);
    } else if (strcmp(local_3c, "ROLLOFF") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        if (exitSound != 0) {
            exitSound->Unload();
            operator delete(exitSound);
            exitSound = 0;
        }
        Sample* snd = new Sample();
        exitSound = snd;
        snd->Load(filename);
    } else if (strcmp(local_3c, "SELECTED") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        char* filename = GetSoundFilename(local_18);
        if (clickSound != 0) {
            clickSound->Unload();
            operator delete(clickSound);
            clickSound = 0;
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
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        actionList->current = actionList->head;
        if (actionList->type == 1 || actionList->type == 2) {
            if (actionList->head == 0) {
                actionList->InsertNode(action);
            } else {
                do {
                    int cur = (int)actionList->current;
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        actionList->InsertNode(action);
                        break;
                    }
                    if ((int)actionList->tail == cur) {
                        actionList->PushNode(action);
                        break;
                    }
                    if (cur != 0) {
                        actionList->current = (ListNode*)*(int*)(cur + 4);
                    }
                } while (actionList->current != 0);
            }
        } else {
            actionList->InsertNode(action);
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
            g_GameState2_0046aa3c->FindState(local_13c),
            local_1c, 0, 0, 0, 0);
        if ((action->instruction == 0x11 || action->instruction == 0x12) && result < 4) {
            action->extra1 = 1;
        }
        if (action->instruction == 0x11 && result < 4) {
            Parser::LBLParse("SC_PRHotSpot");
        }
        if (action == 0) {
            ShowError("queue fault 0101");
        }
        actionList->current = actionList->head;
        if (actionList->type == 1 || actionList->type == 2) {
            if (actionList->head == 0) {
                actionList->InsertNode(action);
            } else {
                do {
                    int cur = (int)actionList->current;
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)action) {
                        actionList->InsertNode(action);
                        break;
                    }
                    if ((int)actionList->tail == cur) {
                        actionList->PushNode(action);
                        break;
                    }
                    if (cur != 0) {
                        actionList->current = (ListNode*)*(int*)(cur + 4);
                    }
                } while (actionList->current != 0);
            }
        } else {
            actionList->InsertNode(action);
        }
    } else if (strcmp(local_3c, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_PRHotSpot");
    }

    return 0;
}

/* Function start: 0x429DF0 */
void SC_PRHotSpot::Update()
{
    if (state > 3) {
        ShowError("SC_PRHotSpot::Update()");
        return;
    }

    if (state == 0) {
        return;
    }

    if (state == 1 || state == 2) {
        int mouseY = 0;
        InputState* pMouse = g_InputManager_0046aa08->pMouse;
        if (pMouse != 0) {
            mouseY = pMouse->y;
        }
        int mouseX;
        if (pMouse != 0) {
            mouseX = pMouse->x;
        } else {
            mouseX = 0;
        }

        if (state == 2) {
            // Hovering: check if still inside bounds
            if (boundsLeft <= mouseX && boundsRight >= mouseX &&
                boundsTop <= mouseY && boundsBottom >= mouseY) {
                if (sprite != 0) {
                    sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
                }
                return;
            }
            // Moved outside — back to idle
            state = 1;
            if (sprite != 0) {
                sprite->ResetAnimation(1, 0);
            }
            if (exitSound != 0) {
                exitSound->Play(100, 1);
            }
        } else {
            // Idle: check if cursor entered bounds
            if (boundsLeft > mouseX || boundsRight < mouseX ||
                boundsTop > mouseY || boundsBottom < mouseY) {
                goto do_animate;
            }
            // Entered — transition to hover
            state = 2;
            if (sprite != 0) {
                sprite->ResetAnimation(2, 0);
            }
            if (hoverSound != 0) {
                hoverSound->Play(100, 1);
            }
        }

    do_animate:
        if (sprite != 0) {
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        return;
    }

    // State 3: clicked — check audio completion
    if (state == 3) {
        if (clickSound != 0) {
            int sampleHandle = (int)clickSound->m_sample;
            if (sampleHandle != 0) {
                int status = *(int*)(sampleHandle + 0xc);
                if (clickSound->m_size == status) {
                    // Audio finished
                }
            }
        }

        // Execute action list
        if (actionList != 0) {
            actionList->current = actionList->head;
            if (actionList->head != 0) {
                while (1) {
                    void* nodeData;
                    if (actionList->current == 0) {
                        nodeData = 0;
                    } else {
                        nodeData = (void*)actionList->current->data;
                    }
                    EnqueueSpriteAction(nodeData);

                    if (actionList->tail == actionList->current) {
                        break;
                    }
                    if (actionList->current != 0) {
                        actionList->current = actionList->current->next;
                    }
                    if (actionList->head == 0) {
                        break;
                    }
                }
            }
            state = 1;
        }

        if (sprite != 0) {
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        return;
    }
}

/* Function start: 0x42A010 */
int SC_PRHotSpot::CheckCollision(void* param_1)
{
    int* msg = (int*)param_1;
    int mx = msg[7];

    if (boundsLeft <= mx && boundsRight >= mx) {
        int my = msg[8];
        if (boundsTop <= my && boundsBottom >= my) {
            if (state != 0) {
                if (msg[9] > 1) {
                    if (hotspotId >= 0x14 && hotspotId <= 0x16) {
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
        }
    }
    return 0;
}
