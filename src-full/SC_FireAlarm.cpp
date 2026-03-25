#include "SC_FireAlarm.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "Animation.h"
#include "Sprite.h"
#include "Palette.h"
#include "ZBuffer.h"
#include "SlimeTable.h"
#include "RockThrower.h"
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
// FUN_00427880 = Weapon::UpdateProjectiles — callers updated
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
    memset(&spriteAction, 0, 0x2A * 4);
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

    screenSize.x = 0x140;
    screenSize.y = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = pmsg[1];

    soundTable = (int)new SlimeTable();

    ParseFile(this, "mis\\CB_FireAlarm.mis", (char*)0);

    if (spriteAction == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        spriteAction = sprite;
    }
}

/* Function start: 0x4076E0 */
int SC_FireAlarm::ShutDown(SC_Message* msg) {
    void* p;

    p = (void*)paletteDummy;
    if (p != 0) {
        delete (Palette*)p;
        paletteDummy = 0;
    }

    p = (void*)bgSprite;
    if (p != 0) {
        delete (Sprite*)p;
        bgSprite = 0;
    }

    if (DAT_004685ac != 0) {
        int* obj = (int*)DAT_004685ac;
        int* vtbl = (int*)*obj;
        ((void (__fastcall*)(int*, int, int))vtbl[3])(obj, 0, 1);
        DAT_004685ac = 0;
    }

    p = (void*)consoleSprite;
    if (p != 0) {
        delete (Sprite*)p;
        consoleSprite = 0;
    }

    p = (void*)planeSprite;
    if (p != 0) {
        delete (Sprite*)p;
        planeSprite = 0;
    }

    p = (void*)handSprite;
    if (p != 0) {
        delete (Sprite*)p;
        handSprite = 0;
    }

    p = (void*)teacherSprite;
    if (p != 0) {
        delete (Sprite*)p;
        teacherSprite = 0;
    }

    p = (void*)caughtsSprite;
    if (p != 0) {
        delete (Sprite*)p;
        caughtsSprite = 0;
    }

    p = (void*)alarmSprite;
    if (p != 0) {
        delete (Sprite*)p;
        alarmSprite = 0;
    }

    p = (void*)spriteAction;
    if (p != 0) {
        delete (SpriteAction*)p;
        spriteAction = 0;
    }

    p = (void*)soundTable;
    if (p != 0) {
        delete (SlimeTable*)p;
        soundTable = 0;
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
        gamePhase = 4;
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
    // Destroy existing action at spriteAction
    if (spriteAction != 0) {
        delete (SpriteAction*)spriteAction;
        spriteAction = 0;
    }

    // Create new SpriteAction
    SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    spriteAction = action;

    // Parse win or lose message from CB_FireAlarm.mis
    Parser temp;
    if (stateFlags & 2) {
        ParseFile(&temp, "mis\\CB_FireAlarm.mis", "[WIN_MESSAGE]");
    } else {
        ParseFile(&temp, "mis\\CB_FireAlarm.mis", "[LOSE_MESSAGE]");
    }

    // Copy handler IDs if this is SCI_PracticeRoom (0x2B)
    if (savedCommand == 0x2B) {
        ((int*)spriteAction)[2] = savedCommand;
        ((int*)spriteAction)[3] = savedMsgData;
    }

    // Enqueue and destroy
    EnqueueSpriteAction((void*)spriteAction);
    if (spriteAction != 0) {
        delete (SpriteAction*)spriteAction;
        spriteAction = 0;
    }
}

/* Function start: 0x407BB0 */
void SC_FireAlarm::ResetState() {
    gamePhase = 0;
    stateFlags = 0;
    if (caughtsSprite != 0) {
        caughtsSprite->ResetAnimation(-1, 0);
    }
    if (alarmSprite != 0) {
        alarmSprite->ResetAnimation(-1, 0);
    }
    if (roundCount == 0) {
        ((Palette*)soundTable)->PlaySound(9);
    }
    stateFlags |= 8;
}

/* Function start: 0x407C20 */
int SC_FireAlarm::HandleClick(int* param) {
    int* coords = (int*)((char*)param + 0x120);
    int animIdx = *(int*)((char*)param + 0x98);
    int* animBase = *(int**)((char*)param + 0x90);
    int frameCount = *(int*)((char*)animBase + animIdx * 16 + 4);
    frameCount--;

    int hitResult = g_BackBuffer_0046aa14->CheckHit(coords[0], coords[1]);

    if (planeClickRange.x <= hitResult && planeClickRange.y >= hitResult) {
        int y = coords[1] - 0x4B;
        int x = coords[0] - 0xA1;
        planeSprite->loc_x = x;
        planeSprite->loc_y = y;
        planeSprite->ResetAnimation(2, 0);
        ((Palette*)soundTable)->PlaySound(2);
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

    if (waterHitRange.x <= hitResult && waterHitRange.y >= hitResult) {
        waterHitCounter.x++;
        timerCounter.x += DAT_004685a0;
        int done;
        if (waterHitCounter.y == 0) {
            done = 0;
        } else if (waterHitCounter.x >= waterHitCounter.y) {
            done = 1;
        } else {
            done = 0;
        }
        if (done) {
            ((Palette*)soundTable)->PlaySound(0xF);
            waterHitCounter.x = 0;
            goto returnOne;
        }
        soundId = 3;
        goto playSound;
    }

    if (bellHitRange.x <= hitResult && bellHitRange.y >= hitResult) {
        soundId = 0xA;
        timerCounter.x += DAT_00472bd8;
        goto playSound;
    }

    {
        int inSlot;
        if (alarmSlotRect.left > coords[0] || alarmSlotRect.right < coords[0] ||
            alarmSlotRect.top > coords[1] || alarmSlotRect.bottom < coords[1]) {
            inSlot = 0;
        } else {
            inSlot = 1;
        }

        if (inSlot) {
            alarmSprite->ResetAnimation(alarmSprite->handle + 1, 0);

            {
                int curFrame = alarmSprite->handle;
                if (curFrame >= 0 && curFrame <= 2) {
                    soundId = curFrame + 4;
                    goto callSound;
                }
                if (curFrame != 4) goto afterSound;
                soundId = 7;
callSound:
                ((Palette*)soundTable)->PlaySound(soundId);
afterSound:;
            }

            if (alarmSprite->handle == 4) {
                stateFlags |= 2;
                goto returnOne;
            }

            if (handSprite->handle == 0) {
                handSprite->ResetAnimation(1, 0);
            }
            goto returnOne;
        }
    }

    soundId = 8;
playSound:
    ((Palette*)soundTable)->PlaySound(soundId);
returnOne:
    return 1;
}

/* Function start: 0x407E50 */
void SC_FireAlarm::Render() {
    bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0);
    teacherSprite->Do(teacherSprite->loc_x, teacherSprite->loc_y, 1.0);
    handSprite->Do(handSprite->loc_x, handSprite->loc_y, 1.0);
    if (((Palette*)soundTable)->CheckSound(9) == 0) {
        gamePhase = 1;
        stateFlags &= ~8;
    }
}

/* Function start: 0x407EE0 */
void SC_FireAlarm::ProcessFrame() {
    Sprite* spr110;
    int iVar4;

    if ((stateFlags & 0xF) == 0 && timerCounter.y != 0 && timerCounter.x >= timerCounter.y) {
        timerCounter.x = 0;
        spr110 = teacherSprite;
        int animRes = (int)spr110->animation_data;
        int dataPtr = *(int*)(animRes + 0xC);
        int curFrame = spr110->handle;
        int totalFrames = *(int*)(dataPtr + 0x374);
        int frameBase = (int)spr110->ranges;
        int distance = totalFrames - *(int*)(curFrame * 16 + frameBase) + 1;
        animDistance = distance;

        if (distance >= 0x3D && distance <= 0x55) {
            int y = teacherHomePos.y;
            int x = teacherHomePos.x - 0x19;
            spr110->loc_x = x;
            spr110->loc_y = y;
        } else if (distance >= 0x15 && distance <= 0x32) {
            int y = teacherHomePos.y;
            int x = teacherHomePos.x + 0x19;
            spr110->loc_x = x;
            spr110->loc_y = y;
        }

        handSprite->ResetAnimation(0, 0);

        if (planeSprite->handle == 0) {
            planeSprite->ResetAnimation(-1, 0);
        }

        caughtsSprite->ResetAnimation(caughtsSprite->handle + 1, 0);

        iVar4 = caughtsSprite->handle;
        if (iVar4 == 2) {
            stateFlags |= 4;
        }

        ((ZBuffer*)DAT_004685ac)->ResetItems();

        teacherSprite->ResetAnimation(iVar4 + 1, 0);

        ((Palette*)soundTable)->PlaySound(iVar4 + 0xC);

        stateFlags |= 1;
    }

    bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0);

    int renderResult = teacherSprite->Do(teacherSprite->loc_x, teacherSprite->loc_y, 1.0);

    if ((stateFlags & 0xF) == 0) {
        int animData = (int)teacherSprite->animation_data;
        int frameCount;
        if (animData != 0) {
            frameCount = *(int*)(*(int*)(animData + 0xC) + 0x374);
        } else {
            frameCount = 0;
        }

        if (frameCount == 0x28) {
            planeSprite->ResetAnimation(0, 0);
        } else if (frameCount == 0x50) {
            if (handSprite->handle == 0) {
                handSprite->ResetAnimation(1, 0);
            }
        }
    }

    if (renderResult != 0) {
        int* spr = (int*)teacherSprite;
        int frame = spr[0x98/4];
        if (frame >= 1 && frame <= 3) {
            {
                SlimeDim temp(teacherHomePos);
                spr[0xAC/4] = temp.x;
                spr[0xB0/4] = temp.y;
            }

            teacherSprite->ResetAnimation(0, animDistance);

            int ac = stateFlags & ~1;
            stateFlags = ac;
            if (ac & 4) {
                roundCount++;
                int done;
                if (maxRounds == 0) {
                    done = 0;
                } else if (roundCount >= maxRounds) {
                    done = 1;
                } else {
                    done = 0;
                }
                if (done) {
                    gamePhase = 3;
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
        int dcRender = handSprite->Do(handSprite->loc_x, handSprite->loc_y, 1.0);
        if (dcRender != 0) {
            int dcFrame = handSprite->handle;
            switch (dcFrame) {
            case 1:
                handSprite->ResetAnimation(2, 0);
                {
                    int r = rand();
                    handIdleDelay.x = 0;
                    handIdleDelay.y = r % 0x14 + 0x14;
                }
                break;
            case 2:
                {
                    handIdleDelay.x++;
                    int done2;
                    if (handIdleDelay.y == 0) {
                        done2 = 0;
                    } else if (handIdleDelay.x >= handIdleDelay.y) {
                        done2 = 1;
                    } else {
                        done2 = 0;
                    }
                    if (!done2) break;
                }
                handSprite->ResetAnimation(3, 0);
                break;
            case 3:
                handSprite->ResetAnimation(0, 0);
                break;
            }
        }
    }

    {
        int f8Render = planeSprite->Do(planeSprite->loc_x, planeSprite->loc_y, 1.0);
        if (f8Render != 0) {
            int f8Frame = planeSprite->handle;
            switch (f8Frame) {
            case 0:
                planeSprite->ResetAnimation(1, 0);
                timerCounter.x += DAT_00472bdc;
                ((Palette*)soundTable)->PlaySound(0xB);
                break;
            case 1:
                planeSprite->ResetAnimation(-1, 0);
                break;
            case 2:
                {
                    SlimeDim temp2(planeHomePos);
                    planeSprite->loc_x = temp2.x;
                    planeSprite->loc_y = temp2.y;
                }
                planeSprite->ResetAnimation(-1, 0);
                break;
            }
        }
    }

    {
        int c8Render = alarmSprite->Do(alarmSprite->loc_x, alarmSprite->loc_y, 1.0);
        if (c8Render != 0) {
            int c8Frame = alarmSprite->handle;
            switch (c8Frame) {
            case 2:
                alarmSprite->ResetAnimation(3, 0);
                break;
            case 4:
                gamePhase = 2;
                break;
            }
        }
    }

    caughtsSprite->Do(caughtsSprite->loc_x, caughtsSprite->loc_y, 1.0);

    if ((stateFlags & 0xF) == 0) {
        (g_Mouse_0046aa18)->DrawCursor();
        ((Weapon*)DAT_004685ac)->UpdateProjectiles();

        int bcRender = consoleSprite->Do(consoleSprite->loc_x, consoleSprite->loc_y, 1.0);
        if (bcRender != 0) {
            int mouseVal = 0;
            int* mousePtr = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
            if (mousePtr != 0) {
                mouseVal = *mousePtr;
            }
            consoleSprite->ResetAnimation(mouseVal / (screenSize.x / 5), 0);
        }

        if (*(int*)(DAT_004685ac + 0xA8) != 0) {
            int mouseVal2 = 0;
            int* mousePtr2 = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
            if (mousePtr2 != 0) {
                mouseVal2 = *mousePtr2;
            }
            consoleSprite->ResetAnimation(mouseVal2 / (screenSize.x / 3) + 5, 0);
        }
    }
}
