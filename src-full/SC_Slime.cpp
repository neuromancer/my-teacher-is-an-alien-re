#include "SC_Slime.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include "LinkedList.h"
#include <string.h>

extern "C" void __cdecl FUN_004309c0(void*);
extern "C" void __cdecl FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" int FUN_00425fa0(char*);
extern "C" void FUN_004265a0();
extern void __cdecl FUN_00425a90(int, int);
extern void __cdecl FUN_00425d70(char*);
extern void* DAT_0046aa24;

// SpriteAction - 0x38-byte object, constructor at 0x00444a40
class SpriteAction {
    int field_0[14];
public:
    SpriteAction(int, int, int, int, int, int, int, int, int, int);
    ~SpriteAction();
};

// SlimeTable - 12-byte object, constructor at 0x00425480
class SlimeTable {
    int fields[3];
public:
    SlimeTable();
    ~SlimeTable();
    void Init(int);
};

// TimerWrapper - 12-byte object, constructor at 0x00421920
class TimerWrapper {
    int fields[3];
public:
    TimerWrapper();
    ~TimerWrapper();
};

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

    if (FUN_00425fa0("CB_Slime") == 0) {
        FUN_004265a0();
    }

    CopyCommandData(msg);

    dimB0.field_0 = 0x140;
    dimB0.field_4 = 0xf0;
    field_AC = 0;

    FUN_00425a90(0x140, 0xf0);

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
            FUN_00425d70("ddouble palette");
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

    TimerWrapper* timer = new TimerWrapper();
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

    extern int DAT_00468bbc;
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
        extern void __fastcall FUN_00421930(void*);
        FUN_00421930((void*)field_B8);
        delete (void*)field_B8;
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
        extern void __fastcall FUN_00444af0(void*);
        FUN_00444af0((void*)field_A8);
        delete (void*)field_A8;
        field_A8 = 0;
    }
    if (field_16C != 0) {
        extern void __fastcall FUN_00425490(void*);
        FUN_00425490((void*)field_16C);
        delete (void*)field_16C;
        field_16C = 0;
    }
    if (field_150 != 0) {
        extern void __fastcall FUN_00425200(void*);
        FUN_00425200((void*)field_150);
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
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
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
    FUN_004309c0(msg);
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

    invSlots[0].field_0 = 0x40;
    invSlots[0].field_4 = 0x18;
    invSlots[0].field_8 = 0x60;
    invSlots[0].field_C = 0x48;
    invSlots[1].field_0 = 0xdd;
    invSlots[1].field_4 = 0x19;
    invSlots[1].field_8 = 0xfd;
    invSlots[1].field_C = 0x49;
    invSlot3.field_0 = 0x4c;
    invSlot3.field_4 = 0x4d;
    invSlot3.field_8 = 0xfb;
    invSlot3.field_C = 0xb9;

    extern void* DAT_0046aa08;
    if (DAT_0046aa08 != 0) {
        extern void __fastcall FUN_00426ce0(void*, int, int);
        FUN_00426ce0(DAT_0046aa08, 0, 1);
    }

    if (spriteBC != 0) {
        extern void* DAT_0046aa08;
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
        FUN_00444d90(1, id, id, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}
