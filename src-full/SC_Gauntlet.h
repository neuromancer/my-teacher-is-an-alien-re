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
    int field_120[7];              // 0x120-0x13B (zeroed, then 4 fields set)
    GauntletEntry entries[36];     // 0x13C-0x547 (36 * 0x1C = 0x3F0 + padding to 0x548)
    int field_548[6];              // 0x548-0x55F
};

#endif // SC_GAUNTLET_H
