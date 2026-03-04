#include "SC_Rats.h"
#include "Memory.h"
#include "Sample.h"
#include "GameState.h"
#include <string.h>
#include <stdio.h>

// Globals for SC_Rats state machine
int DAT_00473df8 = 0;
int DAT_00473e00 = 0;
int DAT_00473e14 = 0;
int DAT_00473e18 = 0;
int DAT_00473e1c = 0;
int DAT_00473e20 = 0;
int DAT_0046ae78 = 0;
int DAT_0046ae70 = 0;

// Extern functions
extern "C" int FUN_00425fa0(void*);
extern "C" void FUN_004265a0();
extern void __fastcall FUN_0042bf00(void*);
extern void __fastcall FUN_0042be00(void*);
extern void __fastcall FUN_004252a0(void*);
extern void __fastcall FUN_00444af0(void*);
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
// SpriteAction - 0x38-byte object, constructor is FUN_00444a40 (thiscall)
class SpriteAction {
    int field_0[14]; // 0x38 bytes
public:
    void* operator new(size_t s) { return AllocateMemory(s); }
    void operator delete(void* p) { FreeMemory(p); }
    SpriteAction(int, int, int, int, int, int, int, int, int, int);
};
extern void __cdecl FUN_00444e40(void*);
extern void __cdecl FUN_00425c50(char*, ...);
extern int __fastcall FUN_00433ae0(void*, int, char*);
extern void __cdecl FUN_00413e70(void*, int, char*);
extern void* __fastcall FUN_00450b10(void*);
extern void __fastcall FUN_0044bac0(void*, int, int, int);
// FUN_00412a50 is Parser::ReportUnknownLabel (thiscall, declared in Parser.h)
extern void __fastcall FUN_0040b760(void*, int, int);
extern void __fastcall FUN_00404d70(void*, int, int);
extern void __fastcall FUN_00401130(void*);
extern void* __fastcall FUN_00403520(void*);
extern void* __fastcall FUN_004035a0(void*);

// IAT entries
extern int (__stdcall *DAT_0047652c)(int);

// Scene data globals
extern void* DAT_0046aa24;
extern "C" void* DAT_0046aa30;

/* Function start: 0x451700 */
SC_Rats::SC_Rats() {
    field_A8 = 0;
    field_AC = 0;
    handlerId = 0x42;
}

/* Function start: 0x4517C0 */
SC_Rats::~SC_Rats() {
    ShutDown(0);
}

/* Function start: 0x451820 */
void SC_Rats::Init(SC_Message* msg) {
    field_A8 = 0;
    field_AC = 0;
    DAT_00473df8 = 0;
    DAT_00473e1c = 0;
    DAT_00473e20 = 0;
    DAT_00473e00 = 0;
    DAT_00473e14 = 0;
    DAT_00473e18 = 0;
    CopyCommandData(msg);
    moduleParam = ((int*)msg)[1];
    if (!FUN_00425fa0("CB_Rats")) {
        FUN_004265a0();
    }
    int sceneData = (int)DAT_0046aa24;
    if (*(int*)(sceneData + 0x98) != 1) {
        *(int*)(sceneData + 0x98) = 1;
        int* list1 = *(int**)(sceneData + 0xA0);
        if (*list1 != 0) {
            list1[2] = *list1;
            while (*list1 != 0) {
                int* item = (int*)FUN_00403520(list1);
                if (item != 0) {
                    *item = 0x461030;
                    FreeMemory(item);
                }
            }
        }
        int* list2 = *(int**)(sceneData + 0xA4);
        if (*list2 != 0) {
            list2[2] = *list2;
            while (*list2 != 0) {
                int* item = (int*)FUN_004035a0(list2);
                if (item != 0) {
                    if (*(int*)((int)item + 4) != 0) {
                        FUN_0040b760((void*)*(int*)((int)item + 4), 0, 1);
                        *(int*)((int)item + 4) = 0;
                    }
                    if (*(int*)((int)item + 8) != 0) {
                        void* sub = (void*)*(int*)((int)item + 8);
                        (*(void (__fastcall **)(void*, int, int))(*(int*)sub))(sub, 0, 1);
                        *(int*)((int)item + 8) = 0;
                    }
                    FreeMemory(item);
                }
            }
        }
        int* list3 = *(int**)(sceneData + 0x9C);
        if (*list3 != 0) {
            list3[2] = *list3;
            while (*list3 != 0) {
                int* current = (int*)list3[2];
                int* removed;
                if (current == 0) {
                    removed = 0;
                } else {
                    if (*list3 == (int)current) {
                        *list3 = current[1];
                    }
                    if (list3[1] == (int)current) {
                        list3[1] = *current;
                    }
                    if (*current != 0) {
                        ((int*)*current)[1] = current[1];
                    }
                    if (current[1] != 0) {
                        *(int*)current[1] = *current;
                    }
                    if (current != 0) {
                        removed = (int*)current[2];
                        FUN_00404d70((void*)current, 0, 1);
                        list3[2] = 0;
                    } else {
                        removed = 0;
                    }
                    list3[2] = *list3;
                }
                if (removed != 0) {
                    *removed = 0x46102C;
                    FUN_00401130((void*)((int)removed + 4));
                    FreeMemory(removed);
                }
            }
        }
        *(int*)(sceneData + 0xA8) = 0;
    }
    ParseFile(this, "mis\\cb_rats.mis", (char*)0);
    FUN_0042be00((void*)DAT_0046ae78);
    if (field_A8 == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = (int)sprite;
        ((int*)sprite)[5] = ((int*)msg)[5];
        *(int*)(field_A8 + 0x1C) = ((int*)msg)[7];
        *(int*)(field_A8 + 0x20) = ((int*)msg)[8];
    }
}

/* Function start: 0x451B30 */
int SC_Rats::ShutDown(SC_Message* msg) {
    if (field_AC != 0) {
        FUN_0042bf00((void*)field_AC);
        if (field_AC != 0) {
            delete (Handler*)field_AC;
            field_AC = 0;
        }
        DAT_0046ae78 = 0;
    }
    if (field_A8 != 0) {
        FUN_00444af0((void*)field_A8);
        FreeMemory((void*)field_A8);
        field_A8 = 0;
    }
    if (msg != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 0;
}

/* Function start: 0x451BC0 */
void SC_Rats::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }
    switch (DAT_00473e18) {
        case 0:
            State0Handler();
            break;
        case 1:
            State1Handler();
            break;
        case 2:
            State2Handler();
            break;
        case 3:
            State3Handler();
            break;
        case 4:
            State4Handler();
            break;
    }
}

/* Function start: 0x451C20 */
int SC_Rats::AddMessage(SC_Message* msg) {
    ((int*)msg)[2] = handlerId;
    ((int*)msg)[3] = moduleParam;
    ((int*)msg)[4] = 0;
    if (((int*)msg)[11] == 0x1B) {
        DAT_00473e18 = 4;
    }
    return 1;
}

/* Function start: 0x451C60 */
int SC_Rats::Exit(SC_Message* msg) {
    if (handlerId != ((int*)msg)[0]) {
        return 0;
    }
    if (((int*)msg)[4] == 0) {
        return 1;
    }
    if (((int*)msg)[4] != 0x17) {
        return 0;
    }
    FUN_00425c50("SCMI_INSERT");
    return 1;
}

/* Function start: 0x451CA0 */
void SC_Rats::ProcessState() {
    int* sceneData;
    int idx;

    if (savedCommand == 0x2B) {
        if (DAT_00473e18 == 2) {
            if (field_A8 != 0) {
                FUN_00444af0((void*)field_A8);
                FreeMemory((void*)field_A8);
                field_A8 = 0;
            }
            field_A8 = (int)new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Parser temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_WIN_LBL_PR_");
        } else if (DAT_00473e18 == 3) {
            if (field_A8 != 0) {
                FUN_00444af0((void*)field_A8);
                FreeMemory((void*)field_A8);
                field_A8 = 0;
            }
            field_A8 = (int)new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Parser temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_LOSE_LBL_PR_");
        }
    } else {
        if (DAT_00473e18 == 4) {
            int* spriteData = (int*)field_A8;
            spriteData[8] = 2 - spriteData[8];
            GameState* gs = (GameState*)DAT_0046aa30;
            idx = FUN_00433ae0(gs, 0, "NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x14;
        } else if (DAT_00473e18 == 3) {
            if (field_A8 != 0) {
                FUN_00444af0((void*)field_A8);
                FreeMemory((void*)field_A8);
                field_A8 = 0;
            }
            field_A8 = (int)new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Parser temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_LOSE_LBL_");
        } else if (DAT_00473e18 == 2) {
            GameState* gs = (GameState*)DAT_0046aa30;
            idx = FUN_00433ae0(gs, 0, "RAT_COMBAT_AVAILABLE");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            gs = (GameState*)DAT_0046aa30;
            idx = FUN_00433ae0(gs, 0, "NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x1E;
        }
    }
    FUN_00444e40((void*)field_A8);
    if (field_A8 != 0) {
        FUN_00444af0((void*)field_A8);
        FreeMemory((void*)field_A8);
        field_A8 = 0;
    }
}

/* Function start: 0x4520E0 */
void SC_Rats::State0Handler() {
    int engineObj = DAT_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x118);

    State1Handler();

    if (DAT_00473e14 == 0) {
        DAT_00473e14 = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            FUN_004252a0((void*)*(int*)(engineObj + 0x100));
        }
        FUN_00444d90(5, 0x3E9, handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (DAT_00473e14 != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (DAT_0047652c((int)snd->m_sample) == 4) goto done;
        }
    }
    DAT_00473e14++;

done:
    if (DAT_00473e14 == 2) {
        DAT_00473e14 = 3;
        DAT_00473e18 = 1;
        FUN_0044bac0((void*)DAT_0046ae70, 0, 1, 0);
    }
}

/* Function start: 0x4521B0 */
void SC_Rats::State1Handler() {
    void* obj = (void*)DAT_0046ae78;
    (*(void (__fastcall **)(void*))(*(int*)obj + 0x38))(obj);
}

/* Function start: 0x4521C0 */
void SC_Rats::State2Handler() {
    int engineObj = DAT_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x110);

    if (DAT_00473e20 == 0) {
        DAT_00473e20 = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            FUN_004252a0((void*)*(int*)(engineObj + 0x100));
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (DAT_00473e20 != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (DAT_0047652c((int)snd->m_sample) == 4) goto done;
        }
    }
    DAT_00473e20++;

done:
    if (DAT_00473e20 == 2) {
        DAT_00473e20 = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x452250 */
void SC_Rats::State3Handler() {
    int engineObj = DAT_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x10C);

    if (DAT_00473e1c == 0) {
        DAT_00473e1c = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            FUN_004252a0((void*)*(int*)(engineObj + 0x100));
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (DAT_00473e1c != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (DAT_0047652c((int)snd->m_sample) == 4) goto done;
        }
    }
    DAT_00473e1c++;

done:
    if (DAT_00473e1c == 2) {
        DAT_00473e1c = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x4522E0 */
void SC_Rats::State4Handler() {
    int engineObj = DAT_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x114);

    if (DAT_00473df8 == 0) {
        DAT_00473df8 = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            FUN_004252a0((void*)*(int*)(engineObj + 0x100));
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (DAT_00473df8 != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (DAT_0047652c((int)snd->m_sample) == 4) goto done;
        }
    }
    DAT_00473df8++;

done:
    if (DAT_00473df8 == 2) {
        DAT_00473df8 = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x452370 */
int SC_Rats::LBLParse(char* param) {
    char buf[32];
    buf[0] = 0;
    sscanf(param, "%s", buf);
    if (strcmp(buf, "DERIVED_ENGINE_INFO") == 0) {
        void* mem = AllocateMemory(0x178);
        void* obj = 0;
        if (mem != 0) {
            obj = FUN_00450b10(mem);
        }
        field_AC = (int)obj;
        DAT_0046ae78 = (int)obj;
        FUN_00413e70(obj, (int)this, (char*)0);
        return 0;
    }
    if (strcmp(buf, "CB") == 0) {
        return 1;
    }
    ReportUnknownLabel("SC_Rats");
    return 0;
}
