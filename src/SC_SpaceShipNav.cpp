#include "SC_SpaceShipNav.h"
#include "stubs.h"
#include "SC_CombatBase.h"
#include "Memory.h"
#include "Sprite.h"
#include "Animation.h"
#include "mCNavigator.h"
#include "MouseControl.h"
#include "InputManager.h"
#include "SoundList.h"

#include "globals.h"

/* Function start: 0x447520 */
SC_SpaceShipNav::SC_SpaceShipNav() {
}

/* Function start: 0x4475B0 */
SC_SpaceShipNav::~SC_SpaceShipNav() {
    ShutDown(0);

    if (field_118[0] != 0) {
        int* obj = (int*)field_118[0];
        int* vtbl = (int*)*obj;
        ((void (__fastcall *)(int*, int, int))vtbl[3])(obj, 0, 1);
        field_118[0] = 0;
    }
}

/* Function start: 0x447630 */
void SC_SpaceShipNav::Init(SC_MessageParser* msg) {
    TODO("SC_SpaceShipNav::Init");
}

/* Function start: 0x447790 */
int SC_SpaceShipNav::ShutDown(SC_MessageParser* msg) {
    if (g_CombatEngine_0046ae78 != 0) {
        if (g_Navigator_0046ae70 != 0) {
            Sprite* spr = ((mCNavigator*)g_Navigator_0046ae70)->sprite;
            if (spr != 0) {
                spr->StopAnimationSound();
            }
        }
        g_CombatEngine_0046ae78->RenderState();
        g_CombatEngine_0046ae78 = 0;
    }
    return SC_Combat::ShutDown(msg);
}

/* Function start: 0x4477F0 */
void SC_SpaceShipNav::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x447810 */
int SC_SpaceShipNav::AddMessage(SC_MessageParser* msg) {
    TODO("SC_SpaceShipNav::AddMessage");
    return 0;
}

/* Function start: 0x447910 */
int SC_SpaceShipNav::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x447940 */
void SC_SpaceShipNav::ProcessLose() {
    TODO("SC_SpaceShipNav::ProcessLose");
}

// g_SpaceNavStates_0046c3f0 — defined in globals.cpp

/* Function start: 0x447950 */
void SC_SpaceShipNav::UpdateCursor() {
    int iVar1 = ((mCNavigator*)g_Navigator_0046ae70)->Update();
    if (iVar1 == 0 && g_Mouse_0046aa18->m_sprite != 0 &&
        g_Mouse_0046aa18->m_sprite->handle != -1) {
        int mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x * 3;
        }
        iVar1 = mouseX / *(int*)((int)this + 0xB8);
        if (iVar1 < 0) iVar1 = 0;
        else if (iVar1 > 2) iVar1 = 2;
        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(g_SpaceNavStates_0046c3f0[iVar1], 0);
        }
        g_Mouse_0046aa18->DrawCursor();
    }
    if (*(int*)((int)g_Navigator_0046ae70 + 0xA0) != 0) {
        Animation* anim = *(Animation**)((int)g_Navigator_0046ae70 + 0xA0 + 0x50);
        int frameNum = 0;
        if (anim != 0) {
            frameNum = anim->smk->FrameNum;
        }
        switch (frameNum) {
        case 0xA3:
        case 0xC6:
        case 0xE9:
        case 299:
            bgSound->Play(0);
            return;
        case 0x122:
            bgSound->Play(1);
            return;
        }
    }
}

/* Function start: 0x447AD0 */
void SC_SpaceShipNav::ProcessAction(int action, int* data) {
    TODO("SC_SpaceShipNav::ProcessAction");
}

/* Function start: 0x447B50 */
void SC_SpaceShipNav::OnProcessEnd() {
    TODO("SC_SpaceShipNav::OnProcessEnd");
}

/* Function start: 0x447B60 */
int SC_SpaceShipNav::LBLParse(char* line) {
    char keyword[32];
    keyword[0] = 0;
    sscanf(line, "%s", keyword);

    if (strcmp(keyword, "NAVIGATION") == 0) {
        Parser::ProcessFile((Parser*)g_Navigator_0046ae70, this, 0);
        return 0;
    }
    if (strcmp(keyword, "END") == 0) {
        return 1;
    }
    SC_Combat::LBLParse(line);
    return 0;
}
