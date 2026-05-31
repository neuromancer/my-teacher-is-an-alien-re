#include "SC_FakeRoom.h"
#include "SC_Question.h"
#include "SoundList.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "GameState.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "Memory.h"
#include <string.h>

#include "globals.h"

// ActionParser is SC_MessageParser — vtable 0x461100, LBLParse at 0x444E60
// SC_MessageParser::targetAddress (0x90) stores the SpriteAction* to fill

/* Function start: 0x4441E0 */
SC_FakeRoom::SC_FakeRoom()
{
    memset(&stateFlags, 0, 6 * 4);
    handlerId = 0x49;
}

/* Function start: 0x444380 */
void SC_FakeRoom::ShutDown(SC_MessageParser* msg)
{
    SC_Combat::ShutDown(msg);
    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId,
                       moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return;
}

/* Function start: 0x4443E0 */
int SC_FakeRoom::AddMessage(SC_MessageParser* msg)
{
    if (SC_Combat::AddMessage(msg) != 0) {
        return 1;
    }

    SpriteAction* action = (SpriteAction*)msg;
    if (action->lastKey == 0x1b) {
        if (savedCommand == 0x2b) {
            statusPtr[2] = 1;  // [2] = escape/win
            return 1;
        }
    } else if (action->button1 > 1) {
        int mouseX = action->mousePos.x;
        if (stateFlags & 1) {
            if (mouseX < 100 || mouseX > 0x21c) {
                stateFlags = stateFlags & ~1;
                bgSprite->ResetAnimation(1, 0);
                return 1;
            }
        } else if (hitbox.left <= mouseX && hitbox.right >= mouseX &&
                   hitbox.top <= action->mousePos.y && hitbox.bottom >= action->mousePos.y) {
            statusPtr[1] = 1;  // [1] = hitbox clicked
        }
    }

    return 1;
}

/* Function start: 0x4448A0 */
int SC_FakeRoom::LBLParse(char* line)
{
    char label[32];
    label[0] = 0;
    sscanf(line, "%s", label);

    if (strcmp(label, "END") == 0) {
        return 1;
    }

    SC_Combat::LBLParse(line);
    return 0;
}

/* Function start: 0x444680 */
void SC_FakeRoom::RenderFrame()
{
    int done = bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);
    if (done != 0) {
        statusPtr[0] = 1;  // [0] = bgAnim done
    }

    if ((stateFlags & 1) != 0) {
        InputState* pMouse = g_InputManager_0046aa08->pMouse;
        if (pMouse == 0 || pMouse->x < 100) {
            Sprite* cursor = g_Mouse_0046aa18->m_sprite;
            if (cursor != 0) {
                cursor->ResetAnimation(4, 0);
            }
        } else if (pMouse == 0 || pMouse->x <= 0x21c) {
            Sprite* cursor = g_Mouse_0046aa18->m_sprite;
            if (cursor != 0) {
                cursor->ResetAnimation(0, 0);
            }
        } else {
            Sprite* cursor = g_Mouse_0046aa18->m_sprite;
            if (cursor != 0) {
                cursor->ResetAnimation(3, 0);
            }
        }
        g_Mouse_0046aa18->DrawCursor();
        return;
    }

    int mouseY;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }

    int mouseX;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    if (hitbox.left <= mouseX && mouseX <= hitbox.right &&
        hitbox.top <= mouseY && mouseY <= hitbox.bottom) {
        Sprite* cursor = g_Mouse_0046aa18->m_sprite;
        if (cursor != 0) {
            cursor->ResetAnimation(0x14, 0);
        }
    } else {
        Sprite* cursor = g_Mouse_0046aa18->m_sprite;
        if (cursor != 0) {
            cursor->ResetAnimation(0, 0);
        }
    }
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x4447B0 */
void SC_FakeRoom::ProcessAction(int action, int* data)
{
    if (action > 4) {
        ShowError("SC_FakeRoom::Process_Action - invalid Action=%d, value=%d", action, *data);
        return;
    }

    switch (action) {
    case 0:
        ((SC_Combat*)this)->ProcessLose();
        return;
    case 1:
        ((SC_Combat*)this)->ProcessLose();
        return;
    case 2:
        ((SC_Combat*)this)->ProcessLose();
        return;
    case 3:
        *data = 0;
        bgSound->Play(0);
        stateFlags = stateFlags | 1;
        return;
    case 4:
        RenderFrame();
        return;
    }
}

SC_FakeRoom::~SC_FakeRoom()
{
}

/* Function start: 0x444840 */ /* No assembly extracted */
void SC_FakeRoom::OnProcessEnd() {}

/* Function start: 0x444320 */
void SC_FakeRoom::Init(SC_MessageParser* msg) {
    memset(&stateFlags, 0, 6 * sizeof(int));
    SC_Combat::Init(msg);
    strcpy(missionPath, "mis\\CB_FakeRoom.mis");
    ParseFile(this, missionPath, (char*)0);
}

/* Function start: 0x4443C0 */
void SC_FakeRoom::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x4444B0 */
int SC_FakeRoom::Exit(SC_MessageParser* msg) {
    if (handlerId != ((SpriteAction*)msg)->addressType) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x4444E0 */
void SC_FakeRoom::ProcessLose()
{
    if (statusPtr[0] != 0) {  // [0] = bgAnim done
        statusPtr[4] = 0;     // [4] = lose state

        if (pendingAction != 0) {
            delete pendingAction;
            pendingAction = 0;
        }

        pendingAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        SC_MessageParser temp;
        temp.targetAddress = (int)pendingAction;
        ParseFile(&temp, "mis\\CB_FakeRoom.mis", "[LOSE_MESSAGE]");

        if (savedCommand == 0x2b) {
            pendingAction->fromType = 0x2b;
            pendingAction->fromValue = savedMsgData;
        }

        GameState* gs = g_GameState_0046aa30;
        int idx = gs->FindState("NUM_ACTIONS");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] += 0x14;
    }

    SC_Combat::ProcessLose();
}
