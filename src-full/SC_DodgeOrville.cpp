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
    memset(field_118, 0, 0x50);
    handlerId = 0x43;
}

SC_DodgeOrville::~SC_DodgeOrville() {
    Cleanup(0);
}

/* Function start: 0x428A40 */
void SC_DodgeOrville::Cleanup(int flag) {
    void* ptr;

    ptr = (void*)field_12C;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        field_12C = 0;
    }

    ptr = (void*)field_130;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        field_130 = 0;
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
    if (*((int*)msg + 0xB) == 0x1B) {
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

                void* gs = g_GameState_0046aa30;
                int idx = ((GameState*)gs)->FindLabel("DODGE_COMBAT_AVAILABLE");
                if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = 0;

                gs = g_GameState_0046aa30;
                idx = ((GameState*)gs)->FindLabel("NUM_ACTIONS");
                if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                *(int*)(*(int*)((int)gs + 0x90) + idx * 4) += 0x1E;
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
    Sprite* spr108 = (Sprite*)bgSprite; // 0x108
    spr108->Do(spr108->loc_x, spr108->loc_y, 1.0);
    if (spr108->Do(spr108->loc_x, spr108->loc_y, 1.0) != 0) {
        if (spr108->handle != 7) {
            SC_DodgeOrville::ThrowBomb();
        }
    }

    Sprite* spr10c = (Sprite*)field_0x10C; // 0x10c
    spr108 = (Sprite*)bgSprite; // 0x108
    int state = spr10c->handle;
    int f0val = (int)spr108->animation_data;
    int frameVal = 0;
    if (f0val != 0) {
        frameVal = *(int*)(*(int*)(f0val + 0xc) + 0x374);
    }

    if (frameVal != 0 && state >= 0 && state <= 2) {
        if (g_HitBounds_00473260[state].minVal <= frameVal && g_HitBounds_00473260[state].maxVal >= frameVal) {
            g_HitBounds_00473260[state].minVal = 0;
            g_HitBounds_00473260[state].maxVal = 0;
            spr10c->ResetAnimation(state + 7, 0);
            SC_DodgeOrville::dim_144.field_0++;
            bgSound->Play(2); // 0x110
            bgSound->Play(field_154[0] + 5);
            if (field_154[0] < 2) {
                field_154[0]++;
            }
        }
    }

    SC_DodgeOrville::UpdateReticle();

    Sprite* spr130 = (Sprite*)field_130;
    spr130->Do(spr130->loc_x, spr130->loc_y, 1.0);
    Sprite* spr12c = (Sprite*)field_12C;
    spr12c->Do(spr12c->loc_x, spr12c->loc_y, 1.0);

    spr130 = (Sprite*)field_130;
    int renderData = 0;
    if ((int)spr130->animation_data != 0) {
        renderData = *(int*)((int)spr130->animation_data + 0x18);
    }

    g_ZBufferManager_0046aa24->DrawVBufferRegion(0, 0x7531, dim_14C.field_0, dim_14C.field_4, 2, 1.0, dim_134.field_0, dim_134.field_4, dim_13C.field_0, dim_13C.field_4);

    if (dim_144.field_4 != 0 && dim_144.field_0 >= dim_144.field_4) {
        spr12c = (Sprite*)field_12C;
        int renderData2 = 0;
        if ((int)spr12c->animation_data != 0) {
            renderData2 = *(int*)((int)spr12c->animation_data + 0x18);
        }
        int w = *(int*)(renderData2 + 0x24);
        int h = *(int*)(renderData2 + 0x2c);
        g_ZBufferManager_0046aa24->DrawVBufferRegion(0, 0x7532, dim_14C.field_0, dim_14C.field_4, 2, 1.0, 0, w, 0, h);
    } else {
        int hits = dim_144.field_0;
        int maxHits = dim_144.field_4;
        int fillHeight = (hits * 0x7c) / maxHits + 0x1e;
        if (fillHeight >= dim_13C.field_0) {
            fillHeight = dim_13C.field_0;
        }

        spr12c = (Sprite*)field_12C;
        int rd3 = 0;
        if ((int)spr12c->animation_data != 0) {
            rd3 = *(int*)((int)spr12c->animation_data + 0x18);
        }
        g_ZBufferManager_0046aa24->DrawVBufferRegion(0, 0x7532, dim_14C.field_0, dim_14C.field_4, 2, 1.0, dim_134.field_0, fillHeight, dim_134.field_4, dim_13C.field_4);
    }
}

/* Function start: 0x429380 */
void SC_DodgeOrville::UpdateReticle()
{
    Sprite* spr10c = (Sprite*)field_0x10C; // 0x10c
    if (spr10c->Do(spr10c->loc_x, spr10c->loc_y, 1.0) == 0) {
        return;
    }

    int state = field_118[0];
    if (state != 1 && state != 3) {
        field_118[1] = CheckCursorRange(0x32);
    }

    int dir = field_118[1];
    int newState = field_118[0] + dir;
    field_118[0] = newState;
    if (newState < 0) {
        field_118[0] = 0;
    } else if (newState > 4) {
        field_118[0] = 4;
    }

    int animState = g_AnimStates_0046ac30[field_118[0]];
    if (dir < 0) {
        if (animState == 3 || animState == 5) {
            animState++;
        }
    }

    spr10c->ResetAnimation(animState, 0);
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
    if (dim_144.field_4 != 0 && dim_144.field_0 >= dim_144.field_4) {
        statusPtr[0] = 1;
        return;
    }

    int throwCount = field_118[2] + 1; // 0x124
    field_118[2] = throwCount;
    int maxThrows = dim_124.field_0; // 0x128 (via field_118[3] alias)

    int atLimit;
    if (maxThrows == 0) {
        atLimit = 0;
    } else {
        atLimit = (maxThrows <= throwCount) ? 1 : 0;
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

    // Copy bomb data to hit bounds
    int* src = (int*)&g_BombData_00473278[dir];
    int* dst = (int*)&g_HitBounds_00473260[0];
    do {
        dst[0] = src[0];
        dst[1] = src[1];
        src += 2;
        dst += 2;
    } while ((unsigned int)dst < 0x473278);

    bgSound->Play(3); // 0x110

    int r = rand();
    int maxTh = dim_124.field_0;
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
    dim_134.field_0 = 0;
    dim_134.field_4 = 0;
    dim_13C.field_0 = 0xb3;
    dim_13C.field_4 = 0x13;
    dim_14C.field_0 = 0x46;
    dim_14C.field_4 = 0x1e;

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindLabel("MAX_HITS_BY_STINK_BOMBS");
    if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    dim_144.field_0 = 0;
    dim_144.field_4 = *(int*)(*(int*)((int)gs + 0x90) + idx * 4);
}

/* Function start: 0x429860 */
void SC_DodgeOrville::InitReset()
{
    SC_Combat::OnProcessEnd();
    CheckCursorRange(0);
    field_118[2] = 0; // 0x120
    field_118[0] = 2; // 0x118
    field_118[1] = 0; // 0x11c
}
