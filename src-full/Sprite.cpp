#include <stdio.h>
#include <string.h>

#include <string.h>

#include "globals.h"
#include "Sprite.h"
#include "Parser.h"
#include "GameState.h"
#include "Animation.h"
#include "Memory.h"
#include "ZBufferManager.h"
#include "string.h"

#include "VBuffer.h"

/* Function start: 0x44C660 */ /* ~82% match */
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

/* Function start: 0x44C740 */ /* ~82% match */
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

/* Function start: 0x41CF10 */ /* DEMO ONLY - no full game match */
void Sprite::Init()
{
    Animation* anim;

    anim = animation_data;
    if (anim != 0) {
        if (anim->targetBuffer != 0) {
            return;
        }
    }
    if (anim == 0) {
        anim = new Animation(sprite_filename);
        animation_data = anim;
    }

    if (animation_data->targetBuffer == 0) {
        animation_data->ToBuffer();
    }

    if (animation_data->targetBuffer != 0) {
        if (g_SpriteTableInitialized_00436b9c == 0) {
            char* ptr = g_SpriteFilenameTable_0043d630;
            do {
                *ptr = 0;
                ptr += 0x40;
            } while (ptr < &g_SpriteFilenameTable_0043d630[0x800]);
            g_SpriteTableInitialized_00436b9c = 1;
        }
        strcpy(&g_SpriteFilenameTable_0043d630[animation_data->targetBuffer->handle * 0x40], sprite_filename);
    }

    CheckRanges1();
    flags |= 0x20;
    SetState2(current_state);
}

/* Function start: 0x44C9D0 */ /* ~92% match */
void Sprite::StopAnimationSound()
{
    Animation* anim = animation_data;
    int sound_idx;

    if (anim == 0 || anim->targetBuffer == 0) {
        sound_idx = -1;
    } else {
        sound_idx = anim->targetBuffer->handle;
    }

    if (g_ZBufferManager_0043698c != 0 && anim != 0 && g_ZBufferManager_0043698c->m_state != 0) {
        g_ZBufferManager_0043698c->QueueAnimationCleanup(anim);
    } else if (anim != 0) {
        delete anim;
    }

    animation_data = 0;

    if (sound_idx != -1) {
        g_SpriteFilenameTable_0043d630[sound_idx * 0x40] = 0;
    }
    flags |= 0x20;
}

/* Function start: 0x41D0C0 */ /* DEMO ONLY - no full game match */
void Sprite::InitAnimation()
{
    if (animation_data != 0) {
        return;
    }

    animation_data = new Animation();
    animation_data->Open(sprite_filename, 0xfe000, -1);
}

/* Function start: 0x44CB10 */ /* ~86% match */
void Sprite::FreeAnimation()
{
    if (animation_data != 0 && animation_data->targetBuffer != 0) {
        if (animation_data->targetBuffer->handle != -1) {
            g_SpriteFilenameTable_0043d630[animation_data->targetBuffer->handle * 0x40] = 0;
        }
        animation_data->FreeVBuffer();
    }
}

/* Function start: 0x41D190 */ /* DEMO ONLY - no full game match */
void Sprite::SetState2(int param_1)
{
    int offset;
    int current_frame;
    int in_range;
    Range* range_ptr;
    int new_start;
    int new_end_frame;

    if (param_1 == -1) {
        current_state = param_1;
        return;
    }

    offset = 0;

    if ((animation_data == 0) || (animation_data->targetBuffer == 0)) {
        Init();
    }

    if (param_1 < 0 || num_states - 1 < param_1) {
        ShowError("Sprite::SetState 0 %d %s", param_1, sprite_filename);
    }

    current_frame = 0;
    if (animation_data != 0) {
        current_frame = animation_data->smk->FrameNum;
    }

    if (animation_data == 0 || ranges == 0) {
        ShowError("range error");
        in_range = 0;
    } else {
        range_ptr = &ranges[param_1];
        if (range_ptr->start > current_frame || range_ptr->end < current_frame) {
            in_range = 0;
        } else {
            in_range = 1;
        }
    }

    if (in_range == 0) {
        flags |= 0x20;
    }

    if ((flags & 0x20) == 0 && current_state == param_1) {
        return;
    }

    if ((flags & 0x10) != 0) {
        Range* rng = ranges;
        Animation* anim2 = animation_data;
        int old_state = current_state;
        offset = anim2->smk->FrameNum - rng[old_state].start;

        new_end_frame = rng[param_1].start + offset + 1;
        offset++;

        if (anim2 == 0 || rng == 0) {
            ShowError("range error");
            in_range = 0;
        } else if (new_end_frame < rng[param_1].start || rng[param_1].end < new_end_frame) {
            in_range = 0;
        } else {
            in_range = 1;
        }

        if (in_range == 0) {
            offset = 0;
        }
    }

    current_state = param_1;
    animation_data->GotoFrame(ranges[param_1].start + offset);

    range_ptr = &ranges[current_state];
    if (range_ptr->end == range_ptr->start) {
        flags |= 4;
    }
    flags &= ~0x20;
}

/* Function start: 0x41d300 */ /* DEMO ONLY - no full game match */
int Sprite::Do(int x, int y, double scale)
{
    int skipAnimLoop;
    int done;
    int remaining;
    int fl;

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
    if ((animation_data == 0) || (animation_data->targetBuffer == 0)) {
        Init();
    }
    if (ranges[current_state].end == ranges[current_state].start) {
        fl = flags;
        skipAnimLoop = 1;
        if ((fl & 4) != 0) {
            skipAnimLoop = 0;
            flags = fl & ~4;
        }
        done = 1;
    }
    if (skipAnimLoop == 0) {
        animation_data->DoFrame();
        if (animation_data == 0) {
            if (ranges[current_state].end != 1) {
                goto do_nextframe;
            }
        } else {
            remaining = ranges[current_state].end - animation_data->smk->FrameNum;
            if (remaining != 1) {
                goto do_nextframe;
            }
        }
        if ((flags & 0x200) != 0) {
            animation_data->NextFrame();
        } else {
            animation_data->GotoFrame(ranges[current_state].start);
        }
        if ((flags & 1) == 0) {
            done = 1;
        }
        goto after_anim;
do_nextframe:
        animation_data->NextFrame();
after_anim:
        ;
    }
    fl = flags;
    if ((fl & 0x100) != 0) {
        return done;
    }
    if ((fl & 2) == 0) {
        if ((fl & 8) != 0) {
            if (animation_data != 0) {
                y = y + (int)((animation_data->targetBuffer->height - 1) * scale);
            } else {
                y = y + (int)(-scale);
            }
        } else {
            if (animation_data != 0) {
                y = y + animation_data->targetBuffer->height - 1;
            } else {
                y = y - 1;
            }
        }
    }
    int mode = 0;
    if ((fl & 8) != 0) {
        fl &= 0x40;
        mode = (fl < 1) ? 2 : 3;
    } else if ((fl & 0x40) != 0) {
        mode = 1;
    }
    g_ZBufferManager_0043698c->PlayAnimationSound(animation_data->targetBuffer, priority, x, y, mode, scale);
    fl = flags & 1;
    if (fl != 0) {
        return 0;
    }
    return done;
}

/* Function start: 0x41D500 */ /* DEMO ONLY - no full game match */
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
            int* piVar2 = (int*)((char*)ranges + iVar3 + 4);
            int iVar1 = animation_data->smk->Frames;
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


/* Function start: 0x41d590 */ /* DEMO ONLY - no full game match */
int Sprite::CheckConditions()
{
    int state_idx;
    LogicCondition* cond;
    GameState* gs;
    int offset;

    if ((num_logic_conditions == 0) || (logic_conditions == 0)) {
        return 1;
    }
    offset = 0;
    for (int i = 0; i < num_logic_conditions; i++) {
        cond = (LogicCondition*)((char*)logic_conditions + offset);
        if (cond->type == 1) {
            state_idx = cond->state_index;
            gs = g_GameState_00436998;
            if ((0 < state_idx) && (gs->maxStates <= state_idx)) {
                ShowError("GameState Error  #%d", 1);
            }
            if (gs->stateValues[state_idx] == 0) {
                return 0;
            }
        }
        cond = (LogicCondition*)((char*)logic_conditions + offset);
        if (cond->type == 2) {
            state_idx = cond->state_index;
            gs = g_GameState_00436998;
            if ((0 < state_idx) && (gs->maxStates <= state_idx)) {
                ShowError("GameState Error  #%d", 1);
            }
            if (gs->stateValues[state_idx] != 0) {
                return 0;
            }
        }
        cond = (LogicCondition*)((char*)logic_conditions + offset);
        if (cond->type == 3) {
            state_idx = cond->state_index;
            gs = g_GameState_00436998;
            if ((0 < state_idx) && (gs->maxStates <= state_idx)) {
                ShowError("GameState Error  #%d", 1);
            }
            if (gs->stateValues[state_idx] != handle) {
                return 0;
            }
        }
        offset += 8;
    }
    return 1;
}

/* Function start: 0x41D6D0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41D740 */ /* DEMO ONLY - no full game match */
void Sprite::SetState(int param_1)
{
    num_states = param_1;
    delete[] ranges;
    ranges = new Range[num_states];

    int i = 0;
    int offset = 0;
    if (num_states > i) {
        int edi = 5000;
        do {
            Range* r = (Range*)((char*)ranges + offset);
            i++;
            offset += 8;
            r->start = 1;
            r->end = edi;
        } while (num_states > i);
    }
    flags |= 0x20;
}

/* Function start: 0x41D860 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41D8D0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41D9D0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41E010 */ /* DEMO ONLY - no full game match */
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
            }
            if (logic_conditions[i].type == 2) {
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
