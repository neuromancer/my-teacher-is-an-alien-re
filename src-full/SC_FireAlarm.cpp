#include "SC_FireAlarm.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "Animation.h"
#include "Sprite.h"
#include "Palette.h"
#include "ZBuffer.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

// FUN_00413e10 = ParseFile in Parser.h
extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetCinematicFilename(int);
extern "C" char* FormatAssetPath(char*, ...);

extern void __fastcall FUN_00425490(void*);
extern void* __fastcall FUN_00425480(void*);
extern "C" void SetVideoRes(int, int);
extern "C" void ShowError(const char* format, ...);
// FUN_004279a0 = ZBuffer::ResetItems (ZBuffer.h)
extern void __fastcall FUN_00427880(void*);
#include "MouseControl.h"

// Thiscall wrapper classes for sprite/palette methods



class DetMask {
public:
    int CheckHit(int x, int y);  // 0x411330
};

class RenderObj {
public:
    void Render();               // 0x432DA0
};
/* Function start: 0x411330 */
int DetMask::CheckHit(int x, int y) {
    // DetMask IS a VBuffer — uses VBuffer field layout
    int* self = (int*)this;
    int w = self[5];        // width at +0x14
    int h = self[6];        // height at +0x18
    int cx1 = self[10];     // clip_x1 at +0x28
    int cx2 = self[11];     // clip_x2 at +0x2C
    int cy1 = self[8];      // clip_y1 at +0x20
    int cy2 = self[9];      // clip_y2 at +0x24

    // Pixel offset: (height - 1 - y) * width + x
    int offset = (h - 1 - y) * w + x;

    // Bounds check
    if (cx1 > x || cx2 < x || cy1 > y || cy2 < y) {
        return -1;
    }

    // Read pixel from data buffer
    ((VBuffer*)this)->Lock();
    unsigned char pixel = ((unsigned char*)((VBuffer*)this)->data)[offset];
    return (int)pixel;
}
void RenderObj::Render() {}

extern int DAT_004685ac;
class InputManager;
extern InputManager* DAT_0046aa08;
extern "C" extern void* DAT_0046aa14;
class MouseControl;
extern MouseControl* DAT_0046aa18;
extern int DAT_004685a0;
extern int DAT_00472bd8;
extern int DAT_00472bdc;

/* Function start: 0x407290 */
SC_FireAlarm::SC_FireAlarm() {
    memset(&field_A8, 0, 0xA8);
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

    void* pal = (void*)malloc(0xC);
    void* palObj = 0;
    if (pal != 0) {
        palObj = FUN_00425480(pal);
    }
    field_148 = (int)palObj;

    ParseFile(this, "mis\\CB_FireAlarm.mis", (char*)0);

    if (field_A8 == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = (int)sprite;
    }
}

/* Function start: 0x4076E0 */
int SC_FireAlarm::ShutDown(SC_Message* msg) {
    void* p;

    p = (void*)field_C0;
    if (p != 0) {
        ((Palette*)p)->~Palette();
        FreeMemory(p);
        field_C0 = 0;
    }

    p = (void*)field_C4;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
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
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_BC = 0;
    }

    p = (void*)field_F8;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_F8 = 0;
    }

    p = (void*)field_DC;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_DC = 0;
    }

    p = (void*)field_110;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_110 = 0;
    }

    p = (void*)field_10C;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_10C = 0;
    }

    p = (void*)field_C8;
    if (p != 0) {
        ((Sprite*)p)->~Sprite();
        FreeMemory(p);
        field_C8 = 0;
    }

    p = (void*)field_A8;
    if (p != 0) {
        ((SpriteAction*)p)->~SpriteAction();
        FreeMemory(p);
        field_A8 = 0;
    }

    p = (void*)field_148;
    if (p != 0) {
        FUN_00425490(p);
        FreeMemory(p);
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
        ((SpriteAction*)field_A8)->~SpriteAction();
        FreeMemory((void*)field_A8);
        field_A8 = 0;
    }

    // Create new SpriteAction
    SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    field_A8 = (int)action;

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
        ((SpriteAction*)field_A8)->~SpriteAction();
        FreeMemory((void*)field_A8);
        field_A8 = 0;
    }
}

/* Function start: 0x407BB0 */
void SC_FireAlarm::ResetState() {
    field_B0 = 0;
    field_AC = 0;
    if (field_10C != 0) {
        ((Sprite*)field_10C)->ResetAnimation(-1, 0);
    }
    if (field_C8 != 0) {
        ((Sprite*)field_C8)->ResetAnimation(-1, 0);
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

    int hitResult = ((DetMask*)DAT_0046aa14)->CheckHit(coords[0], coords[1]);

    if (dim_FC.field_0 <= hitResult && dim_FC.field_4 >= hitResult) {
        int y = coords[1] - 0x4B;
        int x = coords[0] - 0xA1;
        *(int*)(field_F8 + 0xAC) = x;
        *(int*)(field_F8 + 0xB0) = y;
        ((Sprite*)field_F8)->ResetAnimation(2, 0);
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
        field_F0++;
        field_138 += DAT_004685a0;
        int done;
        if (field_F4 == 0) {
            done = 0;
        } else if (field_F0 >= field_F4) {
            done = 1;
        } else {
            done = 0;
        }
        if (done) {
            ((Palette*)field_148)->PlaySound(0xF);
            field_F0 = 0;
            goto returnOne;
        }
        soundId = 3;
        goto playSound;
    }

    if (dim_114.field_0 <= hitResult && dim_114.field_4 >= hitResult) {
        soundId = 0xA;
        field_138 += DAT_00472bd8;
        goto playSound;
    }

    {
        int inSlot;
        if (slot_CC.field_0 > coords[0] || slot_CC.field_8 < coords[0] ||
            slot_CC.field_4 > coords[1] || slot_CC.field_C < coords[1]) {
            inSlot = 0;
        } else {
            inSlot = 1;
        }

        if (inSlot) {
            ((Sprite*)field_C8)->ResetAnimation(*(int*)(field_C8 + 0x98) + 1, 0);

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
                ((Sprite*)field_DC)->ResetAnimation(1, 0);
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
    ((Sprite*)field_C4)->RenderAt(*(int*)(field_C4 + 0xAC), *(int*)(field_C4 + 0xB0), 0, 0x3ff00000);
    ((Sprite*)field_110)->RenderAt(*(int*)(field_110 + 0xAC), *(int*)(field_110 + 0xB0), 0, 0x3ff00000);
    ((Sprite*)field_DC)->RenderAt(*(int*)(field_DC + 0xAC), *(int*)(field_DC + 0xB0), 0, 0x3ff00000);
    if (((Palette*)field_148)->CheckSound(9) == 0) {
        field_B0 = 1;
        field_AC &= ~8;
    }
}

/* Function start: 0x407EE0 */
void SC_FireAlarm::ProcessFrame() {
    int spr110;
    int iVar4;

    if ((field_AC & 0xF) == 0 && field_13C != 0 && field_138 >= field_13C) {
        field_138 = 0;
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

        ((Sprite*)field_DC)->ResetAnimation(0, 0);

        if (*(int*)(field_F8 + 0x98) == 0) {
            ((Sprite*)field_F8)->ResetAnimation(-1, 0);
        }

        ((Sprite*)field_10C)->ResetAnimation(*(int*)(field_10C + 0x98) + 1, 0);

        iVar4 = *(int*)(field_10C + 0x98);
        if (iVar4 == 2) {
            field_AC |= 4;
        }

        ((ZBuffer*)DAT_004685ac)->ResetItems();

        ((Sprite*)field_110)->ResetAnimation(iVar4 + 1, 0);

        ((Palette*)field_148)->PlaySound(iVar4 + 0xC);

        field_AC |= 1;
    }

    ((Sprite*)field_C4)->RenderAt(*(int*)(field_C4 + 0xAC), *(int*)(field_C4 + 0xB0), 0, 0x3ff00000);

    int renderResult = ((Sprite*)field_110)->RenderAt(*(int*)(field_110 + 0xAC), *(int*)(field_110 + 0xB0), 0, 0x3ff00000);

    if ((field_AC & 0xF) == 0) {
        int animData = *(int*)(field_110 + 0xF0);
        int frameCount;
        if (animData != 0) {
            frameCount = *(int*)(*(int*)(animData + 0xC) + 0x374);
        } else {
            frameCount = 0;
        }

        if (frameCount == 0x28) {
            ((Sprite*)field_F8)->ResetAnimation(0, 0);
        } else if (frameCount == 0x50) {
            if (*(int*)(field_DC + 0x98) == 0) {
                ((Sprite*)field_DC)->ResetAnimation(1, 0);
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

            ((Sprite*)field_110)->ResetAnimation(0, field_134);

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
        int dcRender = ((Sprite*)field_DC)->RenderAt(*(int*)(field_DC + 0xAC), *(int*)(field_DC + 0xB0), 0, 0x3ff00000);
        if (dcRender != 0) {
            int dcFrame = *(int*)(field_DC + 0x98);
            switch (dcFrame) {
            case 1:
                ((Sprite*)field_DC)->ResetAnimation(2, 0);
                {
                    int r = rand();
                    field_E0 = 0;
                    field_E4 = r % 0x14 + 0x14;
                }
                break;
            case 2:
                {
                    field_E0++;
                    int done2;
                    if (field_E4 == 0) {
                        done2 = 0;
                    } else if (field_E0 >= field_E4) {
                        done2 = 1;
                    } else {
                        done2 = 0;
                    }
                    if (!done2) break;
                }
                ((Sprite*)field_DC)->ResetAnimation(3, 0);
                break;
            case 3:
                ((Sprite*)field_DC)->ResetAnimation(0, 0);
                break;
            }
        }
    }

    {
        int f8Render = ((Sprite*)field_F8)->RenderAt(*(int*)(field_F8 + 0xAC), *(int*)(field_F8 + 0xB0), 0, 0x3ff00000);
        if (f8Render != 0) {
            int f8Frame = *(int*)(field_F8 + 0x98);
            switch (f8Frame) {
            case 0:
                ((Sprite*)field_F8)->ResetAnimation(1, 0);
                field_138 += DAT_00472bdc;
                ((Palette*)field_148)->PlaySound(0xB);
                break;
            case 1:
                ((Sprite*)field_F8)->ResetAnimation(-1, 0);
                break;
            case 2:
                {
                    SlimeDim temp2(dim_104);
                    *(int*)(field_F8 + 0xAC) = temp2.field_0;
                    *(int*)(field_F8 + 0xB0) = temp2.field_4;
                }
                ((Sprite*)field_F8)->ResetAnimation(-1, 0);
                break;
            }
        }
    }

    {
        int c8Render = ((Sprite*)field_C8)->RenderAt(*(int*)(field_C8 + 0xAC), *(int*)(field_C8 + 0xB0), 0, 0x3ff00000);
        if (c8Render != 0) {
            int c8Frame = *(int*)(field_C8 + 0x98);
            switch (c8Frame) {
            case 2:
                ((Sprite*)field_C8)->ResetAnimation(3, 0);
                break;
            case 4:
                field_B0 = 2;
                break;
            }
        }
    }

    ((Sprite*)field_10C)->RenderAt(*(int*)(field_10C + 0xAC), *(int*)(field_10C + 0xB0), 0, 0x3ff00000);

    if ((field_AC & 0xF) == 0) {
        (DAT_0046aa18)->DrawCursor();
        FUN_00427880((void*)DAT_004685ac);

        int bcRender = ((Sprite*)field_BC)->RenderAt(*(int*)(field_BC + 0xAC), *(int*)(field_BC + 0xB0), 0, 0x3ff00000);
        if (bcRender != 0) {
            int mouseVal = 0;
            int* mousePtr = *(int**)((char*)DAT_0046aa08 + 0x1A0);
            if (mousePtr != 0) {
                mouseVal = *mousePtr;
            }
            ((Sprite*)field_BC)->ResetAnimation(mouseVal / (dim_B4.field_0 / 5), 0);
        }

        if (*(int*)(DAT_004685ac + 0xA8) != 0) {
            int mouseVal2 = 0;
            int* mousePtr2 = *(int**)((char*)DAT_0046aa08 + 0x1A0);
            if (mousePtr2 != 0) {
                mouseVal2 = *mousePtr2;
            }
            ((Sprite*)field_BC)->ResetAnimation(mouseVal2 / (dim_B4.field_0 / 3) + 5, 0);
        }
    }
}
