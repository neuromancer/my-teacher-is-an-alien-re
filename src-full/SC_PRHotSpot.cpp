#include "SC_PRHotSpot.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include <string.h>

extern void __fastcall FUN_0044cb40(void*, int, int, int);
extern void __fastcall FUN_00425100(void*, int, int, int);
extern void __fastcall FUN_0044ccf0(void*, int, int, int, int);
extern void __fastcall FUN_0044c740(void*);
extern void __fastcall FUN_00424ee0(void*);
extern "C" void ShowError(const char* format, ...);
extern void __fastcall FUN_0042b0f0(void*);
extern "C" void FreeMemory(void*);


extern void* DAT_0046aa08;

/* Function start: 0x429470 */ /* already in SC_DodgeOrville.cpp */

/* Function start: 0x429B60 */
SC_PRHotSpot::SC_PRHotSpot(int param_1, int param_2) : Parser()
{
    field_94 = 0;
    field_98 = 0;
    field_9c = 0;
    field_a0 = 0;
    *(int*)this = 0x461508;
    memset(&field_90, 0, 0x30);
    field_90 = 1;
    field_a4 = param_1;
    field_b8 = param_2;
}

/* Function start: 0x429DF0 */
void SC_PRHotSpot::Update()
{
    int state = field_90;

    if (state > 3) {
        ShowError("SC_PRHotSpot::Update()");
        return;
    }

    if (state == 0) {
        return;
    }

    if (state == 1 || state == 2) {
        void* inputMgr = DAT_0046aa08;
        int mouseY = 0;
        void* mouse = *(void**)((char*)inputMgr + 0x1a0);
        if (mouse != 0) {
            mouseY = *(int*)((char*)mouse + 0x4);
        }
        int mouseX;
        if (mouse != 0) {
            mouseX = *(int*)mouse;
        } else {
            mouseX = 0;
        }

        if (state == 2) {
            // State 2: check if cursor is inside bounds
            if (field_94 <= mouseX && field_9c >= mouseX &&
                field_98 <= mouseY && field_a0 >= mouseY) {
                // Still inside - animate
                if ((void*)field_a8 != 0) {
                    Sprite* spr = (Sprite*)field_a8;
                    spr->Do(spr->loc_x, spr->loc_y, 1.0);
                }
                return;
            }
            // Moved outside - transition to state 1
            field_90 = 1;
            if ((void*)field_a8 != 0) {
                FUN_0044cb40((void*)field_a8, 0, 1, 0);
            }
            if ((void*)field_b4 != 0) {
                FUN_00425100((void*)field_b4, 0, 0x64, 1);
            }
        } else {
            // State 1: check if cursor entered bounds
            if (field_94 > mouseX || field_9c < mouseX ||
                field_98 > mouseY || field_a0 < mouseY) {
                goto do_animate;
            }
            // Entered bounds - transition to state 2
            field_90 = 2;
            if ((void*)field_a8 != 0) {
                FUN_0044cb40((void*)field_a8, 0, 2, 0);
            }
            if ((void*)field_ac != 0) {
                FUN_00425100((void*)field_ac, 0, 0x64, 1);
            }
        }

    do_animate:
        if ((void*)field_a8 != 0) {
            Sprite* spr = (Sprite*)field_a8;
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        return;
    }

    // State 3: check audio
    if (state == 3) {
        // Check if sound finished
        if ((void*)field_b0 != 0) {
            void* sndSpr = (void*)field_b0;
            int data = *(int*)((char*)sndSpr + 0xc);
            if (data != 0) {
                int val = *(int*)((char*)data + 0xc);
                if (*(int*)((char*)sndSpr + 0x4) == val) {
                    // TODO: check audio status
                }
            }
        }

        // Process linked list
        void* list = (void*)field_bc;
        if (list != 0) {
            *(int*)((char*)list + 0x8) = *(int*)list;
            if (*(int*)list != 0) {
                while (1) {
                    void* listPtr = (void*)field_bc;
                    int current = *(int*)((char*)listPtr + 0x8);
                    int nodeData;
                    if (current == 0) {
                        nodeData = 0;
                    } else {
                        nodeData = *(int*)((char*)(void*)current + 0x8);
                    }
                    EnqueueSpriteAction((void*)nodeData);

                    listPtr = (void*)field_bc;
                    int cur = *(int*)((char*)listPtr + 0x8);
                    if (*(int*)((char*)listPtr + 0x4) == cur) {
                        break;
                    }
                    if (cur != 0) {
                        *(int*)((char*)listPtr + 0x8) = *(int*)((char*)(void*)cur + 0x4);
                    }
                    listPtr = (void*)field_bc;
                    if (*(int*)listPtr == 0) {
                        break;
                    }
                }
            }
            field_90 = 1;
        }

        // Animate
        if ((void*)field_a8 != 0) {
            Sprite* spr = (Sprite*)field_a8;
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        return;
    }
}

/* Function start: 0x42A010 */
int SC_PRHotSpot::CheckCollision(void* param_1)
{
    int* msg = (int*)param_1;
    int mx = msg[7]; // 0x1c

    if (field_94 <= mx && field_9c >= mx) {
        int my = msg[8]; // 0x20
        if (field_98 <= my && field_a0 >= my) {
            if (field_90 != 0) {
                if (msg[9] > 1) { // 0x24
                    if (field_a4 >= 0x14 && field_a4 <= 0x16) {
                        FUN_0042b0f0((void*)field_b8);
                    }

                    field_90 = 3;
                    if ((void*)field_a8 != 0) {
                        FUN_0044cb40((void*)field_a8, 0, 3, 0);
                    }
                    if ((void*)field_b0 != 0) {
                        FUN_00425100((void*)field_b0, 0, 0x64, 1);
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}
