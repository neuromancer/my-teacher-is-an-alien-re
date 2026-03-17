#include "SC_PRHotSpot.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include "LinkedList.h"
#include "Memory.h"
#include <string.h>

#include "InputManager.h"
extern "C" void ShowError(const char* format, ...);
extern void __fastcall FUN_0042b0f0(void*);
extern void __fastcall FUN_00444af0(void*); // SpriteAction dtor

extern InputManager* DAT_0046aa08;

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
                    FUN_00444af0(data);
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(actionList);
        actionList = 0;
    }
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
        InputState* pMouse = DAT_0046aa08->pMouse;
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
                        FUN_0042b0f0((void*)owner);
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
