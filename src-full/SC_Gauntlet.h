#ifndef SC_GAUNTLET_H
#define SC_GAUNTLET_H

#include "SC_Combat.h"

// GauntletEntry — 0x1C byte element with ctor/dtor (ctor 0x42E660, dtor 0x42E6D0)
struct GauntletEntry {
    int fields[7]; // 0x00-0x1B
    GauntletEntry();
    ~GauntletEntry();
};

// SC_Gauntlet - Android gauntlet mini-game (case 80/0x50)
// Constructor: 0x42E540
// Size: 0x560
// Vtable: 0x461610
// Extends SC_Combat (0x118)
class SC_Gauntlet : public SC_Combat {
public:
    SC_Gauntlet();
    ~SC_Gauntlet();

    int field_118[2];              // 0x118-0x11F
    int field_120[2];              // 0x120-0x127
    int field_128;                 // 0x128
    int field_12C;                 // 0x12C
    int field_130;                 // 0x130
    int field_134;                 // 0x134
    int field_138;                 // 0x138
    GauntletEntry entries[36];     // 0x13C-0x52B (36 * 0x1C = 0x3F0)
    int field_52C[7];              // 0x52C-0x547
    int field_548;                 // 0x548
    int field_54C;                 // 0x54C
    int field_550;                 // 0x550
    int field_554;                 // 0x554
    int field_558;                 // 0x558
    int field_55C;                 // 0x55C
};

#endif // SC_GAUNTLET_H
