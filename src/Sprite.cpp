#include <stdio.h>
#include <string.h>

#include "Sprite.h"
#include "Parser.h"
#include "GameState.h"

extern "C" {
    void Array_Cleanup(int, int, int, void*);
    void FreeFromGlobalHeap(int*);
    int* AllocateMemory_Wrapper(int);
    void FUN_00424b00(int*, int, int, void*, void*);
    int FUN_00420940(GameState*, char*);
    char* strstr_wrapper(char*, const char*);
}

// TODO: Move to a proper header
void (*ShowError)(const char* message, ...) = (void (*)(const char*, ...))0x00419110;

const char* s_error_Sprite_CheckRanges0_00436c04 = "error Sprite::CheckRanges0";
const char* s_error_Sprite_CheckRanges1_00436be8 = "error Sprite::CheckRanges1";
const char* s_bad_range_d_start_d_in_s_00436bc8 = "bad range[%d].start = %d in %s";

struct AnimationData {
    char pad[0xc];
    void* p;
};

struct AnimationData2 {
    char pad[0xc];
    int value;
};

/* Function start: 0x41D500 */
void Sprite::CheckRanges1()
{
    if (this->animation_data == 0) {
        ShowError(s_error_Sprite_CheckRanges0_00436c04);
    }
    if (this->ranges == 0) {
        ShowError(s_error_Sprite_CheckRanges1_00436be8);
    }
    int iVar4 = 0;
    if (0 < this->num_states) {
        int iVar3 = 0;
        do {
            int* piVar2 = (int*)((char*)this->ranges + 4 + iVar3);
            int iVar1 = ((AnimationData2*)(((AnimationData*)this->animation_data)->p))->value;
            if (iVar1 < *piVar2) {
                *piVar2 = iVar1;
            }
            piVar2 = (int*)((char*)this->ranges + iVar3);
            iVar1 = *piVar2;
            if (piVar2[1] < iVar1) {
                ShowError(s_bad_range_d_start_d_in_s_00436bc8, iVar4, iVar1, (char*)&this->sprite_filename);
            }
            iVar3 += 8;
            iVar4++;
        } while (iVar4 < this->num_states);
    }
}

/* Function start: 0x41d590 */
extern GameState* g_GameState;

int Sprite::CheckConditions()
{
    int iVar1;
    int* piVar2;
    int iVar3;
    int local_4;

    iVar3 = 0;
    if ((this->num_logic_conditions == 0) || (this->logic_conditions == 0)) {
        return 1;
    }
    local_4 = 0;
    if (0 < this->num_logic_conditions) {
        do {
            piVar2 = (int*)((char*)this->logic_conditions + iVar3);
            if (piVar2[1] == 1) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (g_GameState->field_0x90 <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState->field_0x88[iVar1] == 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 2) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (g_GameState->field_0x90 <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState->field_0x88[iVar1] != 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 3) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (g_GameState->field_0x90 <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState->field_0x88[iVar1] != this->handle) {
                    return 0;
                }
            }
            iVar3 = iVar3 + 8;
            local_4 = local_4 + 1;
        } while (local_4 < this->num_logic_conditions);
    }
    return 1;
}

/* Function start: 0x41D6D0 */
void Sprite::SetRange(int param_1, int param_2, int param_3)
{
    if (this->num_states <= param_1) {
        ShowError("Sprite::SetRange#1 %s %d", &this->sprite_filename, param_1);
    }
    if ((param_2 < 1) || (param_3 < 1)) {
        ShowError("Sprite::SetRange#2 %s %d range[%d, %d]", &this->sprite_filename, param_1, param_2, param_3);
    }
    int* piVar1 = (int*)((char*)this->ranges + param_1 * 8);
    *piVar1 = param_2;
    piVar1[1] = param_3;
    this->flags |= 0x20;
}

/* Function start: 0x41D9D0 */
int Sprite::LBLParse(char* param_1)
{
    char local_a0[32];
    char local_80[128];
    int local_ac;
    int local_a8;
    int local_a4;

    sscanf(param_1, "%s", local_a0);

    if (strcmp(local_a0, "FNAME") == 0) {
        sscanf(param_1, "%s %s", local_80, this->sprite_filename);
    } else if (strcmp(local_a0, "HANDLE") == 0) {
        sscanf(param_1, "%s %d", local_a0, &this->handle);
    } else if (strcmp(local_a0, "LOC") == 0) {
        sscanf(param_1, "%s %d %d", local_a0, &local_ac, &local_a4);
        this->loc_x = local_ac;
        this->loc_y = local_a4;
    } else if (strcmp(local_a0, "MAXLOGIC") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_ac);
        this->InitLogic(local_ac);
    } else if (strcmp(local_a0, "LOGIC") == 0) {
        sscanf(param_1, "%s %s", local_a0, local_80);
        local_ac = FUN_00420940(g_GameState, local_80);
        if ((0 < local_ac) && (g_GameState->field_0x90 <= local_ac)) {
            ShowError("GameState Error  #%d", 1);
        }
        if (strstr_wrapper(param_1, "FALSE") != 0) {
            this->SetLogic(local_ac, 2);
        } else if (strstr_wrapper(param_1, "TRUE") != 0) {
            this->SetLogic(local_ac, 1);
        } else if (strstr_wrapper(param_1, "EQUAL") != 0) {
            this->SetLogic(local_ac, 3);
            sscanf(param_1, "%s %s %d", local_a0, local_80, &this->handle);
        } else {
            ShowError("illegal %s", param_1);
        }
    } else if (strcmp(local_a0, "STATES") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_a8);
        this->SetState(local_a8);
    } else if (strcmp(local_a0, "RANGE") == 0) {
        sscanf(param_1, "%s %d %d %d", local_a0, &local_a8, &local_ac, &local_a4);
        this->SetRange(local_a8, local_ac, local_a4);
    } else if (strcmp(local_a0, "PRIORITY") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_a8);
        this->priority = local_a8;
    } else if (strcmp(local_a0, "TRANSPARENT") == 0) {
        this->flags |= 0x40;
    } else if (strcmp(local_a0, "BOTTOMLEFT") == 0) {
        this->flags |= 2;
    } else if (strcmp(local_a0, "TOPLEFT") == 0) {
        this->flags &= ~2;
    } else if (strcmp(local_a0, "KEEPOFFSET") == 0) {
        this->flags |= 0x10;
    } else if (strcmp(local_a0, "INVERT") == 0) {
        this->flags |= 0x400;
    } else if (strcmp(local_a0, "SCALE") == 0) {
        this->flags |= 8;
    } else if (strcmp(local_a0, "LOOP") == 0) {
        this->flags |= 1;
    } else if (strcmp(local_a0, "NOGRAPHIC") == 0) {
        this->flags |= 0x80;
    } else if (strcmp(local_a0, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse(param_1);
    }
    return 0;
}

/* Function start: 0x41D8D0 */
void Sprite::InitLogic(int param_1)
{
    if (this->logic_conditions != 0) {
        Array_Cleanup((int)this->logic_conditions, 8, *(int*)((char*)this->logic_conditions - 4), (void*)0x405770);
        FreeFromGlobalHeap((int*)((char*)this->logic_conditions - 4));
        this->logic_conditions = 0;
    }

    this->num_logic_conditions = param_1;
    int* piVar4 = 0;
    int* piVar1 = AllocateMemory_Wrapper(param_1 * 8 + 4);
    if (piVar1 != 0) {
        piVar4 = piVar1 + 1;
        *piVar1 = param_1;
        FUN_00424b00(piVar4, 8, param_1, (void*)0x41d850, (void*)0x405770);
    }
    this->logic_conditions = piVar4;

    if (0 < this->num_logic_conditions) {
        int iVar2 = 0;
        int iVar3 = 0;
        do {
            iVar3 = iVar3 + 8;
            iVar2 = iVar2 + 1;
            *(int*)((char*)this->logic_conditions + -8 + iVar3) = 0;
            *(int*)((char*)this->logic_conditions + -4 + iVar3) = 0;
        } while (iVar2 < this->num_logic_conditions);
    }
}

/* Function start: 0x41D740 */
void Sprite::SetState(int param_1)
{
    this->num_states = param_1;
    if (this->ranges != 0) {
        Array_Cleanup((int)this->ranges, 8, *(int*)((char*)this->ranges - 4), (void*)0x405770);
        FreeFromGlobalHeap((int*)((char*)this->ranges - 4));
        this->ranges = 0;
    }

    int* piVar5 = 0;
    int* piVar1 = AllocateMemory_Wrapper(this->num_states * 8 + 4);
    if (piVar1 != 0) {
        piVar5 = piVar1 + 1;
        *piVar1 = this->num_states;
        FUN_00424b00(piVar5, 8, this->num_states, (void*)0x41d850, (void*)0x405770);
    }
    this->ranges = piVar5;

    if (0 < this->num_states) {
        int iVar3 = 0;
        int iVar4 = 0;
        do {
            iVar4 = iVar4 + 1;
            int* puVar2 = (int*)((char*)this->ranges + iVar3);
            iVar3 = iVar3 + 8;
            *puVar2 = 1;
            puVar2[1] = 5000;
        } while (iVar4 < this->num_states);
    }

    this->flags |= 0x20;
}

/* Function start: 0x41D860 */
void Sprite::SetLogic(int param_1, int param_2)
{
    if (this->logic_conditions == 0) {
        this->InitLogic(1);
    }

    int iVar2 = 0;
    if (0 < this->num_logic_conditions) {
        int* piVar1 = (int*)((char*)this->logic_conditions + 4);
        do {
            if (*piVar1 == 0) {
                *(int*)((char*)this->logic_conditions + iVar2 * 8) = param_1;
                *(int*)((char*)this->logic_conditions + 4 + iVar2 * 8) = param_2;
                return;
            }
            piVar1 = piVar1 + 2;
            iVar2 = iVar2 + 1;
        } while (iVar2 < this->num_logic_conditions);
    }

    ShowError("Sprite::SetLogic %s", &this->sprite_filename);
}
