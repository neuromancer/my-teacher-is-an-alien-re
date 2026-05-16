#include "SC_FireAlarm.h"
#include "stubs.h"
#include "InputManager.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "Animation.h"
#include "Sprite.h"
#include "Palette.h"
#include "ZBuffer.h"
#include "SlimeTable.h"
#include "SoundList.h"
#include "RockThrower.h"
#include "GameEngine.h"
#include "SC_Question.h"
#include "Projectile.h"
#include "ZBufferManager.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// FUN_00413e10 = ParseFile in Parser.h
extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetCinematicFilename(int);
extern "C" char* FormatAssetPath(char*, ...);


extern "C" void SetVideoRes(int, int);
extern "C" void ShowError(const char* format, ...);
extern "C" void WriteToMessageLog(const char* msg, ...);
// FUN_004279a0 = ZBuffer::ResetItems (ZBuffer.h)
// FUN_00427880 = Weapon::UpdateProjectiles — callers updated
#include "MouseControl.h"

// Thiscall wrapper classes for sprite/palette methods




#include "globals.h"

// FireAlarmRockThrower — derived RockThrower with vtable 0x461118
// Overrides CheckTargetHit and OnHit to redirect to active handler (SC_FireAlarm)
// Functions at 0x408E40, 0x408E60; sdtor at 0x408E70
class FireAlarmRockThrower : public RockThrower {
public:
    FireAlarmRockThrower(Parser* parent) : RockThrower(parent) {}
    ~FireAlarmRockThrower();
    void OnHit();
    int CheckTargetHit(int param);
};

/* Function start: 0x407290 */
SC_FireAlarm::SC_FireAlarm() {
    memset(&spriteAction, 0, 0x2A * 4);
    handlerId = 0x40;
}

/* Function start: 0x407470 */
SC_FireAlarm::~SC_FireAlarm() {
    SC_FireAlarm::ShutDown((SC_MessageParser*)0);
}

/* Function start: 0x407590 */
void SC_FireAlarm::Init(SC_MessageParser* msg) {
    int* pmsg = (int*)msg;

    Handler::Init(msg);

    if (FileExists("CB_FireAlarm") == 0) {
        ShowLoadingScreen();
    }

    screenSize.x = 0x140;
    screenSize.y = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = pmsg[1];

    soundList = new SlimeTable();

    ParseFile(this, "mis\\CB_FireAlarm.mis", (char*)0);

    if (spriteAction == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        spriteAction = sprite;
    }
}

/* Function start: 0x4076E0 */
int SC_FireAlarm::ShutDown(SC_MessageParser* msg) {
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

    if (g_FireAlarmEngine_004685ac != 0) {
        int* obj = (int*)g_FireAlarmEngine_004685ac;
        int* vtbl = (int*)*obj;
        ((void (__fastcall*)(int*, int, int))vtbl[3])(obj, 0, 1);
        g_FireAlarmEngine_004685ac = 0;
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

    p = (void*)soundList;
    if (p != 0) {
        delete (SlimeTable*)(void*)soundList;
        soundList = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x407930 */
int SC_FireAlarm::AddMessage(SC_MessageParser* msg) {
    int* pmsg = (int*)msg;

    WriteMessageAddress(msg);

    if (pmsg[11] == 0x1b && savedCommand == 0x2b) {
        gamePhase = 4;
    }

    return 1;
}

/* Function start: 0x407970 */
int SC_FireAlarm::Exit(SC_MessageParser* msg) {
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

/* Function start: 0x4078A0 */
void SC_FireAlarm::Update(int p1, int p2) {
    if (handlerId == p2) {
        if (g_FireAlarmFlag_004685a8 != 0) {
            int result = WaitForInput() - 0x54;
            g_FireAlarmFlag_004685a8 = (result != 0);
        }
        switch (gamePhase) {
        case 0:
            Render();
            return;
        case 2:
            SendResultMessage();
            return;
        case 3:
            SendResultMessage();
            return;
        case 4:
            SendResultMessage();
            return;
        default:
            ProcessFrame();
            return;
        }
    }
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
    SC_MessageParser temp((int)spriteAction);
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
        soundList->Play(9);
    }
    stateFlags |= 8;
}

/* Function start: 0x407C10 */
void SC_FireAlarm::ProcessClick() {
    soundList->Play(1);
}

/* Function start: 0x407C20 */
int SC_FireAlarm::HandleClick(int* param) {
    Projectile* p = (Projectile*)param;
    int animIdx = p->handle;
    int frameCount = p->ranges[animIdx].dim.y;
    frameCount--;

    int hitResult = g_BackBuffer_0046aa14->CheckHit(p->nextPos.x, p->nextPos.y);

    if (planeClickRange.x <= hitResult && planeClickRange.y >= hitResult) {
        int y = p->nextPos.y - 0x4B;
        int x = p->nextPos.x - 0xA1;
        planeSprite->loc_x = x;
        planeSprite->loc_y = y;
        planeSprite->ResetAnimation(2, 0);
        soundList->Play(2);
        return 1;
    }

    Animation* anim = p->animation_data;
    if (anim == 0) {
        if (frameCount != 0) {
            return 0;
        }
    } else {
        if (*(int*)((char*)anim->smk + 0x374) != frameCount) {
            return 0;
        }
    }

    int soundId;

    if (waterHitRange.x <= hitResult && waterHitRange.y >= hitResult) {
        waterHitCounter.x++;
        timerCounter.x += g_FireAlarmTimer_004685a0;
        int done;
        if (waterHitCounter.y != 0 && waterHitCounter.x >= waterHitCounter.y) {
            done = 1;
        } else {
            done = 0;
        }
        if (done) {
            soundList->Play(0xF);
            waterHitCounter.x = 0;
            goto returnOne;
        }
        soundId = 3;
        goto playSound;
    }

    if (bellHitRange.x <= hitResult && bellHitRange.y >= hitResult) {
        soundId = 0xA;
        timerCounter.x += g_FireAlarmField1_00472bd8;
        goto playSound;
    }

    {
        int inSlot;
        if (alarmSlotRect.left > p->nextPos.x || alarmSlotRect.right < p->nextPos.x ||
            alarmSlotRect.top > p->nextPos.y || alarmSlotRect.bottom < p->nextPos.y) {
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
                soundList->Play(soundId);
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
    soundList->Play(soundId);
returnOne:
    return 1;
}

/* Function start: 0x407E50 */
void SC_FireAlarm::Render() {
    bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0);
    teacherSprite->Do(teacherSprite->loc_x, teacherSprite->loc_y, 1.0);
    handSprite->Do(handSprite->loc_x, handSprite->loc_y, 1.0);
    if (soundList->IsSamplePlaying(9) == 0) {
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
        int totalFrames = *(int*)((char*)spr110->animation_data->smk + 0x374);
        int distance = totalFrames - spr110->ranges[spr110->handle].dim.x + 1;
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

        ((RockThrower*)g_FireAlarmEngine_004685ac)->ResetProjectiles();

        teacherSprite->ResetAnimation(iVar4 + 1, 0);

        soundList->Play(iVar4 + 0xC);

        stateFlags |= 1;
    }

    bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0);

    int renderResult = teacherSprite->Do(teacherSprite->loc_x, teacherSprite->loc_y, 1.0);

    if ((stateFlags & 0xF) == 0) {
        Animation* animData = teacherSprite->animation_data;
        int frameCount;
        if (animData != 0) {
            frameCount = *(int*)((char*)animData->smk + 0x374);
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
                if (maxRounds != 0 && roundCount >= maxRounds) {
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
                    if (handIdleDelay.y != 0 && handIdleDelay.x >= handIdleDelay.y) {
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
                timerCounter.x += g_FireAlarmField2_00472bdc;
                soundList->Play(0xB);
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
        Weapon* engine = (Weapon*)g_FireAlarmEngine_004685ac;
        engine->UpdateProjectiles();

        int bcRender = consoleSprite->Do(consoleSprite->loc_x, consoleSprite->loc_y, 1.0);
        if (bcRender != 0) {
            int mouseVal = 0;
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseVal = g_InputManager_0046aa08->pMouse->x;
            }
            consoleSprite->ResetAnimation(mouseVal / (screenSize.x / 5), 0);
        }

        if (engine->m_clicked != 0) {
            int mouseVal2 = 0;
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseVal2 = g_InputManager_0046aa08->pMouse->x;
            }
            consoleSprite->ResetAnimation(mouseVal2 / (screenSize.x / 3) + 5, 0);
        }
    }
}

/* Function start: 0x408530 */
void SC_FireAlarm::OnProcessEnd() {
    if (paletteDummy != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = (Palette*)paletteDummy;
    }

    bellHitRange.x = 0xC2;
    bellHitRange.y = 0xCF;
    waterHitRange.x = 0xB4;
    waterHitRange.y = 0xC1;
    planeClickRange.x = 0xD2;
    alarmSlotRect.top = 0x5B;
    alarmSlotRect.right = 0xEF;
    alarmSlotRect.bottom = 0x6E;
    bellSlotRect.left = 0x7B;
    bellSlotRect.top = 0x28;
    bellSlotRect.right = 0xEE;
    bellSlotRect.bottom = 0xAB;
    planeClickRange.y = 0xDF;
    alarmSlotRect.left = 0xDF;
    timerCounter.y = g_FireAlarmTimerTarget_0046859c;
    timerCounter.x = 0;
    roundCount = 0;
    waterHitCounter.x = 0;
    maxRounds = 7;
    waterHitCounter.y = 2;

    if (g_InputManager_0046aa08 != 0) {
        g_InputManager_0046aa08->Refresh(1);
    }

    if (consoleSprite != 0) {
        int mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        }
        consoleSprite->ResetAnimation(mouseX / (screenSize.x / 5), 0);
    }

    SC_FireAlarm::ResetState();
    SendGameMessage(5, bgSoundId, handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
}

/* Function start: 0x4086B0 */
int SC_FireAlarm::LBLParse(char* line) {
    int tempInt = 0;
    int tempInt2 = 0;
    char buffer[128];
    char label[32];
    buffer[0] = 0;
    label[0] = 0;

    sscanf(line, "%s", label);

    if (strcmp(label, "PALETTE") == 0) {
        sscanf(line, "%s %s", label, buffer);
        Palette* pal = new Palette();
        paletteDummy = (int)pal;
        pal->Load(buffer);
    } else if (strcmp(label, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite((char*)0);
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(label, "CONSOLE_SPRITE") == 0) {
        consoleSprite = new Sprite((char*)0);
        Parser::ProcessFile(consoleSprite, this, (char*)0);
    } else if (strcmp(label, "TEACHER_SPRITE") == 0) {
        teacherSprite = new Sprite((char*)0);
        Parser::ProcessFile(teacherSprite, this, (char*)0);
        teacherHomePos = *(SlimeDim*)&teacherSprite->loc_x;
    } else if (strcmp(label, "PLANE_SPRITE") == 0) {
        planeSprite = new Sprite((char*)0);
        Parser::ProcessFile(planeSprite, this, (char*)0);
        planeHomePos = *(SlimeDim*)&planeSprite->loc_x;
    } else if (strcmp(label, "HAND_SPRITE") == 0) {
        handSprite = new Sprite((char*)0);
        Parser::ProcessFile(handSprite, this, (char*)0);
    } else if (strcmp(label, "ALARM_SPRITE") == 0) {
        alarmSprite = new Sprite((char*)0);
        Parser::ProcessFile(alarmSprite, this, (char*)0);
    } else if (strcmp(label, "CAUGHTS_SPRITE") == 0) {
        caughtsSprite = new Sprite((char*)0);
        Parser::ProcessFile(caughtsSprite, this, (char*)0);
    } else if (strcmp(label, "STUDENT_HITS_ALLOWED") == 0) {
    } else if (strcmp(label, "BG_SOUND") == 0) {
        sscanf(line, " %s %d ", label, &bgSoundId);
    } else if (strcmp(label, "MAX_SOUNDS") == 0) {
        sscanf(line, " %s %d ", label, &tempInt);
        soundList->Allocate(tempInt);
    } else if (strcmp(label, "SOUND") == 0) {
        sscanf(line, " %s %d %s %d ", label, &tempInt, buffer, &tempInt2);
        if (tempInt < 0 || soundList->fields[0] - 1 < tempInt) {
            ReportUnknownLabel("SC_FireAlarm");
        } else {
            soundList->LoadEntry(tempInt, buffer, tempInt2);
        }
    } else if (strcmp(label, "WEAPON") == 0) {
        if (sscanf(line, " %s %s ", label, buffer) == 2) {
            if (strcmp(buffer, "ROCKTHROWER2") == 0) {
                RockThrower* rt = new FireAlarmRockThrower(this);
                g_FireAlarmEngine_004685ac = (int)rt;
            } else {
                ReportUnknownLabel("SC_FireAlarm");
            }
        }
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_FireAlarm");
    }
    return 0;
}

// =========================================================================
// FireAlarmRockThrower overrides (vtable 0x461118)
// =========================================================================

/* Function start: 0x408E40 */
int FireAlarmRockThrower::CheckTargetHit(int param) {
    return ((SC_FireAlarm*)g_GameEngine_0046a6ec->m_activeHandler)->HandleClick((int*)param);
}

/* Function start: 0x408E60 */
void FireAlarmRockThrower::OnHit() {
    ((SC_FireAlarm*)g_GameEngine_0046a6ec->m_activeHandler)->ProcessClick();
}

/* Function start: 0x408E70 */
FireAlarmRockThrower::~FireAlarmRockThrower() {
}
