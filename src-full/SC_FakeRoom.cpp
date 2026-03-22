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

extern InputManager* g_InputManager_0046aa08;
extern MouseControl* g_Mouse_0046aa18;
extern "C" extern GameState* g_GameState_0046aa30;

// ActionParser is SC_Message — vtable 0x461100, LBLParse at 0x444E60
// SC_Message::targetAddress (0x90) stores the SpriteAction* to fill

/* Function start: 0x4441E0 */
SC_FakeRoom::SC_FakeRoom()
{
    hitboxLeft = 0;
    hitboxTop = 0;
    hitboxRight = 0;
    hitboxBottom = 0;
    memset(&stateFlags, 0, 6 * 4);
    handlerId = 0x49;
}

/* Function start: 0x444380 */
int SC_FakeRoom::ShutDown(SC_Message* msg)
{
    SC_Combat::ShutDown(msg);
    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId,
                       moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 0;
}

/* Function start: 0x4443E0 */
int SC_FakeRoom::AddMessage(SC_Message* msg)
{
    if (SC_Combat::AddMessage(msg) != 0) {
        return 1;
    }

    SpriteAction* action = (SpriteAction*)msg;
    if (action->lastKey == 0x1b) {
        if (savedCommand == 0x2b) {
            statusPtr[2] = 1;
            return 1;
        }
    } else if (action->button1 > 1) {
        int mouseX = action->mousePos.field_0;
        if (stateFlags & 1) {
            if (mouseX < 100 || mouseX > 0x21c) {
                stateFlags = stateFlags & ~1;
                bgSprite->ResetAnimation(1, 0);
                return 1;
            }
        } else if (hitboxLeft <= mouseX && hitboxRight >= mouseX &&
                   hitboxTop <= action->mousePos.field_4 && hitboxBottom >= action->mousePos.field_4) {
            statusPtr[1] = 1;
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
    int done = bgSprite->Do(bgSprite->loc_x, bgSprite->loc_y, 1.0);
    if (done != 0) {
        *statusPtr = 1;
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

    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    int mouseY = 0;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }

    int mouseX;
    if (pMouse != 0) {
        mouseX = pMouse->x;
    } else {
        mouseX = 0;
    }

    if (hitboxLeft <= mouseX && mouseX <= hitboxRight &&
        hitboxTop <= mouseY && mouseY <= hitboxBottom) {
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
    switch (action) {
    case 0:
    case 1:
    case 2:
        ProcessLose();
        return;
    case 3:
        *data = 0;
        bgSound->Play(0);
        stateFlags = stateFlags | 1;
        return;
    case 4:
        RenderFrame();
        return;
    default:
        ShowError("SC_FakeRoom::Process_Action - invalid Action=%d, value=%d", action, *data);
        return;
    }
}

SC_FakeRoom::~SC_FakeRoom()
{
}

void SC_FakeRoom::OnProcessEnd() {}
void SC_FakeRoom::Init(SC_Message* msg) {}
void SC_FakeRoom::Update(int p1, int p2) {}
int SC_FakeRoom::Exit(SC_Message* msg) { return 0; }

/* Function start: 0x4444E0 */
void SC_FakeRoom::ProcessLose()
{
    if (*statusPtr != 0) {
        statusPtr[4] = 0;

        if (pendingAction != 0) {
            pendingAction->~SpriteAction();
            operator delete(pendingAction);
            pendingAction = 0;
        }

        pendingAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        SC_Message temp;
        temp.targetAddress = (int)pendingAction;
        ParseFile(&temp, "mis\\CB_FakeRoom.mis", "LOSE_MESSAGE");

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
