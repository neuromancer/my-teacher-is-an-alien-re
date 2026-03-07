#ifndef SC_DODGEORVILLE_H
#define SC_DODGEORVILLE_H

#include "SC_Combat.h"
#include "SpriteAction.h"

// 8-byte embedded object with inline ctor, trivial dtor (no SEH cleanup needed)
struct DodgeDim {
    int field_0;
    int field_4;
    DodgeDim() { field_0 = 0; field_4 = 0; }
};

// SC_DodgeOrville - Dodge Orville stink bomb mini-game (case 67/0x43)
// Constructor: 0x428840
// Size: 0x168
// Vtable: 0x4614C8
// Extends SC_Combat (0x118)
// Engine vtable mapping: [5]=AddMessage, [6]=Cleanup, [11]=ProcessTargets
class SC_DodgeOrville : public SC_Combat {
public:
    SC_DodgeOrville();
    ~SC_DodgeOrville();

    void Cleanup(int flag);            // 0x428A40 - overrides Engine::VirtCleanup [6]
    int AddMessage(SC_Message* msg);   // 0x428AF0 - overrides Engine::CleanupSubsystems [5]
    void ProcessTargets();             // 0x428B60 - overrides Engine::ProcessTargets [11]

    int field_118[3];     // 0x118-0x123 (12 bytes)
    SlimeDim dim_124;     // 0x124-0x12B (8 bytes, non-trivial dtor)
    int field_12C;        // 0x12C - Sprite/SpriteAction pointer
    int field_130;        // 0x130 - Sprite/SpriteAction pointer
    SlimeDim dim_134;     // 0x134-0x13B (8 bytes, non-trivial dtor)
    DodgeDim dim_13C;     // 0x13C-0x143 (8 bytes, trivial dtor)
    DodgeDim dim_144;     // 0x144-0x14B (8 bytes, trivial dtor)
    DodgeDim dim_14C;     // 0x14C-0x153 (8 bytes, trivial dtor)
    int field_154[5];     // 0x154-0x167 (20 bytes)
};

#endif // SC_DODGEORVILLE_H
