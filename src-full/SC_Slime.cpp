#include "SC_Slime.h"
#include "InputManager.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "SoundList.h"
#include "Sample.h"
#include "main.h"
#include "Memory.h"
#include "LinkedList.h"
#include <new.h>
#include <string.h>
#include "GameState.h"
extern "C" extern GameState* DAT_0046aa30;

extern "C" void SetVideoRes(int, int);
#include "string.h"
extern "C" void WriteToLog(const char* format, ...);
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;
extern int DAT_00468bbc;

#include "SlimeTable.h"

// TimerWrapper = TimeOut (ctor 0x421920 wraps 0x421960, dtor 0x421930)
#include "TimeOut.h"

/* Function start: 0x40CF60 */
SC_Slime::SC_Slime()
    : field_130(0), field_134(0), field_138(0), field_13C(0)
{
    memset(&field_A8, 0, 0xD8);
    handlerId = 0x3F;
}

/* Function start: 0x40D100 */
SC_Slime::~SC_Slime()
{
    ShutDown(0);
}

/* Function start: 0x40D1B0 */
void SC_Slime::Init(SC_Message* msg)
{
    int savedId = handlerId;
    memset(&field_A8, 0, 0xD8);
    handlerId = savedId;

    if (FileExists("CB_Slime") == 0) {
        ShowLoadingScreen();
    }

    CopyCommandData(msg);

    dimB0.field_0 = 0x140;
    dimB0.field_4 = 0xf0;
    field_AC = 0;

    SetVideoRes(0x140, 0xf0);

    if (field_170 != 0) {
        delete field_170;
        field_170 = 0;
    }

    field_17C = 7;
    field_170 = new int[7];
    memset(field_170, 0, field_17C * sizeof(int));

    SlimeTable* table = new SlimeTable();
    field_16C = (int)table;
    table->Init(0xC);

    moduleParam = ((int*)msg)[1];
    ParseFile(this, "mis\\cb_slime.mis", (char*)0);

    if (field_A8 == 0) {
        SpriteAction* action = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = (int)action;
    }

    if (palette != 0) {
        int* palSlot = (int*)((int)DAT_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = (int)palette;
    }

    field_170[0] = 0;
    field_170[1] = 0;
    field_170[2] = 0;
    field_170[3] = 1;
    field_170[4] = 1;
    field_170[5] = 1;
    field_170[6] = 0;

    TimeOut* timer = new TimeOut();
    field_B8 = (int)timer;
}

/* Function start: 0x40D430 */
int SC_Slime::ShutDown(SC_Message* msg)
{
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    if (spriteC4 != 0) {
        delete spriteC4;
        spriteC4 = 0;
    }

    if (DAT_00468bbc != 0) {
        int* obj = (int*)DAT_00468bbc;
        int* vtbl = (int*)*obj;
        void (__fastcall *fn)(int*, int, int) = (void (__fastcall *)(int*, int, int))vtbl[3];
        fn(obj, 0, 1);
        DAT_00468bbc = 0;
    }

    if (spriteBC != 0) {
        delete spriteBC;
        spriteBC = 0;
    }
    if (field_B8 != 0) {
        delete (TimeOut*)field_B8;
        field_B8 = 0;
    }
    if (spriteC8 != 0) {
        delete spriteC8;
        spriteC8 = 0;
    }
    field_D0 = 0;
    if (spriteCC != 0) {
        delete spriteCC;
        spriteCC = 0;
    }
    field_D4 = 0;
    if (sprite118 != 0) {
        delete sprite118;
        sprite118 = 0;
    }
    sprite120 = 0;
    if (sprite11C != 0) {
        delete sprite11C;
        sprite11C = 0;
    }
    sprite124 = 0;
    if (sprite128 != 0) {
        delete sprite128;
        sprite128 = 0;
    }
    if (sprite12C != 0) {
        delete sprite12C;
        sprite12C = 0;
    }
    if (field_A8 != 0) {
        delete (SpriteAction*)field_A8;
        field_A8 = 0;
    }
    if (field_16C != 0) {
        ((SlimeTable*)field_16C)->~SlimeTable();
        delete (void*)field_16C;
        field_16C = 0;
    }
    if (field_150 != 0) {
        ((SoundList*)field_150)->~SoundList();
        delete (void*)field_150;
        field_150 = 0;
    }
    field_168 = 0;
    field_164 = 0;
    field_160 = 0;
    field_15C = 0;
    field_158 = 0;
    if (field_170 != 0) {
        delete field_170;
        field_170 = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x40DF30 */
void SC_Slime::ProcessSprite(Sprite* spr) {
    if (spr == 0) {
        goto render_timer;
    }
    if (spr->handle == -1) {
        goto render_timer;
    }

    if (spr == 0) {
        goto render_timer;
    }

    if (spr->Do(spr->num_states, spr->field_0xb0, 1.0) == 0) {
        goto render_timer;
    }

    switch (spr->handle) {
    case 0:
        field_130++;
        {
            int done;
            if (field_134 != 0 && field_130 >= field_134) {
                done = 1;
            } else {
                done = 0;
            }
            if (done != 0) {
                field_170[0] = 1;
                goto update_progress;
            }
        }

        if (field_168 != 0) {
            ((Sample*)field_168)->Play(100, 1);
        }
        spriteBC->ResetAnimation(8, 0);

    update_progress:
        {
            int progress = (field_130 * 8) / field_134;
            if (progress >= 7) {
                progress = 7;
            }
            sprite12C->ResetAnimation(progress, 0);
        }
        spr->ResetAnimation(1, 0);
        break;

    case 1:
    case 2:
        spr->ResetAnimation(-1, 0);
        break;

    default:
        break;
    }

render_timer:
    if (sprite128 == 0) {
        return;
    }
    if (sprite128->Do(sprite128->num_states, sprite128->field_0xb0, 1.0) == 0) {
        return;
    }
    sprite128->ResetAnimation(-1, 0);
}

/* Function start: 0x40E070 */
void SC_Slime::UpdateArmSprites()
{
    Sprite* spr = sprite120;
    int status = *(int*)((char*)spr + 0x98);
    if (status == 0 ||
        (spr = sprite124, status = *(int*)((char*)spr + 0x98), status == 0)) {
        int idx = status << 4;
        int base = *(int*)((char*)spr + 0x90);
        int height = *(int*)(base + idx + 4);
        int fdata = *(int*)((char*)spr + 0xf0);
        int subdata = *(int*)(fdata + 0xc);
        height -= *(int*)(subdata + 0x374);
        spr->ResetAnimation(2, height);
    }
    sprite128->ResetAnimation(-1, 0);
}

/* Function start: 0x40D720 */
int SC_Slime::AddMessage(SC_Message* msg)
{
    WriteMessageAddress(msg);
    if (*(int*)((char*)msg + 0x2c) == 0x1b) {
        field_170[2] = 1;
    }
    return 1;
}

/* Function start: 0x40E0D0 */
void SC_Slime::ResetSprites()
{
    if (sprite120 != 0) {
        sprite120->ResetAnimation(-1, 0);
    }
    if (sprite124 != 0) {
        sprite124->ResetAnimation(-1, 0);
    }
    if (sprite128 != 0) {
        sprite128->ResetAnimation(-1, 0);
    }

    invSlots[0].left = 0x40;
    invSlots[0].top = 0x18;
    invSlots[0].right = 0x60;
    invSlots[0].bottom = 0x48;
    invSlots[1].left = 0xdd;
    invSlots[1].top = 0x19;
    invSlots[1].right = 0xfd;
    invSlots[1].bottom = 0x49;
    invSlot3.left = 0x4c;
    invSlot3.top = 0x4d;
    invSlot3.right = 0xfb;
    invSlot3.bottom = 0xb9;

    extern InputManager* DAT_0046aa08;
    if (DAT_0046aa08 != 0) {
        (DAT_0046aa08)->Refresh(1);
    }

    if (spriteBC != 0) {
        extern InputManager* DAT_0046aa08;
        int mouseX = 0;
        int* pMouse = *(int**)((char*)DAT_0046aa08 + 0x1a0);
        if (pMouse != 0) {
            mouseX = *pMouse;
        }
        int div = dimB0.field_0 / 5;
        spriteBC->ResetAnimation(mouseX / div, 0);
    }
}

/* Function start: 0x40D750 */
int SC_Slime::Exit(SC_Message* msg)
{
    int id = handlerId;
    if (*(int*)msg != id) {
        return 0;
    }

    int action = *(int*)((char*)msg + 0x10);
    switch (action) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, id, id, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x40D7C0 */
void SC_Slime::SendResultMessage() {
    int idx;
    int* gameResult;
    if (savedCommand != 0x2B) {
        // Non-practice mode
        gameResult = (int*)field_170;
        if (gameResult[0] != 0) {
            // Won — send room switch + add 20 to NUM_ACTIONS
            ((int*)field_A8)[0] = 0x2C;
            ((int*)field_A8)[1] = 1;
            goto enqueue;
        }
        if (gameResult[2] != 0) {
            // Alternate win — send room switch + add 30 to NUM_ACTIONS
            ((int*)field_A8)[0] = 0x20;
            ((int*)field_A8)[1] = 2;
            idx = DAT_0046aa30->FindState("NUM_ACTIONS");
            if (idx < 0 || DAT_0046aa30->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            DAT_0046aa30->stateValues[idx] += 0x14;
            goto enqueue;
        }
        // Lost — add 30 to NUM_ACTIONS
        idx = DAT_0046aa30->FindState("NUM_ACTIONS");
        if (idx < 0 || DAT_0046aa30->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        DAT_0046aa30->stateValues[idx] += 0x1E;

    enqueue:
        EnqueueSpriteAction((void*)field_A8);
        if (field_A8 != 0) {
            ((SpriteAction*)field_A8)->~SpriteAction();
            FreeMemory((void*)field_A8);
            field_A8 = 0;
        }
        return;
    }

    // Practice mode (savedCommand == 0x2B)
    gameResult = (int*)field_170;
    if (gameResult[1] != 0) {
        // Lost in practice
        if (field_A8 != 0) {
            ((SpriteAction*)field_A8)->~SpriteAction();
            FreeMemory((void*)field_A8);
            field_A8 = 0;
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        field_A8 = (int)action;
        Parser temp;
        ParseFile(&temp, "mis\\cb_slime.mis", "[WIN_LBL_PR]");
        goto enqueue;
    }

    if (gameResult[0] != 0) {
        // Won in practice
        if (field_A8 != 0) {
            ((SpriteAction*)field_A8)->~SpriteAction();
            FreeMemory((void*)field_A8);
            field_A8 = 0;
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        field_A8 = (int)action;
        Parser temp;
        ParseFile(&temp, "mis\\cb_slime.mis", "[LOSE_LBL_PR]");
        goto enqueue;
    }

    goto enqueue;
}

// Stubs (moved from stubs.cpp)
void SC_Slime::Update(int, int) {}

/* Function start: 0x40DEB0 */
void SC_Slime::CheckTimerExpired(Sprite* spr)
{
    if (spr == 0) {
        return;
    }

    if (spr->Do(spr->num_states, spr->field_0xb0, 1.0) == 0) {
        return;
    }

    if (spr->handle != 4) {
        return;
    }

    if (((Sprite*)field_D0)->handle != 4) {
        return;
    }

    if (((Sprite*)field_D4)->handle != 4) {
        return;
    }

    spriteC4->ResetAnimation(2, 0);
    field_170[1] = 1;
}

/* Function start: 0x40D0D0 */
SlotPair::SlotPair() { field_0 = 0; field_4 = 0; }

int DAT_00468bbc = 0;

/* Function start: 0x425480 */
SlimeTable::SlimeTable()
{
    fields[0] = 0;
    fields[1] = 0;
    fields[2] = 0;
}

/* Function start: 0x425490 */
SlimeTable::~SlimeTable()
{
    Cleanup();
}

void SlimeTable::Init(int count)
{
    Allocate(count);
}

/* Function start: 0x425620 */
void SlimeTable::LoadEntry(int index, char* filename, int value)
{
    if (index < 0 || fields[0] - 1 < index) {
        return;
    }

    Sample* sample = new Sample();
    ((void**)fields[1])[index] = sample;
    sample->Load(MakeAudioName(filename));
    ((int*)fields[2])[index] = value;
}

/* Function start: 0x4254F0 */
void SlimeTable::Cleanup()
{
    int i;

    for (i = 0; i < fields[0]; i++) {
        void* entry = ((void**)fields[1])[i];
        if (entry != 0) {
            ((Sample*)entry)->Unload();
            operator delete(entry);
            ((void**)fields[1])[i] = 0;
        }
    }

    if ((void*)fields[1] != 0) {
        operator delete((void*)fields[1]);
        fields[1] = 0;
    }

    fields[0] = 0;
}

/* Function start: 0x4254A0 */
void SlimeTable::Allocate(int count)
{
    Cleanup();
    fields[0] = count;
    fields[1] = (int)operator new(count * 4);
    memset((void*)fields[1], 0, fields[0] * 4);

    fields[2] = (int)operator new(fields[0] * 4);
    memset((void*)fields[2], 0, fields[0] * 4);
}
