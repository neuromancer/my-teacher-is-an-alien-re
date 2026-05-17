#include "SC_DodgeOrville.h"
#include "stubs.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "GameState.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "main.h"
#include "Animation.h"

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern char* MakeAnimName(int);
extern "C" char* FormatAssetPath(char*, ...);
// FUN_00413e10 = ParseFile in Parser.h
extern "C" void ShowError(const char* format, ...);
extern "C" void OgdenTrace(const char* format, ...);
#include "SoundList.h"
#include "ZBufferManager.h"
#include "globals.h"

// HitBounds, BombData — defined in globals.h

// Original FUN_00428110 fills DAT_00473278 from the CRT initializer table.
static void __cdecl _dodge_orville_crt_init(void) {
    g_BombData_00473278[0].data[0] = 0x23;
    g_BombData_00473278[0].data[1] = 0x28;
    g_BombData_00473278[0].data[2] = 0x00;
    g_BombData_00473278[0].data[3] = 0x00;
    g_BombData_00473278[0].data[4] = 0x27;
    g_BombData_00473278[0].data[5] = 0x2c;

    g_BombData_00473278[1].data[0] = 0x00;
    g_BombData_00473278[1].data[1] = 0x00;
    g_BombData_00473278[1].data[2] = 0x40;
    g_BombData_00473278[1].data[3] = 0x45;
    g_BombData_00473278[1].data[4] = 0x43;
    g_BombData_00473278[1].data[5] = 0x48;

    g_BombData_00473278[2].data[0] = 0x60;
    g_BombData_00473278[2].data[1] = 0x65;
    g_BombData_00473278[2].data[2] = 0x5c;
    g_BombData_00473278[2].data[3] = 0x61;
    g_BombData_00473278[2].data[4] = 0x00;
    g_BombData_00473278[2].data[5] = 0x00;

    g_BombData_00473278[3].data[0] = 0x79;
    g_BombData_00473278[3].data[1] = 0x7e;
    g_BombData_00473278[3].data[2] = 0x7f;
    g_BombData_00473278[3].data[3] = 0x82;
    g_BombData_00473278[3].data[4] = 0x00;
    g_BombData_00473278[3].data[5] = 0x00;

    g_BombData_00473278[4].data[0] = 0x96;
    g_BombData_00473278[4].data[1] = 0x9b;
    g_BombData_00473278[4].data[2] = 0x9a;
    g_BombData_00473278[4].data[3] = 0x9f;
    g_BombData_00473278[4].data[4] = 0x00;
    g_BombData_00473278[4].data[5] = 0x00;

    g_BombData_00473278[5].data[0] = 0xb8;
    g_BombData_00473278[5].data[1] = 0xbd;
    g_BombData_00473278[5].data[2] = 0x00;
    g_BombData_00473278[5].data[3] = 0x00;
    g_BombData_00473278[5].data[4] = 0xb3;
    g_BombData_00473278[5].data[5] = 0xb8;
}

typedef void (__cdecl *_PVFV)(void);
#pragma data_seg(".CRT$XCU")
static _PVFV _init_dodge_orville = _dodge_orville_crt_init;
#pragma data_seg()

int CompareRange(int center, int pos, int range);
int CheckCursorRange(int range);

// SC_Combat base class calls — now using proper inheritance

// Parser-derived class used for MIS file action parsing
// vtable 0x461100 in original binary
// ActionParser is SC_MessageParser — vtable 0x461100


/* Function start: 0x428840 */
SC_DodgeOrville::SC_DodgeOrville() {
    memset(&reticlePos, 0, 0x50);

    handlerId = 0x43;
}

SC_DodgeOrville::~SC_DodgeOrville() {
    ShutDown(0);
}

/* Function start: 0x4289C0 */
void SC_DodgeOrville::Init(SC_MessageParser* msg) {
    memset(&reticlePos, 0, 0x50);
    OgdenTrace("[OGDEN] SC_DodgeOrville::Init begin this=%08lx msg=%08lx action=%d:%d from=%d:%d instr=%d",
        (unsigned long)this,
        (unsigned long)msg,
        ((SpriteAction*)msg)->addressType,
        ((SpriteAction*)msg)->addressValue,
        ((SpriteAction*)msg)->fromType,
        ((SpriteAction*)msg)->fromValue,
        ((SpriteAction*)msg)->instruction);

    if (FileExists("CB_DGOrv") == 0) {
        OgdenTrace("[OGDEN] SC_DodgeOrville::Init ShowLoadingScreen");
        ShowLoadingScreen();
    }

    SC_Combat::Init(msg);
    strcpy((char*)(combatParams + 5), "mis\\cb_DOrville.mis");
    OgdenTrace("[OGDEN] SC_DodgeOrville::Init ParseFile begin");
    ParseFile(this, (char*)(combatParams + 5), (char*)0);
    OgdenTrace("[OGDEN] SC_DodgeOrville::Init ParseFile end status=%08lx actions=%d bg=%08lx console=%08lx barBg=%08lx barFill=%08lx",
        (unsigned long)statusPtr,
        combatParams[2],
        (unsigned long)bgSprite,
        (unsigned long)field_0x10C,
        (unsigned long)barBgSprite,
        (unsigned long)barFillSprite);
}

/* Function start: 0x428A40 */
int SC_DodgeOrville::ShutDown(SC_MessageParser* msg) {
    void* ptr;

    OgdenTrace("[OGDEN] SC_DodgeOrville::ShutDown begin this=%08lx msg=%08lx barFill=%08lx barBg=%08lx",
        (unsigned long)this,
        (unsigned long)msg,
        (unsigned long)barFillSprite,
        (unsigned long)barBgSprite);
    ptr = (void*)barFillSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        barFillSprite = 0;
    }

    ptr = (void*)barBgSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        barBgSprite = 0;
    }

    SC_Combat::ShutDown(msg);

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    OgdenTrace("[OGDEN] SC_DodgeOrville::ShutDown end");
    return 0;
}

/* Function start: 0x428AD0 */
void SC_DodgeOrville::Update(int p1, int p2) {
    if (handlerId == p2) {
        OgdenTrace("[OGDEN] SC_DodgeOrville::Update active actions=%d status=%08lx", combatParams[2], (unsigned long)statusPtr);
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x428AF0 */
int SC_DodgeOrville::AddMessage(SC_MessageParser* msg) {
    int ret;

    ret = SC_Combat::AddMessage(msg);
    if (ret != 0) {
        return 1;
    }
    if (((SpriteAction*)msg)->lastKey == 0x1B) {
        statusPtr[2] = 1;
    }
    return 1;
}

/* Function start: 0x428B30 */
int SC_DodgeOrville::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x428B60 */
void SC_DodgeOrville::ProcessLose() {
    void* ptr;

    OgdenTrace("[OGDEN] SC_DodgeOrville::ProcessLose saved=%d:%d status0=%d status1=%d pending=%08lx",
        savedCommand,
        savedMsgData,
        statusPtr == 0 ? 0 : statusPtr[0],
        statusPtr == 0 ? 0 : statusPtr[1],
        (unsigned long)pendingAction);
    if (savedCommand != 0x2B) {
        if (statusPtr[1] != 0) {
            ptr = (void*)pendingAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_DOrville.mis", "[WIN_LBL]");

                pendingAction->fromType = savedCommand;
                pendingAction->fromValue = savedMsgData;
                pendingAction->extra1 = 6;

                GameState* gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("DODGE_COMBAT_AVAILABLE");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] = 0;

                gs = g_GameState_0046aa30;
                idx = gs->FindLabel("NUM_ACTIONS");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] += 0x1E;
            }
        }
        else if (statusPtr[0] != 0) {
            ptr = (void*)pendingAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_DOrville.mis", "[LOSE_LBL]");
            }
        }
        else {
            ptr = (void*)pendingAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_DOrville.mis", "[QUIT_LBL]");
            }
        }
    }
    else {
        if (statusPtr[1] != 0) {
            ptr = (void*)pendingAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_DOrville.mis", "[WIN_LBL_PR]");
            }
        }
        else if (statusPtr[0] != 0) {
            ptr = (void*)pendingAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_DOrville.mis", "[LOSE_LBL_PR]");
            }
        }
    }

    SC_Combat::ProcessLose();
}

/* Function start: 0x429110 */
void SC_DodgeOrville::UpdateGame()
{
    OgdenTrace("[OGDEN] SC_DodgeOrville::UpdateGame begin bg=%08lx console=%08lx barBg=%08lx barFill=%08lx throws=%d/%d hits=%d/%d",
        (unsigned long)bgSprite,
        (unsigned long)field_0x10C,
        (unsigned long)barBgSprite,
        (unsigned long)barFillSprite,
        throwState.x,
        throwState.y,
        hitCount.x,
        hitCount.y);
    if (bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0) != 0) {
        if (bgSprite->handle != 7) {
            OgdenTrace("[OGDEN] SC_DodgeOrville::UpdateGame ThrowBomb handle=%d", bgSprite->handle);
            ThrowBomb();
        }
    }

    int state = field_0x10C->handle;
    Animation* anim = bgSprite->animation_data;
    int frameVal = 0;
    if (anim != 0) {
        frameVal = anim->smk->FrameNum;
    }

    if (frameVal != 0 && state >= 0 && state <= 2) {
        if (g_HitBounds_00473260[state].minVal <= frameVal && g_HitBounds_00473260[state].maxVal >= frameVal) {
            OgdenTrace("[OGDEN] SC_DodgeOrville::UpdateGame hit state=%d frame=%d", state, frameVal);
            g_HitBounds_00473260[state].minVal = 0;
            g_HitBounds_00473260[state].maxVal = 0;
            field_0x10C->ResetAnimation(state + 7, 0);
            hitCount.x++;
            bgSound->Play(2);
            bgSound->Play(hitSoundIdx + 5);
            if (hitSoundIdx < 2) {
                hitSoundIdx++;
            }
        }
    }

    UpdateReticle();

    barBgSprite->Do(barBgSprite->loc_x, barBgSprite->loc_y, 1.0);
    barFillSprite->Do(barFillSprite->loc_x, barFillSprite->loc_y, 1.0);

    VBuffer* vb = 0;
    if (barBgSprite->animation_data != 0) {
        vb = barBgSprite->animation_data->targetBuffer;
    }

    g_ZBufferManager_0046aa24->DrawVBufferRegion(vb, 0x7531, barPos.x, barPos.y, 2, 1.0, clipStart.x, clipStart.y, clipEnd.x, clipEnd.y);

    if (hitCount.y != 0 && hitCount.x >= hitCount.y) {
        VBuffer* vb2 = 0;
        if (barFillSprite->animation_data != 0) {
            vb2 = barFillSprite->animation_data->targetBuffer;
        }
        g_ZBufferManager_0046aa24->DrawVBufferRegion(vb2, 0x7532, barPos.x, barPos.y, 2, 1.0, 0, 0, vb2->clip_x2, vb2->clip_y2);
    } else {
        int hits = hitCount.x;
        int maxHits = hitCount.y;
        int fillHeight = (hits * 0x7c) / maxHits + 0x1e;
        if (fillHeight >= clipEnd.x) {
            fillHeight = clipEnd.x;
        }

        VBuffer* rd3;
        Animation* animData = barFillSprite->animation_data;
        if (animData != 0) {
            rd3 = animData->targetBuffer;
        } else {
            rd3 = 0;
        }
        g_ZBufferManager_0046aa24->DrawVBufferRegion(rd3, 0x7532, barPos.x, barPos.y, 2, 1.0, clipStart.x, clipStart.y, fillHeight, clipEnd.y);
    }
}

/* Function start: 0x429380 */
void SC_DodgeOrville::UpdateReticle()
{
    if (field_0x10C->Do(field_0x10C->loc_x, field_0x10C->loc_y, 1.0) == 0) {
        return;
    }

    int state = reticlePos;
    if (state != 1 && state != 3) {
        cursorDir = CheckCursorRange(0x32);
    }

    int dir = cursorDir;
    int newState = reticlePos + dir;
    reticlePos = newState;
    if (newState >= 0) {
        if (newState > 4) {
            reticlePos = 4;
        }
    } else {
        reticlePos = 0;
    }

    int animState = g_AnimStates_0046ac30[reticlePos];
    if (dir < 0) {
        if (animState == 3 || animState == 5) {
            animState++;
        }
    }

    field_0x10C->ResetAnimation(animState, 0);
}

/* Function start: 0x429430 */
int CheckCursorRange(int range)
{
    GetCursorPos(&g_CursorPos_00473308);
    int result = CompareRange(0x200, g_CursorPos_00473308.x, range);
    SetCursorPos(0x200, 0x15e);
    return result;
}

/* Function start: 0x429470 */
int CompareRange(int center, int pos, int range)
{
    int diff = pos - center;
    if (diff > 0) {
        if (diff > range) {
            return 1;
        }
    } else if (diff < 0) {
        if (-diff > range) {
            return -1;
        }
    }
    return 0;
}

/* Function start: 0x4294A0 */
void SC_DodgeOrville::ThrowBomb()
{
    OgdenTrace("[OGDEN] SC_DodgeOrville::ThrowBomb begin throws=%d/%d hits=%d/%d",
        throwState.x,
        throwState.y,
        hitCount.x,
        hitCount.y);
    if (hitCount.y != 0 && hitCount.x >= hitCount.y) {
        statusPtr[0] = 1;
        OgdenTrace("[OGDEN] SC_DodgeOrville::ThrowBomb hit limit reached");
        return;
    }

    int tc = throwState.x + 1;
    throwState.x = tc;
    int maxThrows = throwState.y;

    int atLimit;
    if (maxThrows != 0 && maxThrows <= tc) {
        atLimit = 1;
    } else {
        atLimit = 0;
    }

    if (atLimit != 0) {
        statusPtr[1] = 1;
        OgdenTrace("[OGDEN] SC_DodgeOrville::ThrowBomb throw limit reached");
        return;
    }

    int dir;
    do {
        dir = rand() % 6;
    } while (dir == g_LastBombDir_0046ac44);

    g_LastBombDir_0046ac44 = dir;
    OgdenTrace("[OGDEN] SC_DodgeOrville::ThrowBomb dir=%d", dir);
    bgSprite->ResetAnimation(dir + 1, 0); // 0x108

    // Copy bomb data to hit bounds (3 entries)
    int* src = (int*)&g_BombData_00473278[dir];
    int* dst = (int*)&g_HitBounds_00473260[0];
    int* end = (int*)&g_HitBounds_00473260[3];
    do {
        dst[0] = src[0];
        dst[1] = src[1];
        src += 2;
        dst += 2;
    } while (dst < end);

    bgSound->Play(3); // 0x110

    int r = rand();
    int maxTh = throwState.y;
    int rem = r % maxTh;
    if (maxTh / 0x14 > rem) {
        bgSound->Play(8);
        return;
    }
    if (maxTh / 0xa > rem) {
        bgSound->Play(9);
    }
}

/* Function start: 0x4295C0 */
void SC_DodgeOrville::ProcessAction(int action, int* data) {
    OgdenTrace("[OGDEN] SC_DodgeOrville::ProcessAction action=%d value=%d", action, data == 0 ? 0 : *data);
    switch (action) {
    case 0:
        ProcessLose();
        break;
    case 1:
        ProcessLose();
        break;
    case 2:
        if (*data == 1) {
            *data = 2;
            bgSound->StopPlaying();
            bgSound->Play(1);
            bgSprite->ResetAnimation(7, 0);
            statusPtr[3] = 0;
        }
        if (bgSound->IsSamplePlaying(1) == 0) {
            *data = 0;
            ProcessLose();
        }
        break;
    case 3:
        if (*data == 1) {
            *data = 2;
            if (cineIds[0] != 0) {
                char* name = MakeAnimName(cineIds[0]);
                char* path = FormatAssetPath(name);
                OgdenTrace("[OGDEN] SC_DodgeOrville::ProcessAction intro anim=%d path=%s", cineIds[0], path);
                if (FileExists(path) != 0) {
                    Animation anim;
                    anim.Play(path, 0);
                }
            }
            bgSound->Play(0);
            OgdenTrace("[OGDEN] SC_DodgeOrville::ProcessAction intro sound start bgSound=%08lx bgSoundId=%d",
                (unsigned long)bgSound,
                field_0x114);
            if (field_0x114 != 0) {
                SendGameMessage(5, field_0x114, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
            }
            bgSprite->ResetAnimation(7, 0);
        }
        if (bgSound->IsSamplePlaying(0) == 0) {
            OgdenTrace("[OGDEN] SC_DodgeOrville::ProcessAction intro done");
            *data = 0;
            bgSprite->ResetAnimation(0, 0);
        }
        break;
    case 4:
        UpdateGame();
        break;
    default:
        ShowError("SC_DodgeOrville::Process_Action - invalid Action=%d, value=%d", action, *data);
        break;
    }
}

/* Function start: 0x4297D0 */
void SC_DodgeOrville::OnProcessStart()
{
    OgdenTrace("[OGDEN] SC_DodgeOrville::OnProcessStart");
    clipStart.x = 0;
    clipStart.y = 0;
    clipEnd.x = 0xb3;
    clipEnd.y = 0x13;
    barPos.x = 0x46;
    barPos.y = 0x1e;

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindLabel("MAX_HITS_BY_STINK_BOMBS");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    hitCount.x = 0;
    hitCount.y = gs->stateValues[idx];
    OgdenTrace("[OGDEN] SC_DodgeOrville::OnProcessStart maxHits=%d", hitCount.y);
}

/* Function start: 0x429860 */
void SC_DodgeOrville::OnProcessEnd()
{
    OgdenTrace("[OGDEN] SC_DodgeOrville::OnProcessEnd begin");
    SC_Combat::OnProcessEnd();
    CheckCursorRange(0);
    roundReset = 0; // 0x120
    reticlePos = 2; // 0x118
    cursorDir = 0; // 0x11c
    OgdenTrace("[OGDEN] SC_DodgeOrville::OnProcessEnd end");
}

/* Function start: 0x4298A0 */
int SC_DodgeOrville::LBLParse(char* line) {
    int index = 0;
    int value = 0;
    char label[32];

    label[0] = '\0';
    sscanf(line, " %s ", label);

    if (strcmp(label, "NUMBER_OF_THROWS") == 0) {
        sscanf(line, " %s %d ", label, &throwState.y);
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse NUMBER_OF_THROWS %d", throwState.y);
    } else if (strcmp(label, "MAX_HITS_BY_STINK_BOMBS") == 0) {
        sscanf(line, " %s %d ", label, &hitCount.y);
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse MAX_HITS_BY_STINK_BOMBS %d", hitCount.y);
    } else if (strcmp(label, "STINK_METER_BASE_SPRITE") == 0) {
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse STINK_METER_BASE_SPRITE begin");
        Sprite* spr = new Sprite((char*)0);
        barBgSprite = spr;
        Parser::ProcessFile(spr, this, (char*)0);
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse STINK_METER_BASE_SPRITE end sprite=%08lx", (unsigned long)spr);
    } else if (strcmp(label, "STINK_METER_SPRITE") == 0) {
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse STINK_METER_SPRITE begin");
        Sprite* spr = new Sprite((char*)0);
        barFillSprite = spr;
        Parser::ProcessFile(spr, this, (char*)0);
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse STINK_METER_SPRITE end sprite=%08lx", (unsigned long)spr);
    } else if (strcmp(label, "ANIM") == 0) {
        int result = sscanf(line, " %s %d %d", label, &index, &value);
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse ANIM result=%d index=%d value=%d", result, index, value);
        if (result != 3 || index < 0 || index > 2) {
            ReportUnknownLabel("SC_DodgeOrville");
        }
        cineIds[index] = value;
    } else if (strcmp(label, "END") == 0) {
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse END");
        return 1;
    } else {
        OgdenTrace("[OGDEN] SC_DodgeOrville::LBLParse base token=%s", label);
        SC_Combat::LBLParse(line);
    }

    return 0;
}
