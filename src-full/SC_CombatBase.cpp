#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "Sprite.h"
#include "GameState.h"
#include "Memory.h"
#include "Viewport.h"
#include "Target.h"
#include <string.h>

class InputManager;
extern InputManager* DAT_0046aa08;
extern "C" extern GameState* DAT_0046aa30;
extern "C" extern void* DAT_0046ae4c;
extern "C" extern void* DAT_0046ae50;
extern "C" extern void* DAT_0046ae54;
extern "C" extern void* DAT_0046ae58;
extern "C" extern void* DAT_0046ae5c;
extern "C" extern void* DAT_0046ae60;
extern "C" extern void* DAT_0046ae64;
extern "C" extern void* DAT_0046ae68;
extern "C" extern void* DAT_0046ae6c;
extern "C" extern void* DAT_0046ae70;
extern "C" extern void* DAT_0046ae74;
extern void __fastcall FUN_00409730(void*, int, int);
extern void* __fastcall FUN_00443660(void*, int);
extern void __fastcall FUN_00442940(void*, int, int);
extern int __fastcall FUN_0044be70(void*, int);
extern int FUN_0042c9d0(void*);

/* Function start: 0x42BCD0 */
SC_CombatBase::SC_CombatBase()
{
    field_0xBC = 0;
    field_0xC0 = 0;
    field_0xC4 = 0;
    field_0xC8 = 0;
    field_0xCC = 0;
    field_0xD0 = 0;
    field_0xD4 = 0;
    field_0xD8 = 0;
    memset(&field_0x90, 0, 0x18 * 4);
    Initialize();
}

/* Function start: 0x42BDA0 */
SC_CombatBase::~SC_CombatBase()
{
    CleanupAll();
}

/* Function start: 0x42BF20 */
int SC_CombatBase::StopAndCleanup()
{
    method8();

    if (FUN_0042c9d0((void*)this) != 0) {
        return 1;
    }

    if (FUN_0044be70(DAT_0046ae70, 0) != 0) {
        return 1;
    }

    if (method9() != 0) {
        return 1;
    }

    method10();

    GameState* gs = DAT_0046aa30;
    if (gs->maxStates - 1 < 4) {
        ShowError("Invalid gamestate %d", 4);
    }
    if (gs->stateValues[4] != 0) {
        ProcessInput();
    }

    field_0xE0++;
    field_0xE8++;
    return 0;
}

/* Function start: 0x42BFC0 */
void SC_CombatBase::method10()
{
    void* target = FUN_00443660(DAT_0046ae58, 0);

    int* obj60 = (int*)DAT_0046ae60;
    int* vtbl60 = (int*)*obj60;
    ((void (__fastcall *)(int*, int))vtbl60[5])(obj60, 0);

    int* pMouse = *(int**)((char*)DAT_0046aa08 + 0x1a0);
    if (pMouse == 0) {
        ((int*)DAT_0046ae60)[0x2a] = 0;
    } else {
        int clicked = (*(int*)(pMouse + 4) >= 2) ? 1 : 0;
        ((int*)DAT_0046ae60)[0x2a] = clicked;
    }

    if (((int*)DAT_0046ae60)[0x2a] != 0) {
        int* vtbl60b = (int*)*(int*)DAT_0046ae60;
        ((void (__fastcall *)(int*, int))vtbl60b[4])((int*)DAT_0046ae60, 0);
        if (target != 0) {
            FUN_00442940(target, 0, 1);
        }
    }

    method11();
    method12();
}

/* Function start: 0x42C050 */
int SC_CombatBase::method9()
{
    if (DAT_0046ae5c != 0) {
        FUN_00409730(DAT_0046ae5c, 0, field_0xE8);
    }
    return 0;
}

/* Function start: 0x42C120 */
void SC_CombatBase::method11()
{
    if (DAT_0046ae50 != 0) {
        ((Sprite*)DAT_0046ae50)->RenderAt(
            ((Sprite*)DAT_0046ae50)->num_states,
            ((Sprite*)DAT_0046ae50)->field_0xb0,
            0, 0x3FF00000);
    }
}

/* Function start: 0x42C8A0 */
void SC_CombatBase::SetupViewport()
{
    DAT_0046ae50 = (void*)field_0x98;
    DAT_0046ae60 = (void*)field_0xA4;
    DAT_0046ae68 = (void*)field_0xAC;
    DAT_0046ae4c = (void*)field_0x94;
    DAT_0046ae70 = (void*)field_0xB4;
    DAT_0046ae64 = (void*)field_0xA8;
    DAT_0046ae5c = (void*)field_0xA0;
    DAT_0046ae58 = (void*)field_0x90;
    DAT_0046ae6c = (void*)field_0xB0;
    DAT_0046ae54 = (void*)field_0x9C;
    DAT_0046ae74 = (void*)field_0xB8;
}

extern void* __fastcall FUN_00425170(void*, int, int); // ScoreManager ctor
extern void* __fastcall FUN_00434660(void*, int);      // WeaponParser ctor
extern void* __fastcall FUN_0044b8d0(void*, int);      // mCNavigator ctor
extern void* __fastcall FUN_0041dbe0(void*, int);      // Palette ctor
extern void* __fastcall FUN_00408fb0(void*, int);      // CombatSprite ctor
extern void* __fastcall FUN_004432f0(void*, int);      // TargetList ctor
extern void* __fastcall FUN_0040c5e0(void*, int);      // ScoreDisplay ctor
extern void* __fastcall FUN_004454f0(void*, int);      // Viewport ctor
extern void __fastcall FUN_004127c0(void*, int);        // Parser ctor

/* Function start: 0x42C240 */
void SC_CombatBase::Initialize()
{
    void* mem;
    void* result;

    // Combat display (0xB0 bytes, Parser-derived inline ctor)
    mem = operator new(0xB0);
    result = 0;
    if (mem != 0) {
        FUN_004127c0(mem, 0);
        int* obj = (int*)mem;
        *(int*)((char*)mem + 0xA0) = 0;
        *(int*)((char*)mem + 0xA4) = 0;
        obj[0] = 0x4614b0;
        memset((char*)mem + 0x90, 0, 8 * 4);
        *(int*)((char*)mem + 0x90) = 100;
        *(int*)((char*)mem + 0x94) = 0xDC;
        *(int*)((char*)mem + 0x98) = 199;
        *(int*)((char*)mem + 0x9C) = 0;
        *(int*)((char*)mem + 0xAC) = 0;
        *(int*)((char*)mem + 0xA8) = 0;
        obj[0] = 0x4615a0;
        result = mem;
    }
    field_0xA4 = (int)result;

    // ScoreManager (0x10 bytes)
    mem = operator new(0x10);
    result = 0;
    if (mem != 0) {
        result = FUN_00425170(mem, 0, 0x32);
    }
    field_0xAC = (int)result;

    // WeaponParser (0xC0 bytes)
    mem = operator new(0xC0);
    result = 0;
    if (mem != 0) {
        result = FUN_00434660(mem, 0);
    }
    field_0x94 = (int)result;

    // mCNavigator (0xA8 bytes)
    mem = operator new(0xA8);
    result = 0;
    if (mem != 0) {
        result = FUN_0044b8d0(mem, 0);
    }
    field_0xB4 = (int)result;

    // Palette wrapper (0x08 bytes)
    mem = operator new(0x08);
    result = 0;
    if (mem != 0) {
        result = FUN_0041dbe0(mem, 0);
    }
    field_0xA8 = (int)result;

    // CombatSprite (0xA0 bytes)
    mem = operator new(0xA0);
    result = 0;
    if (mem != 0) {
        result = FUN_00408fb0(mem, 0);
    }
    field_0xA0 = (int)result;

    // TargetList (0x1D0 bytes)
    mem = operator new(0x1D0);
    result = 0;
    if (mem != 0) {
        result = FUN_004432f0(mem, 0);
    }
    field_0x90 = (int)result;

    // ScoreDisplay (0x24 bytes)
    mem = operator new(0x24);
    result = 0;
    if (mem != 0) {
        result = FUN_0040c5e0(mem, 0);
    }
    field_0xB0 = (int)result;

    // Viewport (0x2C bytes)
    mem = operator new(0x2C);
    result = 0;
    if (mem != 0) {
        result = FUN_004454f0(mem, 0);
    }
    field_0x9C = (int)result;

    // HotspotPool (0x18 bytes, inline init)
    int* pool = (int*)operator new(0x18);
    if (pool != 0) {
        pool[2] = 0;
        pool[3] = 0;
        pool[1] = 0;
        pool[0] = 0;
        pool[4] = 0;
        pool[5] = 10;
    }
    field_0xB8 = (int)pool;

    SetupViewport();
}

extern void __fastcall FUN_0044c740(void*);  // Sprite dtor
extern void __fastcall FUN_0044b950(void*, int);  // mCNavigator dtor
extern void __fastcall FUN_0041dc10(void*, int);  // Palette wrapper dtor
extern void __fastcall FUN_00443360(void*, int);  // TargetList dtor
extern void __fastcall FUN_00434740(void*, int);  // WeaponParser dtor
extern void __fastcall FUN_00409020(void*, int);  // CombatSprite dtor
extern void __fastcall FUN_00425200(void*, int);  // ScoreManager dtor

/* Function start: 0x42C630 */
void SC_CombatBase::CleanupAll()
{
    if (field_0x98 != 0) {
        FUN_0044c740((void*)field_0x98);
        FreeMemory((void*)field_0x98);
        field_0x98 = 0;
    }
    if (field_0xB4 != 0) {
        FUN_0044b950((void*)field_0xB4, 0);
        FreeMemory((void*)field_0xB4);
        field_0xB4 = 0;
    }
    if (field_0xA8 != 0) {
        FUN_0041dc10((void*)field_0xA8, 0);
        FreeMemory((void*)field_0xA8);
        field_0xA8 = 0;
    }
    if (field_0xB0 != 0) {
        FreeMemory((void*)field_0xB0);
        field_0xB0 = 0;
    }
    if (field_0x90 != 0) {
        FUN_00443360((void*)field_0x90, 0);
        FreeMemory((void*)field_0x90);
        field_0x90 = 0;
    }
    if (field_0x94 != 0) {
        FUN_00434740((void*)field_0x94, 0);
        FreeMemory((void*)field_0x94);
        field_0x94 = 0;
    }
    if (field_0xA0 != 0) {
        FUN_00409020((void*)field_0xA0, 0);
        FreeMemory((void*)field_0xA0);
        field_0xA0 = 0;
    }
    if (field_0x9C != 0) {
        Viewport* vp = (Viewport*)field_0x9C;
        vp->~Viewport();
        FreeMemory(vp);
        field_0x9C = 0;
    }
    if (field_0xAC != 0) {
        FUN_00425200((void*)field_0xAC, 0);
        FreeMemory((void*)field_0xAC);
        field_0xAC = 0;
    }
    if (field_0xA4 != 0) {
        int* obj = (int*)field_0xA4;
        int* vtbl = (int*)*obj;
        ((void (__cdecl *)(int))vtbl[3])(1);
        field_0xA4 = 0;
    }
    if (field_0xB8 != 0) {
        HotspotListData* pool = (HotspotListData*)field_0xB8;
        pool->~HotspotListData();
        FreeMemory(pool);
        field_0xB8 = 0;
    }

    RenderState();
}

int SC_CombatBase::LBLParse(char*) { return 0; }
void SC_CombatBase::ResetState() {}
void SC_CombatBase::ProcessInput() {}
void SC_CombatBase::method8() {}
int SC_CombatBase::method12() { return 0; }
int SC_CombatBase::HandleAction(int*) { return 0; }
void SC_CombatBase::RenderState() {}
int SC_CombatBase::UpdateAndCheck() { return 0; }
