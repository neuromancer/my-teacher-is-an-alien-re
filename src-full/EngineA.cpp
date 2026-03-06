#include "EngineA.h"
#include "GameState.h"
#include <stdio.h>
#include <string.h>

extern void __cdecl FUN_00425c50(char*, ...);
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" void* DAT_0046aa30;

/* Function start: 0x43A4C0 */
EngineA::EngineA() {
}

/* Function start: 0x43A540 */
EngineA::~EngineA() {
}

/* Function start: 0x43A590 */
int EngineA::HandleAction(int* param) {
    int result;
    int idx;
    GameState* gs;

    result = 0;

    if (param[4] != 0x37) {
        FUN_00425c50("EngineNavigateDucts::OnLogicMessage - unsupported instruction %d", param[4]);
    }

    if (param[5] == 1) {
        idx = ((GameState*)DAT_0046aa30)->FindState("DUCT_SWITCH");
        gs = (GameState*)DAT_0046aa30;

        if (idx < 0 || gs->maxStates - 1 < idx) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        if (idx < 0 || gs->maxStates - 1 < idx) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }

        if (gs->stateValues[idx] != 0) {
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
        } else {
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 1;
        }
    } else if (param[5] == 2) {
        FUN_00444d90(1, param[7], 0x41, 1, 4, 0, 0, 0, 0, 0);
        FUN_00444d90(0x20, param[8], 0x41, 1, 4, 0, 0, 0, 0, 0);
        result = 1;
    }

    return result;
}
