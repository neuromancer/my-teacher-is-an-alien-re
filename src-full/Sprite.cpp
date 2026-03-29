#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <new.h>

#include "globals.h"
#include "Sprite.h"
#include "Parser.h"
#include "GameState.h"
#include "Animation.h"
#include "Memory.h"
#include "ZBufferManager.h"
#include "string.h"

#include "VBuffer.h"

extern "C" void WriteToLog(const char* format, ...);
extern "C" GameState* g_GameState_0046aa30;

// Video buffer name table: 32 entries × 64 bytes at 0x4734B0
static char g_VideoBufferNameTable[32][64];

/* Function start: 0x44C650 */
extern "C" char* GetVideoBufferNameSlot(int handle)
{
    return g_VideoBufferNameTable[handle];
}

/* Function start: 0x44C660 */ /* ~82% match */
Sprite::Sprite(char* filename)
{
    // BaseObject::BaseObject_Init((undefined4 *)this);
    loc_x = 0;
    loc_y = 0;
    memset(&ranges, 0, 0x1A * 4);
    flags |= 0x20;
    if (filename != 0) {
        sscanf(filename, "%s", sprite_filename);
    }
    ConfigStates(1);
    priority = 0;
    ConfigRange(0, 1, 5000, 1);
}

/* Function start: 0x44C740 */
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

    if (g_ZBufferManager_0046aa24 != 0 && anim != 0 && g_ZBufferManager_0046aa24->m_state != 0) {
        g_ZBufferManager_0046aa24->QueueAnimationCleanup(anim);
    } else if (anim != 0) {
        delete anim;
    }

    animation_data = 0;

    if (sound_idx != -1) {
        g_VideoBufferNameTable[sound_idx][0] = 0;
    }
    flags |= 0x20;
}

extern char* ResolveAssetPath(char*);
extern "C" char* FormatAssetPath(char*, ...);

/* Function start: 0x44C880 */
void Sprite::InitAnimation()
{
    Animation* anim;

    anim = animation_data;
    if (anim != 0 && anim->targetBuffer != 0) {
        return;
    }

    anim = 0;
    if (animation_data == 0) {
        anim = new Animation(
            (flags & 0x400) != 0
                ? FormatAssetPath(sprite_filename)
                : ResolveAssetPath(sprite_filename)
        );
        animation_data = anim;
    }

    if (animation_data->targetBuffer == 0) {
        animation_data->ToBuffer();
    }

    if (animation_data->targetBuffer != 0) {
        if (g_SpriteTableInitialized_00436b9c == 0) {
            char* ptr = (char*)g_VideoBufferNameTable;
            do {
                *ptr = 0;
                ptr += 0x40;
            } while (ptr < (char*)g_VideoBufferNameTable + 0x800);
            g_SpriteTableInitialized_00436b9c = 1;
        }
        strcpy(g_VideoBufferNameTable[animation_data->targetBuffer->handle], sprite_filename);
    }

    CheckRanges1();
    flags |= 0x20;
    ResetAnimation(handle, 0);
}

/* Function start: 0x44CB10 */
void Sprite::FreeAnimation()
{
    if (animation_data == 0) {
        return;
    }
    if (animation_data->targetBuffer == 0) {
        return;
    }
    if (animation_data->targetBuffer->handle != -1) {
        g_VideoBufferNameTable[animation_data->targetBuffer->handle][0] = '\0';
    }
    animation_data->FreeVBuffer();
}



/* Function start: 0x44CF50 */
void Sprite::CheckRanges1()
{
    if (animation_data == 0) {
        ShowError("error Sprite::CheckRanges0");
    }
    if (ranges == 0) {
        ShowError("error Sprite::CheckRanges1");
    }

    int i = 0;
    if (0 < num_states) {
        do {
            int maxFrames = animation_data->smk->Frames;
            if (maxFrames < ranges[i].dim.y) {
                ranges[i].dim.y = maxFrames;
            }
            if (ranges[i].dim.y < ranges[i].dim.x) {
                ShowError("bad range[%d].start = %d in %s", i, ranges[i].dim.x, sprite_filename);
            }
            i++;
        } while (i < num_states);
    }
}


/* Function start: 0x44CFE0 */
int Sprite::CheckConditions()
{
    int state_idx;
    GameState* gs;

    if ((num_logic_conditions == 0) || (logic_conditions == 0)) {
        return 1;
    }
    int offset = 0;
    int i = 0;
    if (0 < num_logic_conditions) {
        do {
            gs = g_GameState_0046aa30;
            int* cond = (int*)((char*)logic_conditions + offset);
            if (cond[1] == 8) {
                state_idx = cond[0];
                if (state_idx < 0 || g_GameState_0046aa30->maxStates - 1 < state_idx) {
                    ShowError("Invalid gamestate %d", state_idx);
                }
                if (gs->stateValues[state_idx] == 0) {
                    return 0;
                }
            }
            gs = g_GameState_0046aa30;
            cond = (int*)((char*)logic_conditions + offset);
            if (cond[1] == 9) {
                state_idx = cond[0];
                if (state_idx < 0 || g_GameState_0046aa30->maxStates - 1 < state_idx) {
                    ShowError("Invalid gamestate %d", state_idx);
                }
                if (gs->stateValues[state_idx] != 0) {
                    return 0;
                }
            }
            gs = g_GameState_0046aa30;
            cond = (int*)((char*)logic_conditions + offset);
            if (cond[1] == 0xC) {
                state_idx = cond[0];
                if (state_idx < 0 || g_GameState_0046aa30->maxStates - 1 < state_idx) {
                    ShowError("Invalid gamestate %d", state_idx);
                }
                if (gs->stateValues[state_idx] != *(int*)((char*)logic_conditions + offset + 8)) {
                    return 0;
                }
            }
            gs = g_GameState_0046aa30;
            cond = (int*)((char*)logic_conditions + offset);
            if (cond[1] == 0xD) {
                state_idx = cond[0];
                if (state_idx < 0 || g_GameState_0046aa30->maxStates - 1 < state_idx) {
                    ShowError("Invalid gamestate %d", state_idx);
                }
                if (gs->stateValues[state_idx] == *(int*)((char*)logic_conditions + offset + 8)) {
                    return 0;
                }
            }
            gs = g_GameState_0046aa30;
            cond = (int*)((char*)logic_conditions + offset);
            if (cond[1] == 0x1C) {
                state_idx = cond[0];
                if (state_idx < 0 || g_GameState_0046aa30->maxStates - 1 < state_idx) {
                    ShowError("Invalid gamestate %d", state_idx);
                }
                GameState* gs2 = g_GameState_0046aa30;
                if (gs->stateValues[state_idx] != handle) {
                    int idx2 = *(int*)((char*)logic_conditions + offset);
                    if (idx2 < 0 || g_GameState_0046aa30->maxStates - 1 < idx2) {
                        ShowError("Invalid gamestate %d", idx2);
                    }
                    ResetAnimation(gs2->stateValues[idx2], 0);
                }
            }
            offset += 0xC;
            i++;
        } while (i < num_logic_conditions);
    }
    return 1;
}


/* Function start: 0x44D420 */
void Sprite::SetSpriteLogic(char* gsName, char* condition, int value) {
    int gsIdx = g_GameState_0046aa30->FindState(gsName);
    if (gsIdx < 0 || g_GameState_0046aa30->maxStates - 1 < gsIdx) {
        ShowError("Invalid gamestate %d", gsIdx);
    }
    if (strstr(condition, "FALSE") != 0) {
        SetLogic(gsIdx, 9);
    } else if (strstr(condition, "TRUE") != 0) {
        SetLogic(gsIdx, 8);
    } else if (strstr(condition, "FOLLOW") != 0) {
        SetLogic(gsIdx, 0x1c);
    } else if (strstr(condition, "NEQUAL") != 0) {
        SetLogic(gsIdx, 0xd, value);
    } else if (strstr(condition, "EQUAL") != 0) {
        SetLogic(gsIdx, 0xc, value);
    } else {
        ShowError("illegal %s", condition);
    }
}

/* Function start: 0x44D530 */
void Sprite::SetLogic(int param_1, int param_2, int param_3)
{
    if (logic_conditions == 0) {
        InitLogic(1);
    }

    int i = 0;
    if (0 < num_logic_conditions) {
        do {
            if (logic_conditions[i].type == 0) {
                logic_conditions[i].state_index = param_1;
                logic_conditions[i].type = param_2;
                logic_conditions[i].field_8 = param_3;
                return;
            }
            i++;
        } while (i < num_logic_conditions);
    }
    ShowError("Sprite::SetLogic %s", sprite_filename);
}

/* Function start: 0x44D5C0 */
void Sprite::InitLogic(int param_1)
{
    if (logic_conditions != 0) {
        delete[] logic_conditions;
        logic_conditions = 0;
    }

    num_logic_conditions = param_1;
    logic_conditions = new LogicCondition[param_1];

    int i = 0;
    int offset = 0;
    if (0 < num_logic_conditions) {
        do {
            logic_conditions[i].state_index = 0;
            logic_conditions[i].type = 0;
            logic_conditions[i].field_8 = 0;
            offset += 0xC;
            i++;
        } while (i < num_logic_conditions);
    }
}

extern char* MakeAnimName(int);
extern char* MakeSoundName(char*);
extern char* ResolveAssetPath(char*);

/* Function start: 0x44D6E0 */
void Sprite::OnProcessEnd() {
    if ((flags & 0x400) == 0) {
        ResolveAssetPath(sprite_filename);
    }
}

/* Function start: 0x44D700 */
int Sprite::LBLParse(char* param_1)
{
    char keyword[32];
    char buf1[64];
    char buf2[64];
    char buf3[128];
    int val1, val2, val3, val4, val5;
    int result;

    buf3[0] = 0;
    keyword[0] = 0;
    val2 = 0;
    val1 = 0;
    val3 = 0;

    sscanf(param_1, "%s", keyword);

    if (strcmp(keyword, "FNAME") == 0) {
        sscanf(param_1, "%s %s", buf3, sprite_filename);
    } else if (strcmp(keyword, "FINDEX") == 0) {
        val3 = sscanf(param_1, "%s %d", keyword, &val1);
        strcpy(sprite_filename, MakeAnimName(val1));
    } else if (strcmp(keyword, "MAKENAME") == 0) {
        val3 = sscanf(param_1, " %s %s", keyword, buf3);
        strcpy(sprite_filename, MakeSoundName((char*)buf3));
    } else if (strcmp(keyword, "HANDLE") == 0) {
        sscanf(param_1, "%s %d", keyword, &handleValue);
    } else if (strcmp(keyword, "LOC") == 0) {
        sscanf(param_1, " %s %d %d", keyword, &val1, &val2);
        loc_x = val1;
        loc_y = val2;
    } else if (strcmp(keyword, "MAXLOGIC") == 0) {
        sscanf(param_1, "%s %d", keyword, &val1);
        InitLogic(val1);
    } else if (strcmp(keyword, "LOGIC") == 0) {
        sscanf(param_1, "%s %s %s %d", keyword, buf3, buf1, &val2);
        SetSpriteLogic((char*)buf3, (char*)buf1, val2);
    } else if (strcmp(keyword, "STATES") == 0) {
        sscanf(param_1, "%s %d", keyword, &val3);
        ConfigStates(val3);
    } else if (strcmp(keyword, "RANGE") == 0) {
        val4 = 1;
        sscanf(param_1, "%s %d %d %d %d", keyword, &val3, &val1, &val2, &val4);
        ConfigRange(val3, val1, val2, val4);
    } else if (strcmp(keyword, "PRIORITY") == 0) {
        sscanf(param_1, "%s %d", keyword, &val3);
        priority = val3;
    } else if (strcmp(keyword, "SET_STATE") == 0) {
        sscanf(param_1, "%s %d", keyword, &val3);
        ResetAnimation(val3, 0);
    } else if (strcmp(keyword, "TRANSPARENT") == 0) {
        flags |= 0x40;
    } else if (strcmp(keyword, "BOTTOMLEFT") == 0) {
        flags |= 2;
    } else if (strcmp(keyword, "TOPLEFT") == 0) {
        flags &= ~2;
    } else if (strcmp(keyword, "KEEPOFFSET") == 0) {
        flags |= 0x10;
    } else if (strcmp(keyword, "INVERT") == 0) {
        flags |= 0x200;
    } else if (strcmp(keyword, "SCALE") == 0) {
        flags |= 8;
    } else if (strcmp(keyword, "LOOP") == 0) {
        flags |= 1;
    } else if (strcmp(keyword, "NOAUTOWRAP") == 0) {
        flags |= 0x100;
    } else if (strcmp(keyword, "NOGRAPHIC") == 0) {
        flags |= 0x80;
    } else if (strcmp(keyword, "CDROMSP") == 0) {
        flags |= 0x400;
    } else if (strcmp(keyword, "SET_DEFAULT_STATES") == 0) {
        int i = 0;
        if (0 < num_states) {
            do {
                i++;
                ranges[i - 1].dim.x = i;
                ranges[i - 1].dim.y = i;
                ranges[i - 1].repeatLimit = 0;
                ranges[i - 1].frameCounter = 0;
            } while (i < num_states);
        }
    } else if (strcmp(keyword, "SPR") == 0) {
        result = sscanf(param_1, " %s %s %d %d %d %s %s %d", keyword, buf3,
                        &val1, &val2, &val5, buf1, buf2, &val4);
        if (result < 4) {
            Parser::LBLParse("Sprite");
        }
        loc_x = val1;
        loc_y = val2;
        if (4 < result) {
            priority = val5;
        }
        if (6 < result) {
            SetSpriteLogic((char*)buf1, (char*)buf2, val4);
        }
    } else if (strcmp(keyword, "SPR_TRANS") == 0) {
        result = sscanf(param_1, " %s %s %d %d %d %s %s %d", keyword, buf3,
                        &val1, &val2, &val5, buf1, buf2, &val4);
        if (result < 4) {
            Parser::LBLParse("Sprite");
        }
        loc_x = val1;
        loc_y = val2;
        flags |= 0x40;
        if (4 < result) {
            priority = val5;
        }
        if (6 < result) {
            SetSpriteLogic((char*)buf1, (char*)buf2, val4);
        }
    } else if (strcmp(keyword, "SPR_LOOP") == 0) {
        result = sscanf(param_1, " %s %s %d %d %d %s %s %d", keyword, buf3,
                        &val1, &val2, &val5, buf1, buf2, &val4);
        if (result < 4) {
            Parser::LBLParse("Sprite");
        }
        loc_x = val1;
        loc_y = val2;
        flags |= 1;
        if (4 < result) {
            priority = val5;
        }
        if (6 < result) {
            SetSpriteLogic((char*)buf1, (char*)buf2, val4);
        }
    } else if (strcmp(keyword, "SPR_TRANS_LOOP") == 0) {
        result = sscanf(param_1, " %s %s %d %d %d %s %s %d", keyword, buf3,
                        &val1, &val2, &val5, buf1, buf2, &val4);
        if (result < 4) {
            Parser::LBLParse("Sprite");
        }
        loc_x = val1;
        loc_y = val2;
        flags |= 0x41;
        if (4 < result) {
            priority = val5;
        }
        if (6 < result) {
            SetSpriteLogic((char*)buf1, (char*)buf2, val4);
        }
    } else if (strcmp(keyword, "SPR_TRANS_P20") == 0) {
        result = sscanf(param_1, " %s %s %d %d %s %s %d", keyword, buf3,
                        &val1, &val2, buf1, buf2, &val4);
        if (result < 4) {
            Parser::LBLParse("Sprite");
        }
        loc_x = val1;
        loc_y = val2;
        flags |= 0x40;
        priority = 0x14;
        if (5 < result) {
            SetSpriteLogic((char*)buf1, (char*)buf2, val4);
        }
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("Sprite");
    }
    return 0;
}


/* Function start: 0x44CB40 */
void Sprite::ResetAnimation(int state, int offset) {
    int frameNum;
    int inRange;

    if (state == -1) {
        handle = state;
        return;
    }

    if (animation_data == 0 || animation_data->targetBuffer == 0) {
        InitAnimation();
    }

    if (state < 0 || num_states - 1 < state) {
        ShowError("Sprite::SetState - '%s' states are OutOfRange \nstate %d must be between 0 and %d",
                  sprite_filename, state, num_states - 1);
    }

    frameNum = 0;
    if (animation_data != 0) {
        frameNum = animation_data->smk->FrameNum;
    }

    if (animation_data == 0 || ranges == 0) {
        ShowError("range error");
        inRange = 0;
    } else if (ranges[state].dim.x > frameNum || ranges[state].dim.y < frameNum) {
        inRange = 0;
    } else {
        inRange = 1;
    }

    if (!inRange) {
        flags |= 0x20;
    }

    if ((flags & 0x20) == 0 && handle == state) {
        return;
    }

    if ((flags & 0x10) != 0) {
        offset = animation_data->smk->FrameNum - ranges[handle].dim.x + 1;
    }

    if (offset != 0) {
        int targetFrame;
        int rangeStart;
        rangeStart = ranges[state].dim.x;
        targetFrame = offset + rangeStart;
        if (animation_data == 0 || ranges == 0) {
            ShowError("range error");
            inRange = 0;
        } else if (rangeStart > targetFrame || ranges[state].dim.y < targetFrame) {
            inRange = 0;
        } else {
            inRange = 1;
        }
        if (!inRange) {
            offset = 0;
        }
    }

    handle = state;
    animation_data->GotoFrame(ranges[state].dim.x + offset);
    ranges[handle].frameCounter = 0;
    if (ranges[handle].dim.y == ranges[handle].dim.x) {
        flags |= 4;
    }
    flags &= ~0x20;
}

extern ZBufferManager* g_ZBufferManager_0046aa24;
extern "C" GameState* g_GameState_0046aa30;

/* Function start: 0x44CCF0 */
int Sprite::Do(int x, int y, double scale) {
    int done;
    int skipAnim;
    Range* cur;

    if (handle == -1) {
        return 1;
    }
    skipAnim = 0;
    done = 0;
    if (CheckConditions() == 0) {
        return 1;
    }

    if (animation_data == 0 || animation_data->targetBuffer == 0) {
        InitAnimation();
    }

    cur = &ranges[handle];

    if (cur->dim.x == cur->dim.y) {
        if ((flags & 4) != 0) {
            skipAnim = 0;
            flags &= ~4;
        } else {
            int hasLimit;
            skipAnim = 1;
            cur->frameCounter = cur->frameCounter + skipAnim;
            if (cur->repeatLimit == 0 || cur->frameCounter < cur->repeatLimit) {
                hasLimit = 0;
            } else {
                hasLimit = skipAnim;
            }
            if (hasLimit) {
                cur->frameCounter = 0;
                done = 1;
            }
        }
    }

    if (!skipAnim) {
        animation_data->DoFrame();
        if (animation_data != 0) {
            if (animation_data->smk->FrameNum - cur->dim.y != -1) {
                goto do_nextframe;
            }
        } else {
            if (cur->dim.y != 1) {
                goto do_nextframe;
            }
        }
        {
            int hasLimit;
            cur->frameCounter = cur->frameCounter + 1;
            if (cur->repeatLimit == 0 || cur->frameCounter < cur->repeatLimit) {
                hasLimit = 0;
            } else {
                hasLimit = 1;
            }
            if (hasLimit) {
                cur->frameCounter = 0;
                if ((flags & 0x100) != 0) {
                    animation_data->NextFrame();
                } else {
                    animation_data->GotoFrame(cur->dim.x);
                }
                if ((flags & 1) == 0) {
                    done = 1;
                }
            } else {
                animation_data->GotoFrame(cur->dim.x);
            }
        }
        goto after_anim;
do_nextframe:
        animation_data->NextFrame();
after_anim:
        ;
    }

    if ((flags & 0x80) != 0) {
        return done;
    }

    if ((flags & 2) == 0) {
        if ((flags & 8) != 0) {
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

    {
        int mode = 0;
        if ((flags & 8) != 0) {
            if (flags & 0x40) {
                mode = 5;
            } else {
                mode = 4;
            }
        } else if ((flags & 0x40) != 0) {
            if (flags & 0x200) {
                mode = 3;
            } else {
                mode = 2;
            }
        } else if ((flags & 0x200) != 0) {
            mode = 1;
        }

        VBuffer* vb = animation_data->targetBuffer;
        g_ZBufferManager_0046aa24->DrawVBufferRegion(vb, priority, x, y, mode, scale,
            vb->clip_x1, vb->clip_y1, vb->clip_x2, vb->clip_y2);

        return ((flags & 1) != 1) & done;
    }
}

/* Function start: 0x44D210 */
void Sprite::ConfigRange(int state, int start, int count, int param_4) {
    if (num_states <= state) {
        ShowError("Sprite::SetRange 1 %s %d", sprite_filename, state);
    }
    if (start < 1 || count < 1) {
        ShowError("Sprite::SetRange 2 %s %d range = %d, %d", sprite_filename, state, start, count);
    }
    ranges[state].dim.x = start;
    ranges[state].dim.y = count;
    ranges[state].repeatLimit = param_4;
    ranges[state].frameCounter = 0;
    flags |= 0x20;
}

/* Function start: 0x44D2A0 */
void Sprite::ConfigStates(int numStates) {
    num_states = numStates;

    if (ranges != 0) {
        delete[] ranges;
        ranges = 0;
    }

    ranges = new Range[num_states];

    int i = 0;
    if (num_states > 0) {
        int offset = 0;
        do {
            Range* r = (Range*)((char*)ranges + offset);
            i++;
            offset += sizeof(Range);
            r->dim.x = 1;
            r->dim.y = 5000;
            r->repeatLimit = 0;
            r->frameCounter = 0;
        } while (num_states > i);
    }
    flags |= 0x20;
}
