#include "SC_FireAlarm.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "Animation.h"
#include "Sprite.h"
#include "Palette.h"
#include "ZBuffer.h"
#include "SlimeTable.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

// FUN_00413e10 = ParseFile in Parser.h
extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetCinematicFilename(int);
extern "C" char* FormatAssetPath(char*, ...);


extern "C" void SetVideoRes(int, int);
extern "C" void ShowError(const char* format, ...);
// FUN_004279a0 = ZBuffer::ResetItems (ZBuffer.h)
extern void __fastcall FUN_00427880(void*);
#include "MouseControl.h"

// Thiscall wrapper classes for sprite/palette methods




extern int DAT_004685ac;
class InputManager;
extern InputManager* g_InputManager_0046aa08;
extern "C" extern VBuffer* g_BackBuffer_0046aa14;
class MouseControl;
extern MouseControl* g_Mouse_0046aa18;
extern int DAT_004685a0;
extern int DAT_00472bd8;
extern int DAT_00472bdc;

/* Function start: 0x407290 */
SC_FireAlarm::SC_FireAlarm() {
    memset(&field_A8, 0, 0x2A * 4);
    handlerId = 0x40;
}

/* Function start: 0x407470 */
SC_FireAlarm::~SC_FireAlarm() {
    SC_FireAlarm::ShutDown((SC_Message*)0);
}

/* Function start: 0x407590 */
void SC_FireAlarm::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    Handler::Init(msg);

    if (FileExists("CB_FireAlarm") == 0) {
        ShowLoadingScreen();
    }

    dim_B4.field_0 = 0x140;
    dim_B4.field_4 = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = pmsg[1];

    field_148 = (int)new SlimeTable();

    ParseFile(this, "mis\\CB_FireAlarm.mis", (char*)0);

    if (field_A8 == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = sprite;
    }
}

/* Function start: 0x4076E0 */
int SC_FireAlarm::ShutDown(SC_Message* msg) {
    void* p;

    p = (void*)field_C0;
    if (p != 0) {
        delete (Palette*)p;
        field_C0 = 0;
    }

    p = (void*)field_C4;
    if (p != 0) {
        delete (Sprite*)p;
        field_C4 = 0;
    }

    if (DAT_004685ac != 0) {
        int* obj = (int*)DAT_004685ac;
        int* vtbl = (int*)*obj;
        ((void (__fastcall*)(int*, int, int))vtbl[3])(obj, 0, 1);
        DAT_004685ac = 0;
    }

    p = (void*)field_BC;
    if (p != 0) {
        delete (Sprite*)p;
        field_BC = 0;
    }

    p = (void*)field_F8;
    if (p != 0) {
        delete (Sprite*)p;
        field_F8 = 0;
    }

    p = (void*)field_DC;
    if (p != 0) {
        delete (Sprite*)p;
        field_DC = 0;
    }

    p = (void*)field_110;
    if (p != 0) {
        delete (Sprite*)p;
        field_110 = 0;
    }

    p = (void*)field_10C;
    if (p != 0) {
        delete (Sprite*)p;
        field_10C = 0;
    }

    p = (void*)field_C8;
    if (p != 0) {
        delete (Sprite*)p;
        field_C8 = 0;
    }

    p = (void*)field_A8;
    if (p != 0) {
        delete (SpriteAction*)p;
        field_A8 = 0;
    }

    p = (void*)field_148;
    if (p != 0) {
        delete (SlimeTable*)p;
        field_148 = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x407930 */
int SC_FireAlarm::AddMessage(SC_Message* msg) {
    int* pmsg = (int*)msg;

    Handler::AddMessage(msg);

    if (pmsg[11] == 0x1b && savedCommand == 0x2b) {
        field_B0 = 4;
    }

    return 1;
}

/* Function start: 0x407970 */
int SC_FireAlarm::Exit(SC_Message* msg) {
    int* pmsg = (int*)msg;

    if (pmsg[0] != handlerId) {
        return 0;
    }

    switch (pmsg[4]) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x4079E0 */
void SC_FireAlarm::SendResultMessage() {
    // Destroy existing action at field_A8
    if (field_A8 != 0) {
        delete (SpriteAction*)field_A8;
        field_A8 = 0;
    }

    // Create new SpriteAction
    SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    field_A8 = action;

    // Parse win or lose message from CB_FireAlarm.mis
    Parser temp;
    if (field_AC & 2) {
        ParseFile(&temp, "mis\\CB_FireAlarm.mis", "[WIN_MESSAGE]");
    } else {
        ParseFile(&temp, "mis\\CB_FireAlarm.mis", "[LOSE_MESSAGE]");
    }

    // Copy handler IDs if this is SCI_PracticeRoom (0x2B)
    if (savedCommand == 0x2B) {
        ((int*)field_A8)[2] = savedCommand;
        ((int*)field_A8)[3] = savedMsgData;
    }

    // Enqueue and destroy
    EnqueueSpriteAction((void*)field_A8);
    if (field_A8 != 0) {
        delete (SpriteAction*)field_A8;
        field_A8 = 0;
    }
}

/* Function start: 0x407BB0 */
void SC_FireAlarm::ResetState() {
    field_B0 = 0;
    field_AC = 0;
    if (field_10C != 0) {
        field_10C->ResetAnimation(-1, 0);
    }
    if (field_C8 != 0) {
        field_C8->ResetAnimation(-1, 0);
    }
    if (field_140 == 0) {
        ((Palette*)field_148)->PlaySound(9);
    }
    field_AC |= 8;
}

/* Function start: 0x407C20 */
int SC_FireAlarm::HandleClick(int* param) {
    int* coords = (int*)((char*)param + 0x120);
    int animIdx = *(int*)((char*)param + 0x98);
    int* animBase = *(int**)((char*)param + 0x90);
    int frameCount = *(int*)((char*)animBase + animIdx * 16 + 4);
    frameCount--;

    int hitResult = g_BackBuffer_0046aa14->CheckHit(coords[0], coords[1]);

    if (dim_FC.field_0 <= hitResult && dim_FC.field_4 >= hitResult) {
        int y = coords[1] - 0x4B;
        int x = coords[0] - 0xA1;
        *(int*)(field_F8 + 0xAC) = x;
        *(int*)(field_F8 + 0xB0) = y;
        field_F8->ResetAnimation(2, 0);
        ((Palette*)field_148)->PlaySound(2);
        return 1;
    }

    int pf0 = *(int*)((char*)param + 0xF0);
    if (pf0 == 0) {
        if (frameCount != 0) {
            return 0;
        }
    } else {
        if (*(int*)(*(int*)(pf0 + 0xC) + 0x374) != frameCount) {
            return 0;
        }
    }

    int soundId;

    if (dim_E8.field_0 <= hitResult && dim_E8.field_4 >= hitResult) {
        dim_F0.field_0++;
        dim_138.field_0 += DAT_004685a0;
        int done;
        if (dim_F0.field_4 == 0) {
            done = 0;
        } else if (dim_F0.field_0 >= dim_F0.field_4) {
            done = 1;
        } else {
            done = 0;
        }
        if (done) {
            ((Palette*)field_148)->PlaySound(0xF);
            dim_F0.field_0 = 0;
            goto returnOne;
        }
        soundId = 3;
        goto playSound;
    }

    if (dim_114.field_0 <= hitResult && dim_114.field_4 >= hitResult) {
        soundId = 0xA;
        dim_138.field_0 += DAT_00472bd8;
        goto playSound;
    }

    {
        int inSlot;
        if (slot_CC.left > coords[0] || slot_CC.right < coords[0] ||
            slot_CC.top > coords[1] || slot_CC.bottom < coords[1]) {
            inSlot = 0;
        } else {
            inSlot = 1;
        }

        if (inSlot) {
            field_C8->ResetAnimation(*(int*)(field_C8 + 0x98) + 1, 0);

            {
                int curFrame = *(int*)(field_C8 + 0x98);
                if (curFrame >= 0 && curFrame <= 2) {
                    soundId = curFrame + 4;
                    goto callSound;
                }
                if (curFrame != 4) goto afterSound;
                soundId = 7;
callSound:
                ((Palette*)field_148)->PlaySound(soundId);
afterSound:;
            }

            if (*(int*)(field_C8 + 0x98) == 4) {
                field_AC |= 2;
                goto returnOne;
            }

            if (*(int*)(field_DC + 0x98) == 0) {
                field_DC->ResetAnimation(1, 0);
            }
            goto returnOne;
        }
    }

    soundId = 8;
playSound:
    ((Palette*)field_148)->PlaySound(soundId);
returnOne:
    return 1;
}

/* Function start: 0x407E50 */
void SC_FireAlarm::Render() {
    field_C4->Do(*(int*)(field_C4 + 0xAC), *(int*)(field_C4 + 0xB0), 1.0);
    field_110->Do(*(int*)(field_110 + 0xAC), *(int*)(field_110 + 0xB0), 1.0);
    field_DC->Do(*(int*)(field_DC + 0xAC), *(int*)(field_DC + 0xB0), 1.0);
    if (((Palette*)field_148)->CheckSound(9) == 0) {
        field_B0 = 1;
        field_AC &= ~8;
    }
}

/* Function start: 0x407EE0 */
void SC_FireAlarm::ProcessFrame() {
    Sprite* spr110;
    int iVar4;

    if ((field_AC & 0xF) == 0 && dim_138.field_4 != 0 && dim_138.field_0 >= dim_138.field_4) {
        dim_138.field_0 = 0;
        spr110 = field_110;
        int animRes = *(int*)(spr110 + 0xF0);
        int dataPtr = *(int*)(animRes + 0xC);
        int curFrame = *(int*)(spr110 + 0x98);
        int totalFrames = *(int*)(dataPtr + 0x374);
        int frameBase = *(int*)(spr110 + 0x90);
        int distance = totalFrames - *(int*)(curFrame * 16 + frameBase) + 1;
        field_134 = distance;

        if (distance >= 0x3D && distance <= 0x55) {
            int y = dim_12C.field_4;
            int x = dim_12C.field_0 - 0x19;
            *(int*)(spr110 + 0xAC) = x;
            *(int*)(spr110 + 0xB0) = y;
        } else if (distance >= 0x15 && distance <= 0x32) {
            int y = dim_12C.field_4;
            int x = dim_12C.field_0 + 0x19;
            *(int*)(spr110 + 0xAC) = x;
            *(int*)(spr110 + 0xB0) = y;
        }

        field_DC->ResetAnimation(0, 0);

        if (*(int*)(field_F8 + 0x98) == 0) {
            field_F8->ResetAnimation(-1, 0);
        }

        field_10C->ResetAnimation(*(int*)(field_10C + 0x98) + 1, 0);

        iVar4 = *(int*)(field_10C + 0x98);
        if (iVar4 == 2) {
            field_AC |= 4;
        }

        ((ZBuffer*)DAT_004685ac)->ResetItems();

        field_110->ResetAnimation(iVar4 + 1, 0);

        ((Palette*)field_148)->PlaySound(iVar4 + 0xC);

        field_AC |= 1;
    }

    field_C4->Do(*(int*)(field_C4 + 0xAC), *(int*)(field_C4 + 0xB0), 1.0);

    int renderResult = field_110->Do(*(int*)(field_110 + 0xAC), *(int*)(field_110 + 0xB0), 1.0);

    if ((field_AC & 0xF) == 0) {
        int animData = *(int*)(field_110 + 0xF0);
        int frameCount;
        if (animData != 0) {
            frameCount = *(int*)(*(int*)(animData + 0xC) + 0x374);
        } else {
            frameCount = 0;
        }

        if (frameCount == 0x28) {
            field_F8->ResetAnimation(0, 0);
        } else if (frameCount == 0x50) {
            if (*(int*)(field_DC + 0x98) == 0) {
                field_DC->ResetAnimation(1, 0);
            }
        }
    }

    if (renderResult != 0) {
        int* spr = (int*)field_110;
        int frame = spr[0x98/4];
        if (frame >= 1 && frame <= 3) {
            {
                SlimeDim temp(dim_12C);
                spr[0xAC/4] = temp.field_0;
                spr[0xB0/4] = temp.field_4;
            }

            field_110->ResetAnimation(0, field_134);

            int ac = field_AC & ~1;
            field_AC = ac;
            if (ac & 4) {
                field_140++;
                int done;
                if (field_144 == 0) {
                    done = 0;
                } else if (field_140 >= field_144) {
                    done = 1;
                } else {
                    done = 0;
                }
                if (done) {
                    field_B0 = 3;
                } else {
                    char* name = GetCinematicFilename(0x13A6);
                    char* path = FormatAssetPath(name);
                    if (FileExists(path) != 0) {
                        Animation anim;
                        anim.Play(path, 0);
                    }
                    SC_FireAlarm::ResetState();
                }
            }
        }
    }

    {
        int dcRender = field_DC->Do(*(int*)(field_DC + 0xAC), *(int*)(field_DC + 0xB0), 1.0);
        if (dcRender != 0) {
            int dcFrame = *(int*)(field_DC + 0x98);
            switch (dcFrame) {
            case 1:
                field_DC->ResetAnimation(2, 0);
                {
                    int r = rand();
                    dim_E0.field_0 = 0;
                    dim_E0.field_4 = r % 0x14 + 0x14;
                }
                break;
            case 2:
                {
                    dim_E0.field_0++;
                    int done2;
                    if (dim_E0.field_4 == 0) {
                        done2 = 0;
                    } else if (dim_E0.field_0 >= dim_E0.field_4) {
                        done2 = 1;
                    } else {
                        done2 = 0;
                    }
                    if (!done2) break;
                }
                field_DC->ResetAnimation(3, 0);
                break;
            case 3:
                field_DC->ResetAnimation(0, 0);
                break;
            }
        }
    }

    {
        int f8Render = field_F8->Do(*(int*)(field_F8 + 0xAC), *(int*)(field_F8 + 0xB0), 1.0);
        if (f8Render != 0) {
            int f8Frame = *(int*)(field_F8 + 0x98);
            switch (f8Frame) {
            case 0:
                field_F8->ResetAnimation(1, 0);
                dim_138.field_0 += DAT_00472bdc;
                ((Palette*)field_148)->PlaySound(0xB);
                break;
            case 1:
                field_F8->ResetAnimation(-1, 0);
                break;
            case 2:
                {
                    SlimeDim temp2(dim_104);
                    *(int*)(field_F8 + 0xAC) = temp2.field_0;
                    *(int*)(field_F8 + 0xB0) = temp2.field_4;
                }
                field_F8->ResetAnimation(-1, 0);
                break;
            }
        }
    }

    {
        int c8Render = field_C8->Do(*(int*)(field_C8 + 0xAC), *(int*)(field_C8 + 0xB0), 1.0);
        if (c8Render != 0) {
            int c8Frame = *(int*)(field_C8 + 0x98);
            switch (c8Frame) {
            case 2:
                field_C8->ResetAnimation(3, 0);
                break;
            case 4:
                field_B0 = 2;
                break;
            }
        }
    }

    field_10C->Do(*(int*)(field_10C + 0xAC), *(int*)(field_10C + 0xB0), 1.0);

    if ((field_AC & 0xF) == 0) {
        (g_Mouse_0046aa18)->DrawCursor();
        FUN_00427880((void*)DAT_004685ac);

        int bcRender = field_BC->Do(*(int*)(field_BC + 0xAC), *(int*)(field_BC + 0xB0), 1.0);
        if (bcRender != 0) {
            int mouseVal = 0;
            int* mousePtr = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
            if (mousePtr != 0) {
                mouseVal = *mousePtr;
            }
            field_BC->ResetAnimation(mouseVal / (dim_B4.field_0 / 5), 0);
        }

        if (*(int*)(DAT_004685ac + 0xA8) != 0) {
            int mouseVal2 = 0;
            int* mousePtr2 = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
            if (mousePtr2 != 0) {
                mouseVal2 = *mousePtr2;
            }
            field_BC->ResetAnimation(mouseVal2 / (dim_B4.field_0 / 3) + 5, 0);
        }
    }
}
