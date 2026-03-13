#include "SC_DodgeOrville.h"
#include "SpriteAction.h"
#include <string.h>
#include <stdlib.h>

extern void __fastcall FUN_0044c740(void*);
extern void __fastcall FUN_00444af0(void*);
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern void __fastcall FUN_004127c0(void*);
extern "C" void FUN_00413e10(void*, char*, char*, ...);
extern void __cdecl FUN_00425c50(char*, ...);

// Engine base class calls (thiscall via fastcall trick)
extern void __fastcall FUN_00449320(void*, int, int);  // Engine::VirtCleanup
extern int __fastcall FUN_00449400(void*, int, void*);  // Engine::CleanupSubsystems
extern void __fastcall FUN_004494e0(void*);              // Engine::ProcessTargets

extern "C" { extern void* DAT_0046aa30; }

// Parser-derived class used for MIS file action parsing
// vtable 0x461100 in original binary
class ActionParser : public Parser {
public:
    void* actionPtr;  // 0x90 - SpriteAction*
};

class GameStateObj {
public:
    int FindLabel(char* name);  // 0x433AE0
};

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
        FUN_0044c740(ptr);
        free(ptr);
        field_12C = 0;
    }

    ptr = (void*)field_130;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        free(ptr);
        field_130 = 0;
    }

    FUN_00449320(this, 0, flag);

    if (flag != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x428AF0 */
int SC_DodgeOrville::AddMessage(SC_Message* msg) {
    int ret;

    ret = FUN_00449400(this, 0, msg);
    if (ret != 0) {
        return 1;
    }
    if (*((int*)msg + 0xB) == 0x1B) {
        ((int*)field_A8[0])[2] = 1;
    }
    return 1;
}

/* Function start: 0x428B60 */
void SC_DodgeOrville::ProcessTargets() {
    void* ptr;

    if (savedCommand != 0x2B) {
        if (((int*)field_A8[0])[1] != 0) {
            ptr = (void*)field_A8[22];
            if (ptr != 0) {
                FUN_00444af0(ptr);
                free(ptr);
                field_A8[22] = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                field_A8[22] = (int)newAction;
                ActionParser temp;
                temp.actionPtr = (void*)newAction;
                FUN_00413e10(&temp, "mis\\cb_DOrville.mis", "[WIN_LBL]");

                *(int*)(field_A8[22] + 8) = savedCommand;
                *(int*)(field_A8[22] + 0xC) = savedMsgData;
                *(int*)(field_A8[22] + 0x14) = 6;

                void* gs = DAT_0046aa30;
                int idx = ((GameStateObj*)gs)->FindLabel("DODGE_COMBAT_AVAILABLE");
                if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                    FUN_00425c50("Invalid gamestate %d", idx);
                }
                *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = 0;

                gs = DAT_0046aa30;
                idx = ((GameStateObj*)gs)->FindLabel("NUM_ACTIONS");
                if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                    FUN_00425c50("Invalid gamestate %d", idx);
                }
                *(int*)(*(int*)((int)gs + 0x90) + idx * 4) += 0x1E;
            }
        }
        else if (((int*)field_A8[0])[0] != 0) {
            ptr = (void*)field_A8[22];
            if (ptr != 0) {
                FUN_00444af0(ptr);
                free(ptr);
                field_A8[22] = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                field_A8[22] = (int)newAction;
                ActionParser temp;
                temp.actionPtr = (void*)newAction;
                FUN_00413e10(&temp, "mis\\cb_DOrville.mis", "[LOSE_LBL]");
            }
        }
        else {
            ptr = (void*)field_A8[22];
            if (ptr != 0) {
                FUN_00444af0(ptr);
                free(ptr);
                field_A8[22] = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                field_A8[22] = (int)newAction;
                ActionParser temp;
                temp.actionPtr = (void*)newAction;
                FUN_00413e10(&temp, "mis\\cb_DOrville.mis", "[QUIT_LBL]");
            }
        }
    }
    else {
        if (((int*)field_A8[0])[1] != 0) {
            ptr = (void*)field_A8[22];
            if (ptr != 0) {
                FUN_00444af0(ptr);
                free(ptr);
                field_A8[22] = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                field_A8[22] = (int)newAction;
                ActionParser temp;
                temp.actionPtr = (void*)newAction;
                FUN_00413e10(&temp, "mis\\cb_DOrville.mis", "[WIN_LBL_PR]");
            }
        }
        else if (((int*)field_A8[0])[0] != 0) {
            ptr = (void*)field_A8[22];
            if (ptr != 0) {
                FUN_00444af0(ptr);
                free(ptr);
                field_A8[22] = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                field_A8[22] = (int)newAction;
                ActionParser temp;
                temp.actionPtr = (void*)newAction;
                FUN_00413e10(&temp, "mis\\cb_DOrville.mis", "[LOSE_LBL_PR]");
            }
        }
    }

    FUN_004494e0(this);
}
