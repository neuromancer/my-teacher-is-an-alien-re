#include "SC_Fan.h"
#include "Sprite.h"
#include "SpriteAction.h"
#include "Palette.h"
#include "Sample.h"
#include "GameState.h"
#include "RockThrower.h"
#include "mss.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>
#include "TimeOut.h"
#include "VBuffer.h"
#include "ZBufferManager.h"
#include "SC_Question.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "main.h"
#include "string.h"

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" int FileExists(const char*);
extern "C" void WriteToLog(const char* format, ...);
extern char* __cdecl ResolveAssetPath(char* name);
extern void __fastcall FUN_00427880(void*);  // Weapon::Render

extern Weapon* DAT_00468ef0;                 // active weapon display
extern ZBufferManager* DAT_0046aa24;
extern "C" { extern GameState* DAT_0046aa30; }
extern MouseControl* DAT_0046aa18;
extern InputManager* DAT_0046aa08;

/* Function start: 0x40EFF0 */
SC_Fan::SC_Fan()
{
    memset(&field_A8, 0, 0x120);
    handlerId = 0x3E;
}

/* Function start: 0x40F190 */
SC_Fan::~SC_Fan() {
    Cleanup(0);
}

/* Function start: 0x40F290 */
void SC_Fan::Init(SC_Message* msg) {
    int savedId;
    void* mem;
    void* ptr;
    int* target;

    savedId = handlerId;
    memset(&field_A8, 0, 0x120);
    handlerId = savedId;

    if (!FileExists("CB_Fan")) {
        ShowLoadingScreen();
    }

    Handler::Init(msg);

    field_B0 = 0;
    dim_B4.field_0 = 0x140;
    dim_B4.field_4 = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = *((int*)msg + 1);

    ParseFile(this, "mis\\cb_fan.mis", (char*)0);

    if (field_A8 == 0) {
        field_A8 = new SpriteAction(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    ptr = field_C4;
    if (ptr != 0) {
        target = (int*)((int)DAT_0046aa24 + 0xA8);
        if (*target != 0) {
            WriteToLog("ddouble palette");
        }
        *target = (int)ptr;
    }

    mem = malloc(0xC);
    ptr = 0;
    if (mem != 0) {
        InitTimeOut((TimeOut*)mem);
        ptr = mem;
    }
    field_BC = ptr;
}

/* Function start: 0x40F420 */
void SC_Fan::Cleanup(int flag) {
    void* ptr;
    int i;

    ptr = field_C4;
    if (ptr != 0) {
        ((Palette*)ptr)->~Palette();
        free(ptr);
        field_C4 = 0;
    }

    ptr = field_C8;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_C8 = 0;
    }

    ptr = field_CC;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_CC = 0;
    }

    ptr = field_E0;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_E0 = 0;
    }

    ptr = field_E4;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_E4 = 0;
    }

    ptr = field_E8;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_E8 = 0;
    }

    ptr = field_EC;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_EC = 0;
    }

    if (DAT_00468ef0 != 0) {
        delete DAT_00468ef0;
        DAT_00468ef0 = 0;
    }

    ptr = field_C0;
    if (ptr != 0) {
        ((Sprite*)ptr)->~Sprite();
        free(ptr);
        field_C0 = 0;
    }

    ptr = field_144;
    if (ptr != 0) {
        ((VBuffer*)ptr)->~VBuffer();
        free(ptr);
        field_144 = 0;
    }

    ptr = field_BC;
    if (ptr != 0) {
        ((TimeOut*)ptr)->~TimeOut();
        free(ptr);
        field_BC = 0;
    }

    ptr = field_AC;
    if (ptr != 0) {
        ((SpriteAction*)ptr)->~SpriteAction();
        free(ptr);
        field_AC = 0;
    }

    ptr = field_A8;
    if (ptr != 0) {
        ((SpriteAction*)ptr)->~SpriteAction();
        free(ptr);
        field_A8 = 0;
    }

    for (i = 0xb; i != 0; i--) {
        ptr = field_198[0xb - i];
        if (ptr != 0) {
            ((Sample*)ptr)->Unload();
            free(ptr);
            field_198[0xb - i] = 0;
        }
    }

    field_1C4 = 0;

    if (flag != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x40F660 */
void SC_Fan::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    switch (field_17C) {
    case 0:
        State0Handler();
        return;
    case 1:
        RenderFan();
        return;
    case 2:
        State2Handler();
        return;
    case 3:
        ProcessRound();
        return;
    case 4:
        State4Handler();
        return;
    }
}

/* Function start: 0x40F6C0 */
int SC_Fan::AddMessage(SC_Message* msg) {
    Handler::AddMessage(msg);

    if (*((int*)msg + 0xB) == 0x1B) {
        field_17C = 4;
    }

    return 1;
}

/* Function start: 0x40F6F0 */
int SC_Fan::Exit(SC_Message* msg) {
    int* p;

    p = (int*)msg;
    if (p[0] != handlerId) {
        return 0;
    }

    switch (p[4]) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x40F760 */
void SC_Fan::ProcessRound() {
    void* ptr;

    if (savedCommand == 0x2B) {
        SendGameMessage(4, field_1C4, handlerId, moduleParam, 0x13, 0, 100, 0, 0, 0);

        if (field_17C == 2) {
            ptr = field_A8;
            if (ptr != 0) {
                ((SpriteAction*)ptr)->~SpriteAction();
                free(ptr);
                field_A8 = 0;
            }
            {
                void* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                Parser temp;
                field_A8 = newAction;
                ParseFile(&temp, "mis\\cb_fan.mis", "[WIN_LBL_PR]");
            }
        } else if (field_17C == 3) {
            ptr = field_A8;
            if (ptr != 0) {
                ((SpriteAction*)ptr)->~SpriteAction();
                free(ptr);
                field_A8 = 0;
            }
            {
                void* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                Parser temp;
                field_A8 = newAction;
                ParseFile(&temp, "mis\\cb_fan.mis", "[LOSE_LBL_PR]");
            }
        }
    } else {
        SendGameMessage(4, field_1C4, handlerId, moduleParam, 0x13, 0, 1000, 0, 0, 0);
        SendGameMessage(5, 0, handlerId, moduleParam, 0x13, 0, 0, 0, 0, 0);

        if (field_17C == 4) {
            ((SpriteAction*)field_A8)->extra1 = 1;
            ((SpriteAction*)field_A8)->mousePos.field_0 = 0x78;
            ((SpriteAction*)field_A8)->mousePos.field_4 = 0;

            void* gs = DAT_0046aa30;
            int idx = ((GameState*)gs)->FindLabel("NUM_ACTIONS");
            if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            *(int*)(*(int*)((int)gs + 0x90) + idx * 4) += 0x14;
        } else if (field_17C == 3) {
            EnqueueSpriteAction(field_AC);
            ptr = field_AC;
            if (ptr != 0) {
                ((SpriteAction*)ptr)->~SpriteAction();
                free(ptr);
                field_AC = 0;
            }
            ptr = field_A8;
            if (ptr != 0) {
                ((SpriteAction*)ptr)->~SpriteAction();
                free(ptr);
                field_A8 = 0;
            }
            return;
        } else if (field_17C == 2) {
            void* gs = DAT_0046aa30;
            int idx = ((GameState*)gs)->FindLabel("NUM_ACTIONS");
            if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            *(int*)(*(int*)((int)gs + 0x90) + idx * 4) += 0x1E;

            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindLabel("COMBAT_FAN_WON");
            if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = 1;

            ((SpriteAction*)field_A8)->extra1 = 1;
            ((SpriteAction*)field_A8)->mousePos.field_0 = 0x78;
            ((SpriteAction*)field_A8)->mousePos.field_4 = 0;
        }
    }

    EnqueueSpriteAction(field_A8);
    ptr = field_A8;
    if (ptr != 0) {
        ((SpriteAction*)ptr)->~SpriteAction();
        free(ptr);
        field_A8 = 0;
    }
}

/* Function start: 0x40FBD0 */
void SC_Fan::DisplaySprites(int frame) {
    int i;
    void** ptr;

    ((Sprite*)field_C8)->ResetAnimation(frame, 0);

    ptr = &field_E0;
    i = 4;
    do {
        if (*ptr != 0) {
            int val;
            val = *((int*)ptr + 4) ? -1 : 0;
            val = (val & 5) + frame;
            ((Sprite*)*ptr)->ResetAnimation(val, 0);
        }
        ptr = (void**)((char*)ptr + 4);
        i--;
    } while (i != 0);
}

/* Function start: 0x40FEA0 */
void SC_Fan::State0Handler() {
    void* sample;

    RenderFan();

    if (field_180 == 0) {
        field_180 = 1;
        SendGameMessage(4, field_1C4, handlerId, moduleParam, 0x13, 0x32, 0x3E8, 0, 0, 0);

        sample = field_198[1];
        if (sample != 0) {
            ((Sample*)sample)->Play(100, 1);
        }
        return;
    }

    if (field_180 == 1) {
        sample = field_198[1];
        if (sample != 0) {
            if (*(int*)((int)sample + 0xC) != 0) {
                if (*(int*)((int)sample + 4) == *(int*)(*(int*)((int)sample + 0xC) + 0xC)) {
                    if (AIL_sample_status(*(HSAMPLE*)((int)sample + 0xC)) == 4) {
                        return;
                    }
                }
            }
        }
        field_180++;
        field_17C = 1;
        field_178 = 1;
    }
}

/* Function start: 0x40FF40 */
void SC_Fan::State2Handler() {
    Sprite* sprite;
    void* sample;

    sprite = (Sprite*)field_C8;
    if (sprite->Do(sprite->num_states, sprite->field_0xb0, 1.0)) {
        ((Sprite*)field_C8)->ResetAnimation(5, 0);
    }

    sample = field_198[8];
    if (sample != 0) {
        if (*(int*)((int)sample + 0xC) != 0) {
            if (*(int*)((int)sample + 4) == *(int*)(*(int*)((int)sample + 0xC) + 0xC)) {
                if (AIL_sample_status(*(HSAMPLE*)((int)sample + 0xC)) == 4) {
                    return;
                }
            }
        }
    }

    ProcessRound();
}

/* Function start: 0x40FFC0 */
void SC_Fan::State4Handler() {
    void* ptr;
    void* sample;
    int i;

    if (field_190 == 0) {
        ptr = &field_198[0];
        i = 0xB;
        do {
            if (*(void**)ptr != 0) {
                ((Sample*)*(void**)ptr)->~Sample();
            }
            ptr = (char*)ptr + 4;
            i--;
        } while (i != 0);

        sample = field_198[10];
        if (sample != 0) {
            ((Sample*)sample)->Play(100, 1);
        }

        field_190++;
        field_178 = 0;
        RenderFan();
        return;
    }

    if (field_190 == 1) {
        sample = field_198[10];
        if (sample != 0) {
            if (*(int*)((int)sample + 0xC) != 0) {
                if (*(int*)((int)sample + 4) == *(int*)(*(int*)((int)sample + 0xC) + 0xC)) {
                    if (AIL_sample_status(*(HSAMPLE*)((int)sample + 0xC)) == 4) {
                        goto end;
                    }
                }
            }
        }
        field_190++;
        ProcessRound();
    }

end:
    RenderFan();
}

/* Function start: 0x410060 */
void SC_Fan::RenderFan() {
    Sprite* sprite;
    void** ptr;
    int state;
    int i;

    sprite = (Sprite*)field_C8;
    if (sprite == 0) {
        return;
    }

    state = *(int*)((int)sprite + 0x98);

    if (state == 3) {
        if (field_CC != 0) {
            if (((Sprite*)field_CC)->Do(((Sprite*)field_CC)->num_states, ((Sprite*)field_CC)->field_0xb0, 1.0)) {
                DisplaySprites(0);
                SendGameMessage(4, field_1C4, handlerId, moduleParam, 0x13, 0x32, 0x3E8, 0, 0, 0);
            }
        }
    }

    ptr = &field_E0;
    i = 4;
    do {
        if (*ptr != 0) {
            ((Sprite*)*ptr)->Do(((Sprite*)*ptr)->num_states, ((Sprite*)*ptr)->field_0xb0, 1.0);
        }
        ptr = (void**)((char*)ptr + 4);
        i--;
    } while (i != 0);

    sprite = (Sprite*)field_C8;
    if (sprite->Do(sprite->num_states, sprite->field_0xb0, 1.0)) {
        if (state == 0) {
            DisplaySprites(1);
        } else if (state == 2) {
            if (field_CC != 0) {
                ((Sprite*)field_CC)->ResetAnimation(1, 0);
            }
            DisplaySprites(3);
        }
    }

    if (field_144 != 0) {
        int dimVal;
        dimVal = dim_168.field_4;
        if (dimVal != 0 && dim_168.field_0 >= dimVal) {
            (DAT_0046aa24)->DrawVBufferRegion(
                field_144, 0x7531, dim_170.field_0, dim_170.field_4, 2, 1.0,
                invSlot_158.left, invSlot_158.top, invSlot_158.right, invSlot_158.bottom);

            if (*(int*)field_BC == 1) {
                if (((TimeOut*)field_BC)->IsTimeOut()) {
                    field_17C = 3;
                }
            } else {
                ((TimeOut*)field_BC)->Start(0x5DC);
            }
        } else {
            int offset;
            int rnd;
            offset = dim_168.field_0 * 0x36 / dimVal;
            rnd = rand();
            offset = offset - rnd % 3 + 1;
            if (offset < 0) {
                offset = 0;
            } else if (offset > 0x36) {
                offset = 0x36;
            }
            offset = offset * 4 + 0x12;

            (DAT_0046aa24)->DrawVBufferRegion(
                field_144, 0x7531, dim_170.field_0, dim_170.field_4, 2, 1.0,
                invSlot_158.left, invSlot_158.top, offset, invSlot_158.bottom);

            (DAT_0046aa24)->DrawVBufferRegion(
                field_144, 0x7531, dim_170.field_0 + offset, dim_170.field_4, 2, 1.0,
                offset, invSlot_148.top, invSlot_148.right, invSlot_148.bottom);
        }
    }

    if (field_178 != 0) {
        int frames;
        void* p;

        (DAT_0046aa18)->DrawCursor();
        FUN_00427880(DAT_00468ef0);

        if (DAT_00468ef0->m_clicked != 0) {
            frames = 0;
            p = *(void**)((int)DAT_0046aa08 + 0x1A0);
            if (p != 0) {
                frames = *(int*)p;
            }
            ((Sprite*)field_C0)->ResetAnimation(frames / (dim_B4.field_0 / 3) + 5, 0);
        }

        sprite = (Sprite*)field_C0;
        if (sprite->Do(sprite->num_states, sprite->field_0xb0, 1.0)) {
            frames = 0;
            p = *(void**)((int)DAT_0046aa08 + 0x1A0);
            if (p != 0) {
                frames = *(int*)p;
            }
            sprite->ResetAnimation(frames / (dim_B4.field_0 / 5), 0);
        }
    }
}

/* Function start: 0x410390 */
void SC_Fan::OnProcessEnd()
{
}

/* Function start: 0x4104B0 */
int SC_Fan::LBLParse(char* param_1) {
    char local_b8[128];
    char local_38[32];
    int local_18;

    local_18 = 0;
    local_b8[0] = 0;
    local_38[0] = 0;
    sscanf(param_1, "%s", local_38);

    if (strcmp(local_38, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        Palette* pal = new Palette();
        field_C4 = pal;
        pal->Load(local_b8);
    }
    else if (strcmp(local_38, "BACKGROUND_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_C8 = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "STOP_SWITCH_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_CC = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "TL_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_E0 = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "BL_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_E4 = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "TR_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_E8 = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "BR_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_EC = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "CONSOLE_SPRITE") == 0) {
        void* mem = malloc(0xf8);
        void* spr = 0;
        if (mem != 0) {
            spr = new (mem) Sprite((char*)0);
        }
        field_C0 = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "NOISE_METER_VBUFFER") == 0) {
        sscanf(param_1, " %s %s %d %d %d ", local_38, local_b8,
               &dim_170.field_0, &dim_170.field_4, &dim_168.field_4);
        void* mem = malloc(0x30);
        void* obj = 0;
        if (mem != 0) {
            char* path = ResolveAssetPath(local_b8);
            obj = new (mem) VBuffer(path, 0);
        }
        field_144 = obj;
        invSlot_148.right = 0x10D;
        invSlot_148.left = 0;
        invSlot_148.top = 0;
        invSlot_158.left = 0;
        invSlot_158.right = 0x10D;
        invSlot_148.bottom = 0x13;
        invSlot_158.top = 0x14;
        invSlot_158.bottom = 0x27;
    }
    else if (strcmp(local_38, "NUM_BOLTS_REMAINING") == 0) {
        /* recognized but no action */
    }
    else if (strcmp(local_38, "SET_GAMESTATE") == 0) {
        sscanf(param_1, " %s %s %d ", local_38, local_b8, &local_18);
        int val = local_18;
        void* gs = DAT_0046aa30;
        int idx = ((GameState*)gs)->FindLabel(local_b8);
        if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = val;
    }
    else if (strcmp(local_38, "SOUND") == 0) {
        int ret = sscanf(param_1, " %s %d %s ", local_38, &local_18, local_b8);
        if (ret != 3 || local_18 < -1 || 10 < local_18) {
            ReportUnknownLabel("SC_Fan");
        }
        if (local_18 == 0) {
            int sndId = atoi(local_b8);
            SendGameMessage(5, sndId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        }
        else if (local_18 == -1) {
            field_1C4 = atoi(local_b8);
            SendGameMessage(4, field_1C4, handlerId, moduleParam, 0x1b, 1, 0, 0, 0, 0);
        }
        else {
            void* mem = malloc(0x10);
            void* smp = 0;
            if (mem != 0) {
                new (mem) Sample();
                smp = mem;
            }
            field_198[local_18] = smp;
            char* path = MakeAudioName(local_b8);
            int err = ((Sample*)smp)->Load(path);
            if (err != 0 && field_198[local_18] != 0) {
                ((Sample*)field_198[local_18])->Unload();
                free(field_198[local_18]);
                field_198[local_18] = 0;
            }
        }
    }
    else if (strcmp(local_38, "WEAPON") == 0) {
        int ret = sscanf(param_1, " %s %s ", local_38, local_b8);
        if (ret == 2) {
            if (strcmp(local_b8, "ROCKTHROWER2") == 0) {
                void* mem = malloc(200);
                if (mem == 0) {
                    DAT_00468ef0 = 0;
                } else {
                    new (mem) RockThrower(this);
                    DAT_00468ef0 = (Weapon*)mem;
                }
            } else {
                ReportUnknownLabel("SC_Fan");
            }
        }
    }
    else if (strcmp(local_38, "LOSE_MESSAGE") == 0) {
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        field_AC = action;
        ParseSpriteAction(action, this);
    }
    else if (strcmp(local_38, "END") == 0) {
        return 1;
    }
    else {
        ReportUnknownLabel("SC_Fan");
    }

    return 0;
}
