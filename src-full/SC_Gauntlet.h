#ifndef SC_GAUNTLET_H
#define SC_GAUNTLET_H

#include "SC_Combat.h"

// SC_Gauntlet - Android gauntlet mini-game (case 80/0x50)
// Constructor: 0x42E540
// Size: 0x560
// Vtable: 0x461610
// Extends SC_Combat (0x118)
// TypedArray at +0x13C (size=0x1C, count=0x24)
class SC_Gauntlet : public SC_Combat {
public:
    SC_Gauntlet();
    ~SC_Gauntlet();

    int field_118[274]; // 0x118-0x55F (0x448 bytes)
};

#endif // SC_GAUNTLET_H
