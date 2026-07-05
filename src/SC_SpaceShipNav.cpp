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
#include "SpriteAction.h"
#include "SpaceShipEngine.h"
#include "Parser.h"

#include "main.h"
extern Parser* ParseFile(Parser* parser, char* filename, char* key_format, ...);

#include "globals.h"

/* Function start: 0x447520 */
SC_SpaceShipNav::SC_SpaceShipNav() {
    engine = 0;
    field_0x11C = 0;
    handlerId = 0x46;
}

/* Function start: 0x4475B0 */
SC_SpaceShipNav::~SC_SpaceShipNav() {
    ShutDown(0);

    if (engine != 0) {
        delete engine;
        engine = 0;
    }
}

/* Function start: 0x447B40 */
void SC_SpaceShipNav::OnProcessStart()
{
}

/* Function start: 0x447630 */
void SC_SpaceShipNav::Init(SC_MessageParser* msg) {
    engine = 0;
    field_0x11C = 0;

    if (FileExists("CB_ShipNav") == 0) {
        ShowLoadingScreen();
    }

    SC_Combat::Init(msg);

    if (engine == 0) {
        SpaceShipEngine* newEngine = new SpaceShipEngine();
        engine = newEngine;
        g_CombatEngine_0046ae78 = (SC_CombatBase*)newEngine;

        strcpy(missionPath, "mis\\cb_SpaceNav.mis");
        ParseFile((Parser*)this, missionPath, 0);
    }

    g_CombatEngine_0046ae78 = (SC_CombatBase*)engine;
    g_CombatEngine_0046ae78->SetupViewport();

    if (msg != 0) {
        SpriteAction* action = (SpriteAction*)msg;
        if (action->extra1 == 1) {
            g_Navigator_0046ae70->SetNavParams(action->mousePos.x, action->mousePos.y);
        }
    }

    if (field_0x114 != 0) {
        SendGameMessage(5, field_0x114, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x447790 */
void SC_SpaceShipNav::ShutDown(SC_MessageParser* msg) {
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
    SC_Combat::ShutDown(msg);
}

/* Function start: 0x4477F0 */
void SC_SpaceShipNav::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x447810 */
int SC_SpaceShipNav::AddMessage(SC_MessageParser* msg) {
    if (SC_Combat::AddMessage(msg) == 0) {
        SpriteAction* action = (SpriteAction*)msg;
        if (savedCommand == 0x2b) {
            if (action->lastKey == 0x1b) {
                statusPtr[2] = 1;
            }
        } else if (action->lastKey == 0x77) {
            mCNavigator* nav = (mCNavigator*)g_Navigator_0046ae70;
            {
                SpriteAction temp(handlerId, moduleParam, handlerId, moduleParam,
                                  4, 1, 0, 0, nav->startingNode, nav->bearing);
                g_PendingAction_00472d58.CopyFrom(&temp);
            }
            SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        }
    }
    return 1;
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
    SC_Combat::ProcessLose();
}

// g_SpaceNavStates_0046c3f0 — defined in globals.cpp

/* Function start: 0x447950 */
void SC_SpaceShipNav::UpdateCursor() {
    int iVar1 = ((mCNavigator*)g_Navigator_0046ae70)->Update();
    if (iVar1 == 0 && g_Mouse_0046aa18->m_sprite != 0 &&
        g_Mouse_0046aa18->m_sprite->handle != -1) {
        int mouseX;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x * 3;
        } else {
            mouseX = 0;
        }
        iVar1 = mouseX / combatDim.x;
        if (iVar1 >= 0) {
            if (iVar1 > 2) iVar1 = 2;
        } else {
            iVar1 = 0;
        }
        int state = g_SpaceNavStates_0046c3f0[iVar1];
        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(state, 0);
        }
        g_Mouse_0046aa18->DrawCursor();
    }
    Sprite* navObj = g_Navigator_0046ae70->sprite;
    if (navObj != 0) {
        Animation* anim = navObj->animation_data;
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
            break;
        case 0x122:
            bgSound->Play(1);
            break;
        }
    }
}

/* Function start: 0x447AD0 */
void SC_SpaceShipNav::ProcessAction(int action, int* data) {
    switch (action) {
    case 0:
        ProcessLose();
        break;
    case 1:
        ProcessLose();
        break;
    case 2:
        ProcessLose();
        break;
    case 3:
        *data = 0;
        break;
    case 4:
        UpdateCursor();
        break;
    default:
        ShowError("SC_SpaceShipNav::Process_Action - invalid Action=%d, value=%d", action, *data);
        break;
    }
}

/* Function start: 0x447B50 */
void SC_SpaceShipNav::OnProcessEnd() {
    SC_Combat::OnProcessEnd();
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
