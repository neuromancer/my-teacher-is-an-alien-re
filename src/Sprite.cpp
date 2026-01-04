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
#include "ZBufferManager.h"
#include "string.h"
#include "Cleanup.h"

class VBuffer;

/* Function start: 0x41CD50 */
Sprite::Sprite(char* filename)
{
    // BaseObject::BaseObject_Init((undefined4 *)this);
    loc_x = 0;
    loc_y = 0;
    memset(&ranges, 0, 0x14 * 4);
    flags |= 0x20;
    if (filename != 0) {
        sscanf(filename, "%s", sprite_filename);
    }
    SetState(1);
    priority = 0;
    SetRange(0, 1, 5000);
}

/* Function start: 0x41CE30 */
Sprite::~Sprite()
{
    StopAnimationSound();

    if (ranges != 0) {
        delete[] ranges;
        ranges = 0;
    }

    if (logic_conditions != 0) {
        delete[] logic_conditions;
        logic_conditions = 0;
    }
}

/* Function start: 0x41CF10 */
void Sprite::Init()
{
    Animation* anim;

    try {
        anim = animation_data;
        if (anim != 0) {
            if (anim->data != 0) {
                return;
            }
        }
        if (anim == 0) {
            anim = new Animation(sprite_filename);
            animation_data = anim;
        }
    } catch (...) {
    }

    if (animation_data->data == 0) {
        animation_data->ToBuffer();
    }

    if (animation_data->data != 0) {
        if (DAT_00436b9c == 0) {
            char* ptr = DAT_0043d630;
            do {
                *ptr = 0;
                ptr += 0x40;
            } while (ptr < &DAT_0043d630[0x800]);
            DAT_00436b9c = 1;
        }
        strcpy(&DAT_0043d630[animation_data->data->handle * 0x40], sprite_filename);
    }

    //CheckRanges1();
    flags |= 0x20;
    SetState2(current_state);
}

/* Function start: 0x41D040 */
void Sprite::StopAnimationSound()
{
    Animation* anim = animation_data;
    int sound_idx;

    if (anim == 0 || anim->data == 0) {
        sound_idx = -1;
    } else {
        sound_idx = anim->data->handle;
    }

    if (DAT_0043698c == 0 || *(int*)((int)DAT_0043698c + 0x98) == 0) {
        if (anim != 0) {
            anim->Delete(1);
        }
    } else if (anim != 0) {
        ((Queue*)DAT_0043698c)->Insert(anim);
    }

    animation_data = 0;

    if (sound_idx != -1) {
        DAT_0043d630[sound_idx * 0x40] = 0;
    }
    flags |= 0x20;
}

/* Function start: 0x41D0C0 */
void Sprite::InitAnimation()
{
    if (animation_data != 0) {
        return;
    }

    /*Animation* anim = (Animation*)AllocateMemory(0x2c);
    try {
        if (anim != 0) {
            anim = anim->Init();
        }
    } catch (...) {
    }*/
    animation_data = new Animation();
    animation_data->Open(sprite_filename, 0xfe000, -1);
}

/* Function start: 0x41D160 */
void Sprite::FreeAnimation()
{
    if (animation_data != 0 && animation_data->data != 0) {
        if (animation_data->data->handle != -1) {
            DAT_0043d630[animation_data->data->handle * 0x40] = 0;
        }
        animation_data->FreeVBuffer();
    }
}

/* Function start: 0x41D190 */
void Sprite::SetState2(int param_1)
{
    int offset = 0;

    if (param_1 == -1) {
        current_state = param_1;
        return;
    }

    if ((animation_data == 0) || (animation_data->data == 0)) {
        Init();
    }

    if (param_1 < 0 || param_1 > num_states - 1) {
        ShowError("Sprite::SetState 0 %d %s", param_1, sprite_filename);
    }

    int current_frame = 0;
    if (animation_data != 0) {
        current_frame = animation_data->smk->FrameNum;
    }

    int in_range;
    if (animation_data == 0) {
        ShowError("range error");
        in_range = 0;
    } else if (ranges == 0) {
        ShowError("range error");
        in_range = 0;
    } else {
        int* range = (int*)((char*)ranges + param_1 * 8);
        if (current_frame < range[0] || range[1] < current_frame) {
            in_range = 0;
        } else {
            in_range = 1;
        }
    }

    if (in_range == 0) {
        flags |= 0x20;
    }

    int fl = flags;
    if ((fl & 0x20) == 0 && current_state == param_1) {
        return;
    }

    if ((fl & 0x10) != 0) {
        int* old_range = (int*)((char*)ranges + current_state * 8);
        offset = animation_data->smk->FrameNum - old_range[0];

        int new_start = *(int*)((char*)ranges + param_1 * 8);
        int new_end_frame = new_start + offset + 1;
        offset++;

        int valid;
        if (animation_data == 0 || ranges == 0) {
            ShowError("range error");
            valid = 0;
        } else {
            if (new_end_frame < new_start || *(int*)((char*)ranges + param_1 * 8 + 4) < new_end_frame) {
                valid = 0;
            } else {
                valid = 1;
            }
        }

        if (valid == 0) {
            offset = 0;
        }
    }

    current_state = param_1;
    animation_data->GotoFrame(*(int*)((char*)ranges + param_1 * 8) + offset);

    int* range = (int*)(current_state * 8 + (int)ranges);
    if (range[1] == range[0]) {
        flags |= 4;
    }
    flags &= ~0x20;
}

/* Function start: 0x41d300 */
int Sprite::Do(int x, int y, double scale)
{
    int skipAnimLoop;
    int done;

    if (current_state == -1) {
        return 1;
    }
    skipAnimLoop = 0;
    done = 0;
    if (CheckConditions() == 0) {
        return 1;
    }
    if ((flags & 0x80) != 0) {
        return 1;
    }
    if ((animation_data == 0) || (animation_data->data == 0)) {
        Init();
    }
    int* range = (int*)(current_state * 8 + (int)ranges);
    if (range[1] == range[0]) {
        int f = flags;
        skipAnimLoop = 1;
        if ((f & 4) != 0) {
            skipAnimLoop = 0;
            flags = f & ~4;
        }
        done = 1;
    }
    if (skipAnimLoop == 0) {
        animation_data->DoFrame();
        Animation* anim = animation_data;
        int remaining;
        if (anim == 0) {
            remaining = *(int*)((int)ranges + 4 + current_state * 8);
        }
        else {
            remaining = *(int*)((int)ranges + 4 + current_state * 8) - animation_data->smk->FrameNum;
        }
        if (remaining == 1) {
            if ((flags & 0x200) == 0) {
                animation_data->GotoFrame(*(int*)((int)ranges + current_state * 8));
            }
            else {
                animation_data->NextFrame();
            }
            if ((flags & 1) == 0) {
                done = 1;
            }
        }
        else {
            animation_data->NextFrame();
        }
    }
    if ((flags & 0x100) != 0) {
        return done;
    }
    if ((flags & 2) == 0) {
        Animation* anim = animation_data;
        if ((flags & 8) == 0) {
            if (anim == 0) {
                y = y - 1;
            }
            else {
                y = y + animation_data->data->height - 1;
            }
        }
        else if (anim == 0) {
            y = y + (int)(-scale);
        }
        else {
            y = y + (int)((animation_data->data->height - 1) * scale);
        }
    }
    int mode = 0;
    if ((flags & 8) == 0) {
        if ((flags & 0x40) != 0) {
            mode = 1;
        }
    }
    else {
        mode = ((flags & 0x40) < 1 ? -1 : 0) + 3;
    }
    //DAT_0043698c->PlayAnimationSound(animation_data->data, priority, x, y, mode, *(int*)&scale, *((int*)&scale + 1));
    return -((flags & 1) == 0) & done;
}

/* Function start: 0x41D500 */
void Sprite::CheckRanges1()
{
    if (animation_data == 0) {
        ShowError("error Sprite::CheckRanges0");
    }
    if (ranges == 0) {
        ShowError("error Sprite::CheckRanges1");
    }
    /*if (num_states > 0) {
        for (int i = 0; i < num_states; i++) {
            Range *range = &ranges[i];
            int iVar1 = animation_data->smk->FrameNum;
            if (iVar1 < range->end) {
                range->end = iVar1;
            }
            if (range->end < range->start) {
                ShowError("bad range[%d].start = %d in %s", i, range->start, sprite_filename);
            }   
        }
    }*/

    int iVar4 = 0;
    if (0 < num_states) {
        int iVar3 = 0;
        do {
            int* piVar2 = (int*)((char*)ranges + 4 + iVar3);
            int iVar1 = animation_data->smk->FrameNum;
            if (iVar1 < *piVar2) {
                *piVar2 = iVar1;
            }
            piVar2 = (int*)((char*)ranges + iVar3);
            iVar1 = *piVar2;
            if (piVar2[1] < iVar1) {
                ShowError("bad range[%d].start = %d in %s", iVar4, iVar1, sprite_filename);
            }
            iVar3 += 8;
            iVar4++;
        } while (iVar4 < num_states);
    }
}


/* Function start: 0x41d590 */
int Sprite::CheckConditions()
{
    int iVar1;
    LogicCondition* piVar2;

    if ((num_logic_conditions == 0) || (logic_conditions == 0)) {
        return 1;
    }
    if (num_logic_conditions > 0) {
        for (int i = 0; i < num_logic_conditions; i++) {
            piVar2 = &logic_conditions[i];
            if (piVar2->type == 1) {
                iVar1 = piVar2->state_index;
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] == 0) {
                    return 0;
                }
            }
            if (piVar2->type == 2) {
                iVar1 = piVar2->state_index;
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] != 0) {
                    return 0;
                }
            }
            if (piVar2->type == 3) {
                iVar1 = piVar2->state_index;
                if ((0 < iVar1) && (g_GameState_00436998->maxStates <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (g_GameState_00436998->stateValues[iVar1] != handle) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/* Function start: 0x41D6D0 */
void Sprite::SetRange(int param_1, int param_2, int param_3)
{
    if (num_states <= param_1) {
        ShowError("Sprite::SetRange#1 %s %d", sprite_filename, param_1);
    }
    if ((param_2 < 1) || (param_3 < 1)) {
        ShowError("Sprite::SetRange#2 %s %d range[%d, %d]", sprite_filename, param_1, param_2, param_3);
    }
    ranges[param_1].start = param_2;
    ranges[param_1].end = param_3;
    flags |= 0x20;
}

/* Function start: 0x41D740 */
void Sprite::SetState(int param_1)
{
    num_states = param_1;
    delete[] ranges;
    ranges = new Range[num_states];

    int edi = 5000;
    for (int i = 0; i < num_states; i++) {
        ranges[i].start = 1;
        ranges[i].end = edi;
    }
    flags |= 0x20;
}

/* Function start: 0x41D860 */
void Sprite::SetLogic(int param_1, int param_2)
{
    if (logic_conditions == 0) {
        InitLogic(1);
    }

    for (int i = 0; i < num_logic_conditions; i++) {
        if (logic_conditions[i].type == 0) {
            logic_conditions[i].state_index = param_1;
            logic_conditions[i].type = param_2;
            return;
        }
    }
    ShowError("Sprite::SetLogic %s", sprite_filename);
}

/* Function start: 0x41D8D0 */
void Sprite::InitLogic(int param_1)
{
    if (logic_conditions != 0) {
        delete[] logic_conditions;
        logic_conditions = 0;
    }

    num_logic_conditions = param_1;
    logic_conditions = new LogicCondition[num_logic_conditions];

    int zero = 0;
    for (int i = 0; i < num_logic_conditions; i++) {
        logic_conditions[i].state_index = zero;
        logic_conditions[i].type = zero;
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

    sscanf(param_1, " %s ", local_a0);

    if (strcmp(local_a0, "FNAME") == 0) {
        sscanf(param_1, "%s %s", local_80, sprite_filename);
    } else if (strcmp(local_a0, "HANDLE") == 0) {
        sscanf(param_1, "%s %d", local_a0, &handle);
    } else if (strcmp(local_a0, "LOC") == 0) {
        sscanf(param_1, "%s %d %d", local_a0, &local_ac, &local_a4);
        loc_x = local_ac;
        loc_y = local_a4;
    } else if (strcmp(local_a0, "MAXLOGIC") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_ac);
        InitLogic(local_ac);
    } else if (strcmp(local_a0, "LOGIC") == 0) {
        sscanf(param_1, "%s %s", local_a0, local_80);
        local_ac = g_GameState_00436998->FindState(local_80);
        if ((0 < local_ac) && (g_GameState_00436998->maxStates <= local_ac)) {
            ShowError("GameState Error  #%d", 1);
        }
        if (strstr(param_1, "FALSE") != 0) {
            SetLogic(local_ac, 2);
        } else if (strstr(param_1, "TRUE") != 0) {
            SetLogic(local_ac, 1);
        } else if (strstr(param_1, "EQUAL") != 0) {
            SetLogic(local_ac, 3);
            sscanf(param_1, "%s %s %d", local_a0, local_80, &handle);
        } else {
            ShowError("illegal %s", param_1);
        }
    } else if (strcmp(local_a0, "STATES") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_a8);
        SetState(local_a8);
    } else if (strcmp(local_a0, "RANGE") == 0) {
        sscanf(param_1, "%s %d %d %d", local_a0, &local_a8, &local_ac, &local_a4);
        SetRange(local_a8, local_ac, local_a4);
    } else if (strcmp(local_a0, "PRIORITY") == 0) {
        sscanf(param_1, "%s %d", local_a0, &local_a8);
        priority = local_a8;
    } else if (strcmp(local_a0, "TRANSPARENT") == 0) {
        flags |= 0x40;
    } else if (strcmp(local_a0, "BOTTOMLEFT") == 0) {
        flags |= 2;
    } else if (strcmp(local_a0, "TOPLEFT") == 0) {
        flags &= ~2;
    } else if (strcmp(local_a0, "KEEPOFFSET") == 0) {
        flags |= 0x10;
    } else if (strcmp(local_a0, "INVERT") == 0) {
        flags |= 0x400;
    } else if (strcmp(local_a0, "SCALE") == 0) {
        flags |= 8;
    } else if (strcmp(local_a0, "LOOP") == 0) {
        flags |= 1;
    } else if (strcmp(local_a0, "NOGRAPHIC") == 0) {
        flags |= 0x80;
    } else if (strcmp(local_a0, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("Sprite");
    }
    return 0;
}

/* Function start: 0x41E010 */
void Sprite::Dump()
{
    int i;
    WriteToMessageLog("FNAME: %s", sprite_filename);
    WriteToMessageLog("HANDLE: %d", handle);
    WriteToMessageLog("LOC: %d %d", loc_x, loc_y);
    WriteToMessageLog("PRIORITY: %d", priority);
    if (logic_conditions != 0) {
        if (num_logic_conditions > 1) {
            WriteToMessageLog("MAXLOGIC: %d", num_logic_conditions);
        }
        
        for (i = 0; i < num_logic_conditions; i++) {
            if (logic_conditions[i].type == 1) {
                WriteToMessageLog("LOGIC: %s TRUE", g_GameState_00436998->GetState(logic_conditions[i].state_index));
            } else if (logic_conditions[i].type == 2) {
                WriteToMessageLog("LOGIC: %s FALSE", g_GameState_00436998->GetState(logic_conditions[i].state_index));
            }
        }
    }
    if ((flags & 0x40) != 0) {
        WriteToMessageLog("TRANSPARENT");
    }
    if ((flags & 2) != 0) {
        WriteToMessageLog("BOTTOMLEFT");
    }
    if ((flags & 0x10) != 0) {
        WriteToMessageLog("KEEPOFFSET");
    }
    if ((flags & 8) != 0) {
        WriteToMessageLog("SCALE");
    }
    if ((flags & 1) != 0) {
        WriteToMessageLog("LOOP");
    }
    if ((flags & 0x80) != 0) {
        WriteToMessageLog("NOGRAPHIC");
    }
    WriteToMessageLog("STATES: %d", num_states);
    for (i = 0; i < num_states; i++) {
        WriteToMessageLog("RANGE: %d %d %d", i, ranges[i].start, ranges[i].end);
    }
    WriteToMessageLog("END");
}
