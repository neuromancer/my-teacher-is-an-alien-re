#include <stdio.h>
#include <string.h>

#include <string.h>

#include "globals.h"
#include "Sprite.h"
#include "Parser.h"
#include "GameState.h"
#include "Animation.h"
#include "Queue.h"
#include "Memory.h"
#include "Array.h"
#include "SoundManager.h"
#include "string.h"

class VBuffer;

extern Animation* Animation_Ctor(Animation*);
extern Animation* Animation_Ctor_Filename(Animation*, char*);

extern "C" {
    //__int64 __ftol();

}

// C++ helper defined in Array.cpp
void Array_Iterate(void* array, unsigned int element_size, int num_elements, void (*callback)(void*), void (*cleanup_function)(void*));

// Constructor wrapper for external callers
Sprite* Sprite_Ctor(Sprite* p, char* filename) {
    p->Sprite::Sprite(filename);
    return p;
}

/* Function start: 0x41CD50 */
Sprite::Sprite(char* filename)
{
    // BaseObject::BaseObject_Init((undefined4 *)this);
    this->loc_x = 0;
    this->loc_y = 0;
    memset(&this->ranges, 0, 0x14 * 4);
    this->flags |= 0x20;
    if (filename != 0) {
        sscanf(filename, "%s", &this->sprite_filename);
    }
    this->SetState(1);
    this->priority = 0;
    this->SetRange(0, 1, 5000);
}

/* Function start: 0x41CE30 */
Sprite::~Sprite()
{
    this->StopAnimationSound();

    if (this->ranges != 0) {
        Array_Cleanup((void*)this->ranges, 8, *(int*)((char*)this->ranges - 4), (void(__cdecl*)(void*))0x405770);
        FreeMemory((int*)((char*)this->ranges - 4));
        this->ranges = 0;
    }

    if (this->logic_conditions != 0) {
        Array_Cleanup((void*)this->logic_conditions, 8, *(int*)((char*)this->logic_conditions - 4), (void(__cdecl*)(void*))0x405770);
        FreeMemory((int*)((char*)this->logic_conditions - 4));
        this->logic_conditions = 0;
    }
}

/* Function start: 0x41CF10 */
void Sprite::Init()
{
    try {
        if (this->animation_data == 0) {
            Animation* anim = (Animation*)AllocateMemory(0x2c);
            Animation* result = 0;
            if (anim != 0) {
                result = Animation_Ctor_Filename(anim, this->sprite_filename);
            }
            this->animation_data = result;
        } else if (this->animation_data->data == 0) {
            this->animation_data->ToBuffer();
        }

        if (this->animation_data && this->animation_data->data) {
            if (DAT_00436b9c == 0) {
                char* p = DAT_0043d630;
                do {
                    *p = 0;
                    p = p + 0x40;
                } while (p < &DAT_0043d630[0x800]);
                DAT_00436b9c = 1;
            }
            memcpy(&DAT_0043d630[this->animation_data->data->handle * 0x40], this->sprite_filename, strlen(this->sprite_filename) + 1);
        }

        this->CheckRanges1();
        this->flags |= 0x20;
        this->SetState2(this->current_state);
    } catch (...) {
    }
}

/* Function start: 0x41D040 */
void Sprite::StopAnimationSound()
{
    Animation* anim = this->animation_data;
    int sound_idx;

    if (anim == 0 || anim->data == 0) {
        sound_idx = -1;
    } else {
        sound_idx = anim->data->handle;
    }

    if (g_SoundManager == 0 || *(int*)((int)g_SoundManager + 0x98) == 0) {
        if (anim != 0) {
            anim->Delete(1);
        }
    } else if (anim != 0) {
        ((Queue*)g_SoundManager)->Insert(anim);
    }

    this->animation_data = 0;

    if (sound_idx != -1) {
        DAT_0043d630[sound_idx * 0x40] = 0;
    }
    this->flags |= 0x20;
}

/* Function start: 0x41D0C0 */
void Sprite::InitAnimation()
{
    try {
        if (this->animation_data == 0) {
            Animation* anim = (Animation*)AllocateMemory(0x2c);
            if (anim != 0) {
                anim = Animation_Ctor(anim);
            }
            this->animation_data = anim;
            anim->Open(this->sprite_filename, 0xfe000, -1);
        }
    } catch (...) {
    }
}

/* Function start: 0x41D160 */
void Sprite::FreeAnimation()
{
    if (this->animation_data != 0 && this->animation_data->data != 0) {
        if (this->animation_data->data->handle != -1) {
            DAT_0043d630[this->animation_data->data->handle * 0x40] = 0;
        }
        this->animation_data->FreeVBuffer();
    }
}

/* Function start: 0x41D190 */
void Sprite::SetState2(int param_1)
{
    int offset = 0;

    if (param_1 == -1) {
        this->current_state = param_1;
        return;
    }

    if ((this->animation_data == 0) || (this->animation_data->data == 0)) {
        this->Init();
    }

    if (param_1 < 0 || param_1 > this->num_states - 1) {
        ShowError("Sprite::SetState 0 %d %s", param_1, &this->sprite_filename);
    }

    int current_frame = 0;
    if (this->animation_data != 0) {
        current_frame = this->animation_data->smk->FrameNum;
    }

    int in_range;
    if (this->animation_data == 0) {
        ShowError("range error");
        in_range = 0;
    } else if (this->ranges == 0) {
        ShowError("range error");
        in_range = 0;
    } else {
        int* range = (int*)((char*)this->ranges + param_1 * 8);
        if (current_frame < range[0] || range[1] < current_frame) {
            in_range = 0;
        } else {
            in_range = 1;
        }
    }

    if (in_range == 0) {
        this->flags |= 0x20;
    }

    int fl = this->flags;
    if ((fl & 0x20) == 0 && this->current_state == param_1) {
        return;
    }

    if ((fl & 0x10) != 0) {
        int* old_range = (int*)((char*)this->ranges + this->current_state * 8);
        offset = this->animation_data->smk->FrameNum - old_range[0];

        int new_start = *(int*)((char*)this->ranges + param_1 * 8);
        int new_end_frame = new_start + offset + 1;
        offset++;

        int valid;
        if (this->animation_data == 0 || this->ranges == 0) {
            ShowError("range error");
            valid = 0;
        } else {
            if (new_end_frame < new_start || *(int*)((char*)this->ranges + param_1 * 8 + 4) < new_end_frame) {
                valid = 0;
            } else {
                valid = 1;
            }
        }

        if (valid == 0) {
            offset = 0;
        }
    }

    this->current_state = param_1;
    this->animation_data->GotoFrame(*(int*)((char*)this->ranges + param_1 * 8) + offset);

    int* range = (int*)(this->current_state * 8 + (int)this->ranges);
    if (range[1] == range[0]) {
        this->flags |= 4;
    }
    this->flags &= ~0x20;
}

/* Function start: 0x41d300 */
int Sprite::Do(int x, int y, double scale)
{
    int skipAnimLoop;
    int done;

    if (this->current_state == -1) {
        return 1;
    }
    skipAnimLoop = 0;
    done = 0;
    if (CheckConditions() == 0) {
        return 1;
    }
    if ((this->flags & 0x80) != 0) {
        return 1;
    }
    if ((this->animation_data == 0) || (this->animation_data->data == 0)) {
        this->Init();
    }
    int* range = (int*)(this->current_state * 8 + (int)this->ranges);
    if (range[1] == range[0]) {
        int f = this->flags;
        skipAnimLoop = 1;
        if ((f & 4) != 0) {
            skipAnimLoop = 0;
            this->flags = f & ~4;
        }
        done = 1;
    }
    if (skipAnimLoop == 0) {
        Animation::DoFrame(this->animation_data);
        Animation* anim = this->animation_data;
        int remaining;
        if (anim == 0) {
            remaining = *(int*)((int)this->ranges + 4 + this->current_state * 8);
        }
        else {
            remaining = *(int*)((int)this->ranges + 4 + this->current_state * 8) - this->animation_data->smk->FrameNum;
        }
        if (remaining == 1) {
            if ((this->flags & 0x200) == 0) {
                this->animation_data->GotoFrame(*(int*)((int)this->ranges + this->current_state * 8));
            }
            else {
                this->animation_data->NextFrame();
            }
            if ((this->flags & 1) == 0) {
                done = 1;
            }
        }
        else {
            this->animation_data->NextFrame();
        }
    }
    int flags = this->flags;
    if ((flags & 0x100) != 0) {
        return done;
    }
    if ((flags & 2) == 0) {
        Animation* anim = this->animation_data;
        if ((flags & 8) == 0) {
            if (anim == 0) {
                y = y - 1;
            }
            else {
                y = y + this->animation_data->data->height - 1;
            }
        }
        else if (anim == 0) {
            y = y + (int)(-scale);
        }
        else {
            y = y + (int)((this->animation_data->data->height - 1) * scale);
        }
    }
    int mode = 0;
    if ((flags & 8) == 0) {
        if ((flags & 0x40) != 0) {
            mode = 1;
        }
    }
    else {
        mode = 3 - (unsigned int)((flags & 0x40) == 0);
    }
    g_SoundManager->PlayAnimationSound(this->animation_data->data, this->priority, x, y, mode, *(int*)&scale, *((int*)&scale + 1));
    return -((this->flags & 1) == 0) & done;
}

/* Function start: 0x41D500 */
void Sprite::CheckRanges1()
{
    if (this->animation_data == 0) {
        ShowError("error Sprite::CheckRanges0");
    }
    if (this->ranges == 0) {
        ShowError("error Sprite::CheckRanges1");
    }
    int iVar4 = 0;
    if (0 < this->num_states) {
        int iVar3 = 0;
        do {
            int* piVar2 = (int*)((char*)this->ranges + 4 + iVar3);
            int iVar1 = this->animation_data->smk->FrameNum;
            if (iVar1 < *piVar2) {
                *piVar2 = iVar1;
            }
            piVar2 = (int*)((char*)this->ranges + iVar3);
            iVar1 = *piVar2;
            if (piVar2[1] < iVar1) {
                ShowError("bad range[%d].start = %d in %s", iVar4, iVar1, (char*)&this->sprite_filename);
            }
            iVar3 += 8;
            iVar4++;
        } while (iVar4 < this->num_states);
    }
}


/* Function start: 0x41d590 */
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
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] == 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 2) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] != 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 3) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] != this->handle) {
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

/* Function start: 0x41D740 */
void Sprite::SetState(int param_1)
{
    try {
        this->num_states = param_1;
        if (this->ranges != 0) {
            Array_Cleanup((void*)this->ranges, 8, *(int*)((char*)this->ranges - 4), (void(__cdecl*)(void*))0x405770);
            FreeMemory((int*)((char*)this->ranges - 4));
            this->ranges = 0;
        }

        int num = this->num_states;
        int* piVar5 = 0;
        int* piVar1 = (int*)AllocateMemory(num * 8 + 4);
        if (piVar1 != 0) {
            piVar5 = piVar1 + 1;
            *piVar1 = num;
            Array_Iterate(piVar5, 8, num, (void(*)(void*))0x41d850, (void(*)(void*))0x405770);
        }

        int iVar3 = 0;
        int iVar4 = 0;
        this->ranges = piVar5;

        if (0 < this->num_states) {
            do {
                iVar4 = iVar4 + 1;
                int* puVar2 = (int*)((char*)this->ranges + iVar3);
                iVar3 = iVar3 + 8;
                *puVar2 = 1;
                puVar2[1] = 5000;
            } while (iVar4 < this->num_states);
        }

        this->flags |= 0x20;
    } catch (...) {
    }
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

/* Function start: 0x41D8D0 */
void Sprite::InitLogic(int param_1)
{
    try {
        if (this->logic_conditions != 0) {
            Array_Cleanup((void*)this->logic_conditions, 8, *(int*)((char*)this->logic_conditions - 4), (void(__cdecl*)(void*))0x405770);
            FreeMemory((int*)((char*)this->logic_conditions - 4));
            this->logic_conditions = 0;
        }

        this->num_logic_conditions = param_1;
        int* piVar4 = 0;
        int* piVar1 = (int*)AllocateMemory(param_1 * 8 + 4);
        if (piVar1 != 0) {
            piVar4 = piVar1 + 1;
            *piVar1 = param_1;
            Array_Iterate(piVar4, 8, param_1, (void(*)(void*))0x41d850, (void(*)(void*))0x405770);
        }

        int iVar2 = 0;
        int iVar3 = 0;
        this->logic_conditions = piVar4;

        if (this->num_logic_conditions > 0) {
            int edx = 0;
            do {
                edx = edx + 8;
                iVar3 = iVar3 + 1;
                *(int*)((char*)this->logic_conditions + edx - 8) = iVar2;
                *(int*)((char*)this->logic_conditions + edx - 4) = iVar2;
            } while (iVar3 < this->num_logic_conditions);
        }
    } catch (...) {
    }
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
        local_ac = g_GameState_00436998->FindState(local_80);
        if ((0 < local_ac) && (g_GameState_00436998->maxStates <= local_ac)) {
            ShowError("GameState Error  #%d", 1);
        }
        if (strstr(param_1, "FALSE") != 0) {
            this->SetLogic(local_ac, 2);
        } else if (strstr(param_1, "TRUE") != 0) {
            this->SetLogic(local_ac, 1);
        } else if (strstr(param_1, "EQUAL") != 0) {
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

/* Function start: 0x41E010 */
void Sprite::Dump()
{
    int iVar4;
    int iVar5;

    WriteToMessageLog("FNAME: %s", this->sprite_filename);
    WriteToMessageLog("HANDLE: %d", this->handle);
    WriteToMessageLog("LOC: %d %d", this->loc_x, this->loc_y);
    WriteToMessageLog("PRIORITY: %d", this->priority);
    if (this->logic_conditions != 0) {
        if (1 < this->num_logic_conditions) {
            WriteToMessageLog("MAXLOGIC: %d", this->num_logic_conditions);
        }
        iVar4 = 0;
        iVar5 = 0;
        if (0 < this->num_logic_conditions) {
            do {
                int* piVar1 = (int*)((char*)this->logic_conditions + iVar4);
                if (piVar1[1] == 1) {
                    int uVar2 = g_GameState_00436998->GetState(*piVar1);
                    WriteToMessageLog("LOGIC: %s TRUE", (char*)uVar2);
                }
                piVar1 = (int*)((char*)this->logic_conditions + iVar4);
                if (piVar1[1] == 2) {
                    int uVar2 = g_GameState_00436998->GetState(*piVar1);
                    WriteToMessageLog("LOGIC: %s FALSE", (char*)uVar2);
                }
                iVar4 = iVar4 + 8;
                iVar5 = iVar5 + 1;
            } while (iVar5 < this->num_logic_conditions);
        }
    }
    if ((this->flags & 0x40) != 0) {
        WriteToMessageLog("TRANSPARENT");
    }
    if ((this->flags & 2) != 0) {
        WriteToMessageLog("BOTTOMLEFT");
    }
    if ((this->flags & 0x10) != 0) {
        WriteToMessageLog("KEEPOFFSET");
    }
    if ((this->flags & 8) != 0) {
        WriteToMessageLog("SCALE");
    }
    if ((this->flags & 1) != 0) {
        WriteToMessageLog("LOOP");
    }
    if ((this->flags & 0x80) != 0) {
        WriteToMessageLog("NOGRAPHIC");
    }
    WriteToMessageLog("STATES: %d", this->num_states);
    iVar4 = 0;
    if (0 < this->num_states) {
        do {
            int* puVar3 = (int*)(iVar4 * 8 + (int)this->ranges);
            iVar5 = iVar4 + 1;
            WriteToMessageLog("RANGE: %d %d %d", iVar4, *puVar3, puVar3[1]);
            iVar4 = iVar5;
        } while (iVar5 < this->num_states);
    }
    WriteToMessageLog("END");
}
