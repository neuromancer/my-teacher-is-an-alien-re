#include "SC_Wahoo.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include "SoundList.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>

extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" void FUN_00454400(void*);
extern "C" void* FUN_00454500(int);

extern void* __fastcall FUN_0041dbe0(void*);
extern void* __fastcall FUN_0044c660(void*, int, char*);
extern void __fastcall FUN_0041dc10(void*);
extern void __fastcall FUN_0044c740(void*);
extern void __fastcall FUN_00410fd0(void*);
// FUN_00421930 = TimeOut::~TimeOut (full game)
extern void __fastcall FUN_00424ee0(void*);
extern void __fastcall FUN_00425200(void*);

extern void __cdecl FUN_00425c50(char*, ...);
extern void __cdecl FUN_00412a50();
extern char* __cdecl FUN_00426190(char* name);
extern char* __cdecl FUN_0044e470(char*);

extern void* __fastcall FUN_00410fb0(void*, int, char*, int);
extern void __fastcall FUN_004274c0(void*, int, int);

class PaletteObj {
public:
    void Load(char* filename);  // 0x41DCC0
};

// FUN_00426ce0 is thiscall with 1 param (not fastcall)
// Use local class wrapper for correct calling convention
class InputObj {
public:
    void Refresh(int param);  // 0x426CE0
};

// TimerObj = TimeOut (ctor 0x421920 wraps 0x421960, dtor 0x421930)
// Check() = IsTimeOut() @ 0x421A30, Set() = Start() @ 0x4219F0
#include "TimeOut.h"

class DetectionObj {
public:
    void Render();            // 0x427880
};

class DetMask {
public:
    unsigned int CheckHit(int x, int y);  // 0x411330
};

extern int (__stdcall *DAT_0047652c)(int);

extern int __fastcall FUN_00433ae0(void*, int, char*);
extern void __fastcall FUN_00409f20(void*, int);
extern void __cdecl FUN_00444e40(void*);

extern void __cdecl FUN_00413e70(void*, int, char*);
extern "C" void FUN_00413e10(void*, char*, char*, ...);

extern void __fastcall FUN_004309a0(void*, int, int);
extern "C" int FUN_00425fa0(char*);
extern "C" void FUN_004265a0();
extern void __cdecl FUN_00425a90(int, int);
extern void __cdecl FUN_00425d70(char*);
extern void* DAT_0046aa24;


extern void* DAT_0046bbfc;
extern void* DAT_0046aa08;
extern void* DAT_0046aa18;
extern "C" extern void* DAT_0046aa30;

/* Function start: 0x437830 */
SC_Wahoo::SC_Wahoo() {
    field_CC = 0;
    field_D0 = 0;
    field_D4 = 0;
    field_D8 = 0;
    field_134 = 0;
    field_138 = 0;
    field_144 = 0;
    field_148 = 0;
    field_14C = 0;
    field_150 = 0;
    field_154 = 0;
    field_158 = 0;
    field_15C = 0;
    field_160 = 0;
    memset(&field_A8, 0, 0xC0);
    handlerId = 0x3D;
}

/* Function start: 0x437980 */
SC_Wahoo::~SC_Wahoo() {
    ShutDown(0);
}

/* Function start: 0x437A40 */
void SC_Wahoo::Init(SC_Message* msg) {
    int uVar1 = *(int*)((int)this + 0x90);
    int* puVar6 = (int*)((int)this + 0xa8);
    int iVar7;
    for (iVar7 = 0x30; iVar7 != 0; iVar7--) {
        *puVar6 = 0;
        puVar6++;
    }
    *(int*)((int)this + 0x90) = uVar1;

    FUN_004309a0(this, 0, (int)msg);

    *(int*)((int)this + 0x164) = 0;

    if (!FUN_00425fa0("CB_Bridge")) {
        FUN_004265a0();
    }

    *(int*)((int)this + 0x134) = 0x140;
    *(int*)((int)this + 0x138) = 0xf0;
    FUN_00425a90(0x140, 0xf0);

    *(int*)((int)this + 0x94) = *(int*)((int)msg + 4);

    void* pvVar4 = DAT_0046aa30;
    unsigned int uVar3 = FUN_00433ae0(pvVar4, 0, "PLAY_RIGHT_BRIDGE");
    if ((int)uVar3 < 0 || *(int*)((int)pvVar4 + 0x98) - 1 < (int)uVar3) {
        FUN_00425c50("Invalid gamestate %d", uVar3);
    }
    field_140 = *(int*)(*(int*)((int)pvVar4 + 0x90) + uVar3 * 4);

    FUN_00413e10(this, "mis\\cb_bridge.mis", (char*)0);

    pvVar4 = DAT_0046aa30;
    uVar3 = FUN_00433ae0(pvVar4, 0, "PLAY_RIGHT_BRIDGE");
    if ((int)uVar3 < 0 || *(int*)((int)pvVar4 + 0x98) - 1 < (int)uVar3) {
        FUN_00425c50("Invalid gamestate %d", uVar3);
    }
    field_140 = *(int*)(*(int*)((int)pvVar4 + 0x90) + uVar3 * 4);

    if (field_A8 == 0) {
        field_A8 = new SpriteAction(
            *(int*)((int)this + 0x98),
            *(int*)((int)this + 0x9c),
            *(int*)((int)this + 0x90),
            *(int*)((int)this + 0x94),
            4, 0, 0, 0, 0, 0
        );
    }

    iVar7 = (int)field_AC;
    if (iVar7 != 0) {
        int* piVar8 = (int*)((int)DAT_0046aa24 + 0xa8);
        if (*piVar8 != 0) {
            FUN_00425d70("ddouble palette");
        }
        *piVar8 = iVar7;
    }

    field_C8 = new TimeOut();
}

/* Function start: 0x437C60 */
int SC_Wahoo::ShutDown(SC_Message* msg) {
    void* ptr;
    int i;

    ptr = field_AC;
    if (ptr != 0) {
        FUN_0041dc10(ptr);
        FUN_00454400(ptr);
        field_AC = 0;
    }

    ptr = field_B0;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_B0 = 0;
    }

    ptr = field_B4;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_B4 = 0;
    }

    ptr = field_C4;
    if (ptr != 0) {
        FUN_00410fd0(ptr);
        FUN_00454400(ptr);
        field_C4 = 0;
    }

    ptr = field_B8;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_B8 = 0;
    }

    ptr = field_BC;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_BC = 0;
    }

    ptr = field_C0;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_C0 = 0;
    }

    if (DAT_0046bbfc != 0) {
        (*(void (**)(int))(*((int*)DAT_0046bbfc) + 0xC))(1);
        DAT_0046bbfc = 0;
    }

    ptr = field_13C;
    if (ptr != 0) {
        FUN_0044c740(ptr);
        FUN_00454400(ptr);
        field_13C = 0;
    }

    ptr = field_C8;
    if (ptr != 0) {
        delete (TimeOut*)ptr;
        field_C8 = 0;
    }

    ptr = field_A8;
    if (ptr != 0) {
        delete (SpriteAction*)ptr;
        field_A8 = 0;
    }

    ptr = field_114;
    if (ptr != 0) {
        FUN_00425200(ptr);
        FUN_00454400(ptr);
        field_114 = 0;
    }

    field_130 = 0;
    field_12C = 0;
    field_128 = 0;
    field_124 = 0;
    field_120 = 0;
    field_11C = 0;

    for (i = 0xe; i != 0; i--) {
        ptr = field_DC[0xe - i];
        if (ptr != 0) {
            FUN_00424ee0(ptr);
            FUN_00454400(ptr);
            field_DC[0xe - i] = 0;
        }
    }

    if (msg != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x4381E0 */
int SC_Wahoo::AddMessage(SC_Message* msg) {
    int* m = (int*)msg;

    if (m[0xB] == 0x1b) {
        m[2] = handlerId;
        m[3] = moduleParam;
        m[4] = 0;
        ProcessState();
        return 1;
    }

    m[2] = handlerId;
    m[3] = moduleParam;
    m[4] = 0;

    if (m[9] >= 1) {
        if (m[7] < field_CC || field_D4 < m[7] ||
            m[8] < field_D0 || field_D8 < m[8]) {
            *(int*)((int)DAT_0046bbfc + 0xB8) = *(int*)((int)DAT_0046bbfc + 0xB8) & 0xFFFFFFFE;
        } else {
            *(int*)((int)DAT_0046bbfc + 0xB8) = *(int*)((int)DAT_0046bbfc + 0xB8) | 1;
            if (m[9] > 1) {
                field_164 = field_164 | 2;
                ProcessState();
                return 1;
            }
        }
    }

    return 1;
}

/* Function start: 0x438280 */
int SC_Wahoo::Exit(SC_Message* msg) {
    int* m = (int*)msg;
    int id = handlerId;

    if (m[0] != id) {
        return 0;
    }

    switch (m[4]) {
    case 0:
        break;
    case 7:
        FUN_00444d90(1, id, id, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        FUN_00425c50("SCMI_INSERT");
        break;
    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x438470 */
void SC_Wahoo::OnProcessEnd() {
    int iVar1;

    field_CC = 0;
    field_D0 = 200;
    field_D4 = 0x140;
    field_D8 = 0xF0;

    if (field_B4 != 0) {
        ((Sprite*)field_B4)->ResetAnimation(0, 0);
    }
    ((Sprite*)field_B8)->ResetAnimation(0, 0);
    ((Sprite*)field_C0)->ResetAnimation(4, 0);

    if (field_B4 != 0) {
        ((Sprite*)field_B4)->ResetAnimation(1, 0);
    }
    ((Sprite*)field_B8)->ResetAnimation(3, 0);
    ((Sprite*)field_BC)->ResetAnimation(4, 0);
    ((Sprite*)field_C0)->ResetAnimation(1, 0);

    if (field_140 != 0) {
        field_144 = 0x116;
        field_148 = 0x40;
        field_14C = 0x121;
        field_150 = 0x4B;
        field_154 = 0x11;
        field_158 = 0x15;
        field_15C = 0x79;
    } else {
        field_144 = 0x1F;
        field_148 = 0x40;
        field_14C = 0x2A;
        field_150 = 0x4B;
        field_154 = 0xBB;
        field_158 = 0x15;
        field_15C = 0x12F;
    }

    field_160 = 0x91;

    if (DAT_0046aa08 != 0) {
        ((InputObj*)DAT_0046aa08)->Refresh(1);
    }

    if (field_13C != 0) {
        iVar1 = 0;
        if (*(int*)((int)DAT_0046aa08 + 0x1A0) != 0) {
            iVar1 = **(int**)((int)DAT_0046aa08 + 0x1A0);
        }
        ((Sprite*)field_13C)->ResetAnimation(iVar1 / (field_134 / 5), 0);
    }

    if (field_128 != 0) {
        ((Sample*)field_128)->Play(100, 0);
    }

    if (field_DC[9] != 0) {
        ((Sample*)field_DC[9])->Play(100, 1);
    }
}

/* Function start: 0x437EA0 */
void SC_Wahoo::Update(int param1, int param2) {
    Sprite* spr;
    int* cursorData;
    int cursorX;
    int cursorY;
    int frame;

    if (handlerId != param2) {
        return;
    }

    if (*(int*)((int)field_C8) == 1) {
        if (((TimeOut*)field_C8)->IsTimeOut() == 0) {
            return;
        }
        int val = field_140;
        void* gs = DAT_0046aa30;
        int idx = FUN_00433ae0(gs, 0, "COMBAT_BRIDGE_STATE");
        FUN_00409f20(gs, idx);
        *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = val + 1;
        ProcessState();
        return;
    }

    spr = (Sprite*)field_B0;
    if ((*(char*)((int)&field_164) & 1) != 0) {
        if (((Sprite*)spr)->Do(spr->loc_x, spr->loc_y, 1.0) != 0) {
            ((TimeOut*)field_C8)->Start(2000);
        }
        spr = (Sprite*)field_B8;
        if (spr != 0) {
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        spr = (Sprite*)field_BC;
        if (spr != 0) {
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        spr = (Sprite*)field_C0;
        if (spr == 0) {
            return;
        }
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
        return;
    }

    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)field_B4;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)field_B8;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)field_BC;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)field_C0;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }

    cursorY = 0;
    cursorData = *(int**)((int)DAT_0046aa08 + 0x1A0);
    if (cursorData != 0) {
        cursorY = cursorData[1];
    }
    cursorX = 0;
    if (cursorData != 0) {
        cursorX = cursorData[0];
    }

    if (cursorX < field_CC || field_D4 < cursorX ||
        cursorY < field_D0 || field_D8 < cursorY) {
        spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
        if (spr == 0) goto label_done;
        frame = 0xe;
    } else if (cursorData == 0 || cursorData[0] < 0xa0) {
        spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
        if (spr == 0) goto label_done;
        frame = 0xa;
    } else {
        spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
        if (spr == 0) goto label_done;
        frame = 0xb;
    }
    spr->ResetAnimation(frame, 0);

label_done:
    if (DAT_0046bbfc == 0) return;
    if (field_13C == 0) return;

    ((DetectionObj*)DAT_0046bbfc)->Render();

    if (*(int*)((int)DAT_0046bbfc + 0xA8) != 0) {
        cursorX = 0;
        if (*(int**)((int)DAT_0046aa08 + 0x1A0) != 0) {
            cursorX = **(int**)((int)DAT_0046aa08 + 0x1A0);
        }
        ((Sprite*)field_13C)->ResetAnimation(cursorX / (field_134 / 3) + 5, 0);
    }

    spr = (Sprite*)field_13C;
    if (spr->Do(spr->loc_x, spr->loc_y, 1.0) != 0) {
        cursorX = 0;
        if (*(int**)((int)DAT_0046aa08 + 0x1A0) != 0) {
            cursorX = **(int**)((int)DAT_0046aa08 + 0x1A0);
        }
        ((Sprite*)field_13C)->ResetAnimation(cursorX / (field_134 / 5), 0);
    }
}

/* Function start: 0x4382F0 */
void SC_Wahoo::ProcessState() {
    void* sprAction;

    if (savedCommand == 0x2B && (*(char*)(&field_164) & 1) != 0) {
        sprAction = field_A8;
        if (sprAction != 0) {
            delete (SpriteAction*)sprAction;
            field_A8 = 0;
        }
        field_A8 = (void*)new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        {
            Parser temp;
            FUN_00413e10(&temp, "mis\\cb_bridge.mis", "_WIN_LBL_PR_");
        }
    }

    FUN_00444e40(field_A8);
    sprAction = field_A8;
    if (sprAction != 0) {
        delete (SpriteAction*)sprAction;
        field_A8 = 0;
    }
}

/* Function start: 0x438630 */
int SC_Wahoo::LBLParse(char* param_1) {
    char local_b8[128];
    char local_38[32];
    int local_18;

    local_18 = 0;
    local_b8[0] = 0;
    local_38[0] = 0;
    sscanf(param_1, "%s", local_38);

    if (strcmp(local_38, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        void* mem = FUN_00454500(8);
        void* pal = 0;
        if (mem != 0) {
            pal = FUN_0041dbe0(mem);
        }
        field_AC = pal;
        ((PaletteObj*)pal)->Load(local_b8);
    }
    else if (strcmp(local_38, "DETECTION_MASK") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        void* mem = FUN_00454500(0x30);
        void* obj = 0;
        if (mem != 0) {
            char* path = FUN_00426190(local_b8);
            obj = FUN_00410fb0(mem, 0, path, 0);
        }
        field_C4 = obj;
    }
    else if (strcmp(local_38, "BACKGROUND_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_B0 = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "RESET_SWITCH_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_B4 = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "INNER_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_B8 = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "MIDDLE_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_BC = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "OUTER_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_C0 = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "CONSOLE_SPRITE") == 0) {
        void* mem = FUN_00454500(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = FUN_0044c660(mem, 0, (char*)0);
        }
        field_13C = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    }
    else if (strcmp(local_38, "SET_GAMESTATE") == 0) {
        sscanf(param_1, " %s %s %d ", local_38, local_b8, &local_18);
        int val = local_18;
        void* gs = DAT_0046aa30;
        unsigned int idx = FUN_00433ae0(gs, 0, local_b8);
        if ((int)idx < 0 || *(int*)((int)gs + 0x98) - 1 < (int)idx) {
            FUN_00425c50("Invalid gamestate %d");
        }
        *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = val;
    }
    else if (strcmp(local_38, "SOUND") == 0) {
        sscanf(param_1, " %s %d %s ", local_38, &local_18, local_b8);
        if (field_114 == 0) {
            field_114 = (void*)new SoundList(0xf);
        }
        if (local_18 == 0) {
            int iVar3 = atoi(local_b8);
            FUN_00444d90(5, iVar3, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        }
        else if (local_18 >= 1 && local_18 <= 6) {
            char* path = FUN_0044e470(local_b8);
            void* snd = ((SoundList*)field_114)->Register(path);
            if (local_18 == 1) {
                field_11C = (int)snd;
            }
            else if (local_18 == 2) {
                field_120 = snd;
            }
            else if (local_18 == 3) {
                field_124 = snd;
            }
            else if (local_18 == 4) {
                field_128 = snd;
            }
            else if (local_18 == 5) {
                field_12C = (int)snd;
            }
            else if (local_18 == 6) {
                field_130 = (int)snd;
            }
        }
        else if (local_18 >= 7 && local_18 <= 0xd) {
            Sample* smp = new Sample();
            field_DC[local_18] = (void*)smp;
            char* path = FUN_0044e470(local_b8);
            int err = smp->Load(path);
            if (err != 0 && field_DC[local_18] != 0) {
                FUN_00424ee0(field_DC[local_18]);
                FUN_00454400(field_DC[local_18]);
                field_DC[local_18] = 0;
            }
        }
        else {
            FUN_00412a50();
        }
    }
    else if (strcmp(local_38, "WEAPON") == 0) {
        int ret = sscanf(param_1, " %s %s ", local_38, local_b8);
        if (ret == 2) {
            if (strcmp(local_b8, "ROCKTHROWER2") == 0) {
                void* mem = FUN_00454500(200);
                if (mem == 0) {
                    DAT_0046bbfc = 0;
                } else {
                    FUN_004274c0(mem, 0, (int)this);
                    DAT_0046bbfc = mem;
                }
            }
        }
    }
    else if (strcmp(local_38, "END") == 0) {
        return 1;
    }
    else {
        FUN_00412a50();
    }

    return 0;
}

/* Function start: 0x438F10 */
int SC_Wahoo::FUN_438F10(int param_1) {
    void* pvVar6;
    int iVar3;
    int iVar5;
    int iVar7;
    int* piVar4;
    int* piVar8;
    int uVar10;
    int local_c[3];

    iVar7 = *(int*)(*(int*)(param_1 + 0x98) * 0x10 + 4 + *(int*)(param_1 + 0x90)) - 1;
    if (*(int*)(param_1 + 0xf0) == 0) {
        if (iVar7 != 0) {
            return 0;
        }
    }
    else if (*(int*)(*(int*)(*(int*)(param_1 + 0xf0) + 0xc) + 0x374) != iVar7) {
        return 0;
    }

    iVar7 = *(int*)(param_1 + 0x120);
    int bVar1;
    if (iVar7 < field_144 || field_14C < iVar7 ||
        *(int*)(param_1 + 0x124) < field_148 || field_150 < *(int*)(param_1 + 0x124)) {
        bVar1 = 0;
    } else {
        bVar1 = 1;
    }

    if (!bVar1) {
        if (iVar7 < field_154 || field_15C < iVar7 ||
            *(int*)(param_1 + 0x124) < field_158 || field_160 < *(int*)(param_1 + 0x124)) {
            bVar1 = 0;
        } else {
            bVar1 = 1;
        }
        if (!bVar1) goto LAB_004392b0;

        {
        unsigned int uVar2 = ((DetMask*)field_C4)->CheckHit(
            iVar7 - field_154,
            *(int*)(param_1 + 0x124) - field_158);

        if ((int)uVar2 < 1 || 3 < (int)uVar2 ||
            (pvVar6 = *(void**)((int)this + uVar2 * 4 + 0xb4),
             *(int*)((int)pvVar6 + 0x98) != 0)) {
            if (field_120 != 0) {
                ((Sample*)field_120)->Play(100, 1);
            }
            return 1;
        }

        if (*(int*)((int)pvVar6 + 0xf0) == 0) {
            iVar7 = 0;
        } else {
            iVar7 = *(int*)(*(int*)(*(int*)((int)pvVar6 + 0xf0) + 0xc) + 0x374);
        }

        if (uVar2 != 2) {
            iVar3 = (iVar7 - 1) / 4 + 1;
        } else {
            if (iVar7 >= 0 && iVar7 <= 3) {
                iVar3 = 1;
            } else if (iVar7 >= 4 && iVar7 <= 9) {
                iVar3 = 4;
            } else if (iVar7 >= 8 && iVar7 <= 0xb) {
                iVar3 = 3;
            } else {
                iVar3 = 2;
            }
        }

        ((Sprite*)pvVar6)->ResetAnimation(iVar3, 0);

        if (*(void**)((int)this + 0x12c) != 0) {
            ((Sample*)*(void**)((int)this + 0x12c))->Play(100, 1);
        }

        iVar3 = 0;
        piVar8 = local_c;
        piVar4 = (int*)((int)this + 0xb8);
        iVar7 = 3;
        do {
            if (*(int*)(*piVar4 + 0x98) != 0) {
                *piVar8 = *(int*)(*piVar4 + 0x98);
                piVar8 = piVar8 + 1;
                iVar3 = iVar3 + 1;
            }
            piVar4 = piVar4 + 1;
            iVar7 = iVar7 - 1;
        } while (iVar7 != 0);

        if (iVar3 == 3 && (iVar7 = (int)field_124, iVar7 != 0) &&
            (iVar5 = *(int*)(iVar7 + 0xc), iVar5 != 0) &&
            *(int*)(iVar5 + 0xc) == *(int*)(iVar7 + 4)) {
            iVar7 = DAT_0047652c(iVar5);
            if (iVar7 == 4) {
                ((Sample*)field_124)->Stop();
            }
        }

        iVar7 = 1;
        if (1 < iVar3) {
            iVar5 = iVar3 - 1;
            piVar4 = local_c;
            do {
                if (*piVar4 == piVar4[1]) {
                    iVar7 = iVar7 + 1;
                }
                iVar5 = iVar5 - 1;
                piVar4 = piVar4 + 1;
            } while (iVar5 != 0);
        }

        if (*(int*)((int)field_B4 + 0x98) == 0 && 1 < iVar3) {
            if (iVar7 == 3) {
                *(unsigned int*)((int)this + 0x164) = *(unsigned int*)((int)this + 0x164) | 1;
                ((Sprite*)field_B0)->ResetAnimation(1, 0);
                pvVar6 = *(void**)((int)this + 0x130);
                if (pvVar6 != 0) {
                    uVar10 = 1;
                    goto LAB_00439160;
                }
            }
            else if (iVar7 != iVar3) {
                ((Sprite*)field_B4)->ResetAnimation(1, 0);
                pvVar6 = field_128;
                if (pvVar6 != 0) {
                    uVar10 = 0;
LAB_00439160:
                    ((Sample*)pvVar6)->Play(100, uVar10);
                }
            }
        }

        local_c[0] = 0xe;
        piVar4 = (int*)((int)this + 0xdc);
        do {
            if (*piVar4 != 0) {
                ((Sample*)*piVar4)->Stop();
            }
            piVar4 = piVar4 + 1;
            local_c[0] = local_c[0] - 1;
        } while (local_c[0] != 0);

        if (iVar7 == iVar3) {
            pvVar6 = *(void**)((int)this + iVar3 * 4 + 0x104);
            if (pvVar6 != 0) {
                ((Sample*)pvVar6)->Play(100, 1);
            }
        }
        else if (*(void**)((int)this + 0x104) != 0) {
            ((Sample*)*(void**)((int)this + 0x104))->Play(100, 1);
        }
        }
    } else {
        if (*(int*)((int)field_B4 + 0x98) == 1) {
            pvVar6 = DAT_0046aa30;
            {
            unsigned int uVar2 = FUN_00433ae0(pvVar6, 0, "NUM_ACTIONS");
            if ((int)uVar2 < 0 || *(int*)((int)pvVar6 + 0x98) - 1 < (int)uVar2) {
                FUN_00425c50("Invalid gamestate %d", uVar2);
            }
            piVar4 = (int*)(*(int*)((int)pvVar6 + 0x90) + uVar2 * 4);
            }
            *piVar4 = *piVar4 + 5;
            ((Sprite*)field_B4)->ResetAnimation(0, 0);
            if (field_128 != 0) {
                ((Sample*)field_128)->Stop();
            }
            {
            void** puVar9 = (void**)((int)this + 0xb8);
            iVar7 = 3;
            do {
                if (*(int*)((int)*puVar9 + 0x98) != 0) {
                    ((Sprite*)*puVar9)->ResetAnimation(0, 0);
                    iVar3 = (int)field_124;
                    if (iVar3 != 0) {
                        iVar5 = *(int*)(iVar3 + 0xc);
                        if (iVar5 != 0 && *(int*)(iVar3 + 4) == *(int*)(iVar5 + 0xc)) {
                            iVar3 = DAT_0047652c(iVar5);
                            if (iVar3 == 4) goto LAB_004392aa;
                        }
                        ((Sample*)field_124)->Play(100, 0);
                    }
                }
LAB_004392aa:
                puVar9 = puVar9 + 1;
                iVar7 = iVar7 - 1;
            } while (iVar7 != 0);
            }
        }
    }

LAB_004392b0:
    if (field_120 != 0) {
        ((Sample*)field_120)->Play(100, 1);
    }
    return 1;
}
