#ifndef ENGINEC_H
#define ENGINEC_H

#include "SC_CombatBase.h"
#include <string.h>

class Sprite;

// EC_Dim - ctor-only 8-byte counter pair (current/limit), no destructor.
// Same pattern as FA_Dim in SC_FireAlarm.h: the EngineC constructor at 0x40BBF0
// zero-constructs three of these via lea+paired stores BEFORE the vtable store,
// proving they are members with inline default constructors (MSVC 4.20 emits the
// vfptr store after the member initializers).
struct EC_Dim { int x; int y; EC_Dim() { memset(this, 0, 8); } };

// EngineC - Third combat engine type (Detention mini-game engine)
// Constructor: 0x40BBF0, Destructor: 0x40BCC0
// Vtable: 0x4611B0 (18 entries, inherits from SC_CombatBase)
// Size: 0x120
// Created by SC_Detention's LBLParse (0x40BAD0)
class EngineC : public SC_CombatBase {
public:
    EngineC();
    ~EngineC();

    virtual int LBLParse(char* line);   // [0] 0x40C270
    virtual void OnProcessEnd();        // [2] 0x40C200
    virtual void RenderBackground();    // [11] 0x40BDC0
    virtual int HandleAction(int* param); // [13] 0x40C1B0
    virtual void ProcessFrame();         // [10] 0x40C0D0

    // Fields from 0xF0 to 0x11F (12 ints)
    int mouseReleased;    // 0xF0 — mouse button release tracker
    EC_Dim missCounter;   // 0xF4/0xF8 — x = last seen miss count
    EC_Dim hitCounter;    // 0xFC/0x100 — x = hit count, y = MAX_NUM_HITS
    Sprite* sprite104;    // 0x104 — Sprite* (deleted in dtor)
    EC_Dim shotCounter;   // 0x108/0x10C — x = engine shots, y = NUM_ENGINE_SHOTS
    Sprite* sprite110;    // 0x110 — Sprite* (deleted in dtor)
    Sprite* sprite114;    // 0x114 — Sprite* (deleted in dtor)
    Sprite* sprite118;    // 0x118 — Sprite* (deleted in dtor)
    int field_0x11C;      // 0x11C
};

#endif // ENGINEC_H
