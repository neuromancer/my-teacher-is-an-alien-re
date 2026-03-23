#include "SC_DodgeOrville.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "GameState.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_00413e10 = ParseFile in Parser.h
extern "C" void ShowError(const char* format, ...);
#include "SoundList.h"
#include "ZBufferManager.h"
extern ZBufferManager* g_ZBufferManager_0046aa24;
extern int g_AnimStates_0046ac30[5];
extern int g_LastBombDir_0046ac44;

struct HitBounds {
    int minVal;
    int maxVal;
};
extern HitBounds g_HitBounds_00473260[3];

struct BombData {
    int data[6];
};
extern BombData g_BombData_00473278[6];

extern POINT g_CursorPos_00473308;



int CompareRange(int center, int pos, int range);
int CheckCursorRange(int range);

// SC_Combat base class calls — now using proper inheritance

extern "C" { extern GameState* g_GameState_0046aa30; }

// Parser-derived class used for MIS file action parsing
// vtable 0x461100 in original binary
// ActionParser is SC_Message — vtable 0x461100


/* Function start: 0x428840 */
SC_DodgeOrville::SC_DodgeOrville() {
    memset(&reticlePos, 0, 0x50);

    handlerId = 0x43;
}

SC_DodgeOrville::~SC_DodgeOrville() {
    Cleanup(0);
}

/* Function start: 0x428A40 */
void SC_DodgeOrville::Cleanup(int flag) {
    void* ptr;

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

    SC_Combat::ShutDown((SC_Message*)flag);

    if (flag != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x428AF0 */
int SC_DodgeOrville::AddMessage(SC_Message* msg) {
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

/* Function start: 0x428B60 */
void SC_DodgeOrville::ProcessTargets() {
    void* ptr;

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
                SC_Message temp;
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
                SC_Message temp;
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
                SC_Message temp;
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
                SC_Message temp;
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
                SC_Message temp;
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
    if (((Sprite*)bgSprite)->Do(((Sprite*)bgSprite)->loc_x, ((Sprite*)bgSprite)->loc_y, 1.0) != 0) {
        if (((Sprite*)bgSprite)->handle != 7) {
            ThrowBomb();
        }
    }

    int state = ((Sprite*)field_0x10C)->handle;
    Animation* anim = ((Sprite*)bgSprite)->animation_data;
    int frameVal = 0;
    if (anim != 0) {
        frameVal = anim->smk->FrameNum;
    }

    if (frameVal != 0 && state >= 0 && state <= 2) {
        if (g_HitBounds_00473260[state].minVal <= frameVal && g_HitBounds_00473260[state].maxVal >= frameVal) {
            g_HitBounds_00473260[state].minVal = 0;
            g_HitBounds_00473260[state].maxVal = 0;
            ((Sprite*)field_0x10C)->ResetAnimation(state + 7, 0);
            hitCount.field_0++;
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

    g_ZBufferManager_0046aa24->DrawVBufferRegion(vb, 0x7531, barPos.field_0, barPos.field_4, 2, 1.0, clipStart.field_0, clipStart.field_4, clipEnd.field_0, clipEnd.field_4);

    if (hitCount.field_4 != 0 && hitCount.field_0 >= hitCount.field_4) {
        VBuffer* vb2 = 0;
        if (barFillSprite->animation_data != 0) {
            vb2 = barFillSprite->animation_data->targetBuffer;
        }
        g_ZBufferManager_0046aa24->DrawVBufferRegion(vb2, 0x7532, barPos.field_0, barPos.field_4, 2, 1.0, 0, vb2->clip_y2, 0, vb2->clip_x2);
    } else {
        int hits = hitCount.field_0;
        int maxHits = hitCount.field_4;
        int fillHeight = (hits * 0x7c) / maxHits + 0x1e;
        if (fillHeight >= clipEnd.field_0) {
            fillHeight = clipEnd.field_0;
        }

        VBuffer* rd3;
        Animation* animData = barFillSprite->animation_data;
        if (animData != 0) {
            rd3 = animData->targetBuffer;
        } else {
            rd3 = 0;
        }
        g_ZBufferManager_0046aa24->DrawVBufferRegion(rd3, 0x7532, barPos.field_0, barPos.field_4, 2, 1.0, clipStart.field_0, fillHeight, clipStart.field_4, clipEnd.field_4);
    }
}

/* Function start: 0x429380 */
void SC_DodgeOrville::UpdateReticle()
{
    if (((Sprite*)field_0x10C)->Do(((Sprite*)field_0x10C)->loc_x, ((Sprite*)field_0x10C)->loc_y, 1.0) == 0) {
        return;
    }

    int state = reticlePos;
    if (state != 1 && state != 3) {
        cursorDir = CheckCursorRange(0x32);
    }

    int dir = cursorDir;
    int newState = reticlePos + dir;
    reticlePos = newState;
    if (newState < 0) {
        reticlePos = 0;
    } else if (newState > 4) {
        reticlePos = 4;
    }

    int animState = g_AnimStates_0046ac30[reticlePos];
    if (dir < 0) {
        if (animState == 3 || animState == 5) {
            animState++;
        }
    }

    ((Sprite*)field_0x10C)->ResetAnimation(animState, 0);
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
    if (hitCount.field_4 != 0 && hitCount.field_0 >= hitCount.field_4) {
        statusPtr[0] = 1;
        return;
    }

    int tc = throwState.field_0 + 1;
    throwState.field_0 = tc;
    int maxThrows = throwState.field_4;

    int atLimit;
    if (maxThrows == 0) {
        atLimit = 0;
    } else {
        atLimit = (maxThrows <= tc) ? 1 : 0;
    }

    if (atLimit != 0) {
        statusPtr[1] = 1;
        return;
    }

    int dir;
    do {
        dir = rand() % 6;
    } while (dir == g_LastBombDir_0046ac44);

    g_LastBombDir_0046ac44 = dir;
    ((Sprite*)bgSprite)->ResetAnimation(dir + 1, 0); // 0x108

    // Copy bomb data to hit bounds (3 entries)
    int* src = (int*)&g_BombData_00473278[dir];
    int* dst = (int*)&g_HitBounds_00473260[0];
    int* end = (int*)&g_BombData_00473278[0];
    do {
        dst[0] = src[0];
        dst[1] = src[1];
        src += 2;
        dst += 2;
    } while (dst < end);

    bgSound->Play(3); // 0x110

    int r = rand();
    int maxTh = throwState.field_4;
    int rem = r % maxTh;
    if (maxTh / 0x14 > rem) {
        bgSound->Play(8);
        return;
    }
    if (maxTh / 0xa > rem) {
        bgSound->Play(9);
    }
}

/* Function start: 0x4297D0 */
void SC_DodgeOrville::InitGameState()
{
    clipStart.field_0 = 0;
    clipStart.field_4 = 0;
    clipEnd.field_0 = 0xb3;
    clipEnd.field_4 = 0x13;
    barPos.field_0 = 0x46;
    barPos.field_4 = 0x1e;

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindLabel("MAX_HITS_BY_STINK_BOMBS");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    hitCount.field_0 = 0;
    hitCount.field_4 = gs->stateValues[idx];
}

/* Function start: 0x429860 */
void SC_DodgeOrville::InitReset()
{
    SC_Combat::OnProcessEnd();
    CheckCursorRange(0);
    field_120 = 0; // 0x120
    reticlePos = 2; // 0x118
    cursorDir = 0; // 0x11c
}
