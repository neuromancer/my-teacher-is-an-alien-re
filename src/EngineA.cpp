#include "EngineA.h"
#include "GameState.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include "SpriteAction.h"
#include "string.h"


/* Function start: 0x43A4C0 */
EngineA::EngineA() {
}

/* Function start: 0x43A540 */
EngineA::~EngineA() {
}

/* Function start: 0x43A590 */
int EngineA::HandleAction(SpriteAction* param) {
    int result;
    int idx;
    GameState* gs;

    result = 0;

    if (param->instruction != 0x37) {
        ShowError("EngineNavigateDucts::OnLogicMessage - unsupported instruction %d", param->instruction);
    }

    switch (param->extra1) {
    case 1:
    {
        idx = (g_GameState_0046aa30)->FindState("DUCT_SWITCH");
        gs = g_GameState_0046aa30;

        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }

        if (gs->stateValues[idx] != 0) {
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
        } else {
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 1;
        }
        break;
    }
    case 2:
        SendGameMessage(1, param->mousePos.x, 0x41, 1, 4, 0, 0, 0, 0, 0);
        SendGameMessage(0x20, param->mousePos.y, 0x41, 1, 4, 0, 0, 0, 0, 0);
        result = 1;
        break;
    }

    return result;
}
