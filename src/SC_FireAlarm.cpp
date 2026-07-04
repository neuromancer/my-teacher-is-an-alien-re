#include "SC_FireAlarm.h"
#include "stubs.h"
#include "InputManager.h"
#include "SpriteAction.h"
#include "Memory.h"
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
#include "string.h"
#include "VBuffer.h"
#include "CDData.h"
#include "MouseControl.h"
extern char* MakeAnimName(int);


// FUN_004279a0 = ZBuffer::ResetItems (ZBuffer.h)
// FUN_00427880 = Weapon::UpdateProjectiles — callers updated

// Thiscall wrapper classes for sprite/palette methods




#include "globals.h"

// FireAlarmRockThrower — derived RockThrower with vtable 0x461118
// Overrides CheckTargetHit and OnHit to redirect to active handler (SC_FireAlarm)
// Functions at 0x408E40, 0x408E60; sdtor at 0x408E70
class FireAlarmRockThrower : public RockThrower {
public:
    FireAlarmRockThrower(Parser* parent) : RockThrower(parent) {}
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
    SC_FireAlarm::ShutDown(0);
}

/* Function start: 0x407590 */
void SC_FireAlarm::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    Handler::Init(msg);

    if (FileExists("CB_FireAlarm") == 0) {
        ShowLoadingScreen();
    }

    screenSize.x = 0x140;
    screenSize.y = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = action->addressValue;

    soundList = new SlimeTable();

    ParseFile(this, "mis\\CB_FireAlarm.mis", 0);

    if (spriteAction == 0) {
        SpriteAction* newAction = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        spriteAction = newAction;
    }
}

/* Function start: 0x4076E0 */
void SC_FireAlarm::ShutDown(SC_MessageParser* msg) {
    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }

    if (g_FireAlarmEngine_004685ac != 0) {
        // Virtual scalar-deleting-destructor dispatch (vtable slot 3, flag 1) —
        // written as `delete (Weapon*)...` this emits an extra implicit null test.
        int* obj = (int*)g_FireAlarmEngine_004685ac;
        int* vtbl = (int*)*obj;
        ((void (__fastcall*)(int*, int, int))vtbl[3])(obj, 0, 1);
        g_FireAlarmEngine_004685ac = 0;
    }

    if (consoleSprite != 0) {
        delete consoleSprite;
        consoleSprite = 0;
    }

    if (planeSprite != 0) {
        delete planeSprite;
        planeSprite = 0;
    }

    if (handSprite != 0) {
        delete handSprite;
        handSprite = 0;
    }

    if (teacherSprite != 0) {
        delete teacherSprite;
        teacherSprite = 0;
    }

    if (caughtsSprite != 0) {
        delete caughtsSprite;
        caughtsSprite = 0;
    }

    if (alarmSprite != 0) {
        delete alarmSprite;
        alarmSprite = 0;
    }

    if (spriteAction != 0) {
        delete spriteAction;
        spriteAction = 0;
    }

    if (soundList != 0) {
        delete soundList;
        soundList = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return;
}

/* Function start: 0x407930 */
int SC_FireAlarm::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    WriteMessageAddress(msg);

    if (action->lastKey == 0x1b && savedCommand == 0x2b) {
        gamePhase = 4;
    }

    return 1;
}

/* Function start: 0x407970 */
int SC_FireAlarm::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    if (action->addressType != handlerId) {
        return 0;
    }

    switch (action->instruction) {
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
        delete spriteAction;
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
        spriteAction->fromType = savedCommand;
        spriteAction->fromValue = savedMsgData;
    }

    // Enqueue and destroy
    EnqueueSpriteAction(spriteAction);
    if (spriteAction != 0) {
        delete spriteAction;
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
int SC_FireAlarm::HandleClick(Projectile* proj) {
    SlimeDim* nextPos = &proj->nextPos;
    int animIdx = proj->handle;
    int frameCount = proj->ranges[animIdx].dim.y;
    frameCount--;

    int hitResult = g_BackBuffer_0046aa14->CheckHit(nextPos->x, nextPos->y);

    if (planeClickRange.x <= hitResult && planeClickRange.y >= hitResult) {
        int y = nextPos->y - 0x4B;
        int x = nextPos->x - 0xA1;
        planeSprite->loc.x = x;
        planeSprite->loc.y = y;
        planeSprite->ResetAnimation(2, 0);
        soundList->Play(2);
        return 1;
    }

    Animation* anim = proj->animation_data;
    if (anim == 0) {
        if (frameCount != 0) {
            return 0;
        }
    } else {
        if (anim->smk->FrameNum != (unsigned int)frameCount) {
            return 0;
        }
    }

    int soundId;

    if (kidHitRange.x <= hitResult && kidHitRange.y >= hitResult) {
        kidHitCounter.x++;
        timerCounter.x += g_FireAlarmTimer_004685a0;
        int done;
        if (kidHitCounter.y != 0 && kidHitCounter.x >= kidHitCounter.y) {
            done = 1;
        } else {
            done = 0;
        }
        if (done) {
            soundList->Play(0xF);
            kidHitCounter.x = 0;
            goto returnOne;
        }
        soundId = 3;
        goto playSound;
    }

    // Original FUN_407C20: palette C2-CF plays SOUND 10, labelled SMP_HitTeacher
    // in CB_FIREALARM.MIS. DAT_00472bd8 is seeded from DAT_0046859c at startup.
    if (teacherHitRange.x <= hitResult && teacherHitRange.y >= hitResult) {
        soundId = 0xA;
        timerCounter.x += g_FireAlarmField1_00472bd8;
        goto playSound;
    }

    {
        int inSlot;
        if (alarmSlotRect.left > nextPos->x || alarmSlotRect.right < nextPos->x ||
            alarmSlotRect.top > nextPos->y || alarmSlotRect.bottom < nextPos->y) {
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
    bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);
    teacherSprite->Do(teacherSprite->loc.x, teacherSprite->loc.y, 1.0);
    handSprite->Do(handSprite->loc.x, handSprite->loc.y, 1.0);
    if (soundList->IsSamplePlaying(9) == 0) {
        gamePhase = 1;
        stateFlags &= ~8;
    }
}

/* Function start: 0x407EE0 */
void SC_FireAlarm::ProcessFrame() {
    Sprite* teacher;
    int caughtFrame;

    if ((stateFlags & 0xF) == 0 && timerCounter.y != 0 && timerCounter.x >= timerCounter.y) {
        timerCounter.x = 0;
        teacher = teacherSprite;
        int totalFrames = teacher->animation_data->smk->FrameNum;
        int distance = totalFrames - teacher->ranges[teacher->handle].dim.x + 1;
        animDistance = distance;

        if (distance >= 0x3D && distance <= 0x55) {
            int y = teacherHomePos.y;
            int x = teacherHomePos.x - 0x19;
            teacher->loc.x = x;
            teacher->loc.y = y;
        } else if (distance >= 0x15 && distance <= 0x32) {
            int y = teacherHomePos.y;
            int x = teacherHomePos.x + 0x19;
            teacher->loc.x = x;
            teacher->loc.y = y;
        }

        handSprite->ResetAnimation(0, 0);

        if (planeSprite->handle == 0) {
            planeSprite->ResetAnimation(-1, 0);
        }

        caughtsSprite->ResetAnimation(caughtsSprite->handle + 1, 0);

        caughtFrame = caughtsSprite->handle;
        if (caughtFrame == 2) {
            stateFlags |= 4;
        }

        ((RockThrower*)g_FireAlarmEngine_004685ac)->ResetProjectiles();

        teacherSprite->ResetAnimation(caughtFrame + 1, 0);

        soundList->Play(caughtFrame + 0xC);

        stateFlags |= 1;
    }

    bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);

    int renderResult = teacherSprite->Do(teacherSprite->loc.x, teacherSprite->loc.y, 1.0);

    if ((stateFlags & 0xF) == 0) {
        Animation* animData = teacherSprite->animation_data;
        int frameCount;
        if (animData == 0) {
            frameCount = 0;
        } else {
            frameCount = animData->smk->FrameNum;
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
        Sprite* spr = teacherSprite;
        int frame = spr->handle;
        if (frame >= 1 && frame <= 3) {
            {
                SlimeDim temp(teacherHomePos);
                spr->loc.x = temp.x;
                spr->loc.y = temp.y;
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
                    char* name = MakeAnimName(0x13A6);
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
        int dcRender = handSprite->Do(handSprite->loc.x, handSprite->loc.y, 1.0);
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
        int f8Render = planeSprite->Do(planeSprite->loc.x, planeSprite->loc.y, 1.0);
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
                    planeSprite->loc.x = temp2.x;
                    planeSprite->loc.y = temp2.y;
                }
                planeSprite->ResetAnimation(-1, 0);
                break;
            }
        }
    }

    {
        int c8Render = alarmSprite->Do(alarmSprite->loc.x, alarmSprite->loc.y, 1.0);
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

    caughtsSprite->Do(caughtsSprite->loc.x, caughtsSprite->loc.y, 1.0);

    if ((stateFlags & 0xF) == 0) {
        g_Mouse_0046aa18->DrawCursor();
        Weapon* engine = (Weapon*)g_FireAlarmEngine_004685ac;
        engine->UpdateProjectiles();

        int bcRender = consoleSprite->Do(consoleSprite->loc.x, consoleSprite->loc.y, 1.0);
        if (bcRender != 0) {
            int mouseVal = 0;
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseVal = g_InputManager_0046aa08->pMouse->x;
            }
            consoleSprite->ResetAnimation(mouseVal / (screenSize.x / 5), 0);
        }

        if (((Weapon*)g_FireAlarmEngine_004685ac)->m_clicked != 0) {
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
    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }

    teacherHitRange.x = 0xC2;
    teacherHitRange.y = 0xCF;
    kidHitRange.x = 0xB4;
    kidHitRange.y = 0xC1;
    planeClickRange.x = 0xD2;
    alarmSlotRect.top = 0x5B;
    alarmSlotRect.right = 0xEF;
    alarmSlotRect.bottom = 0x6E;
    teacherSlotRect.left = 0x7B;
    teacherSlotRect.top = 0x28;
    teacherSlotRect.right = 0xEE;
    teacherSlotRect.bottom = 0xAB;
    planeClickRange.y = 0xDF;
    alarmSlotRect.left = 0xDF;
    timerCounter.y = g_FireAlarmTimerTarget_0046859c;
    timerCounter.x = 0;
    roundCount = 0;
    kidHitCounter.x = 0;
    maxRounds = 7;
    kidHitCounter.y = 2;

    if (g_InputManager_0046aa08 != 0) {
        g_InputManager_0046aa08->Refresh(1);
    }

    if (consoleSprite != 0) {
        int mouseX = 0;
        // Original bug at 0x408530: DAT_0046aa08 is dereferenced after only the earlier Refresh guard.
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
        palette = pal;
        pal->Load(buffer);
    } else if (strcmp(label, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite(0);
        Parser::ProcessFile(bgSprite, this, 0);
    } else if (strcmp(label, "CONSOLE_SPRITE") == 0) {
        consoleSprite = new Sprite(0);
        Parser::ProcessFile(consoleSprite, this, 0);
    } else if (strcmp(label, "TEACHER_SPRITE") == 0) {
        teacherSprite = new Sprite(0);
        Parser::ProcessFile(teacherSprite, this, 0);
        teacherHomePos = teacherSprite->loc;
    } else if (strcmp(label, "PLANE_SPRITE") == 0) {
        planeSprite = new Sprite(0);
        Parser::ProcessFile(planeSprite, this, 0);
        planeHomePos = planeSprite->loc;
    } else if (strcmp(label, "HAND_SPRITE") == 0) {
        handSprite = new Sprite(0);
        Parser::ProcessFile(handSprite, this, 0);
    } else if (strcmp(label, "ALARM_SPRITE") == 0) {
        alarmSprite = new Sprite(0);
        Parser::ProcessFile(alarmSprite, this, 0);
    } else if (strcmp(label, "CAUGHTS_SPRITE") == 0) {
        caughtsSprite = new Sprite(0);
        Parser::ProcessFile(caughtsSprite, this, 0);
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
    return ((SC_FireAlarm*)g_GameEngine_0046a6ec->m_activeHandler)->HandleClick((Projectile*)param);
}

/* Function start: 0x408E60 */
void FireAlarmRockThrower::OnHit() {
    ((SC_FireAlarm*)g_GameEngine_0046a6ec->m_activeHandler)->ProcessClick();
}
