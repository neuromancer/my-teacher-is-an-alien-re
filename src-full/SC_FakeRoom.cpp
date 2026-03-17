#include "SC_FakeRoom.h"
#include "SC_Question.h"
#include "SoundList.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "GameState.h"
#include "Memory.h"
#include <string.h>

#include "InputManager.h"
#include "MouseControl.h"
extern InputManager* DAT_0046aa08;
extern MouseControl* DAT_0046aa18;
extern "C" extern GameState* DAT_0046aa30;

extern void __fastcall FUN_0044cb40(void*, int, int, int);  // Sprite::ResetAnimation
extern void __fastcall FUN_004494e0(void*);                  // SC_Combat base ProcessLose tail

// Parser-derived class used for MIS file action parsing
// vtable 0x461100 in original binary
class ActionParser : public Parser {
public:
    SpriteAction* actionPtr;  // 0x90
};

/* Function start: 0x4441E0 */
SC_FakeRoom::SC_FakeRoom()
{
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
        if ((stateFlags & 1) == 0) {
            if (hitboxLeft <= mouseX && mouseX <= hitboxRight &&
                hitboxTop <= action->mousePos.field_4 && action->mousePos.field_4 <= hitboxBottom) {
                statusPtr[1] = 1;
            }
        } else if (mouseX < 100 || mouseX > 0x21c) {
            stateFlags = stateFlags & ~1;
            FUN_0044cb40(bgSprite, 0, 1, 0);
            return 1;
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
    int done = bgSprite->Do(bgSprite->num_states, bgSprite->field_0xb0, 1.0);
    if (done != 0) {
        *statusPtr = 1;
    }

    InputState* pMouse = DAT_0046aa08->pMouse;

    if ((*(char*)&stateFlags & 1) != 0) {
        int state;
        if (pMouse == 0 || pMouse->x < 100) {
            state = 4;
        } else if (pMouse->x > 0x21c) {
            state = 3;
        } else {
            state = 0;
        }
        Sprite* cursor = DAT_0046aa18->m_sprite;
        if (cursor != 0) {
            FUN_0044cb40(cursor, 0, state, 0);
        }
        DAT_0046aa18->DrawCursor();
        return;
    }

    int mouseX = 0;
    int mouseY;
    if (pMouse == 0) {
        mouseY = 0;
    } else {
        mouseX = pMouse->y;
        mouseY = pMouse->x;
    }

    int state;
    if (mouseY < hitboxLeft || hitboxRight < mouseY ||
        mouseX < hitboxTop || hitboxBottom < mouseX) {
        state = 0;
    } else {
        state = 0x14;
    }
    Sprite* cursor = DAT_0046aa18->m_sprite;
    if (cursor != 0) {
        FUN_0044cb40(cursor, 0, state, 0);
    }
    DAT_0046aa18->DrawCursor();
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

        ActionParser temp;
        temp.actionPtr = pendingAction;
        ParseFile(&temp, "mis\\CB_FakeRoom.mis", "LOSE_MESSAGE");

        if (savedCommand == 0x2b) {
            pendingAction->fromType = 0x2b;
            pendingAction->fromValue = savedMsgData;
        }

        GameState* gs = DAT_0046aa30;
        int idx = gs->FindState("NUM_ACTIONS");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] += 0x14;
    }

    FUN_004494e0(this);
}
