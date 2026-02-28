#ifndef SC_ROACH_H
#define SC_ROACH_H

#include "SC_Combat.h"

// SC_Roach - Roach room handler (case 71/0x47)
// Constructor: 0x418C20
// Size: 0x730
// Vtable: 0x461330
// Extends SC_Combat (0x118)
// References: "SC_Roach::Process_Action", "ThotsLvl.cpp"
// Two TypedArrays: at +0x118 (size=0x2C, count=8) and +0x270 (size=0x20, count=0x24)
class SC_Roach : public SC_Combat {
public:
    SC_Roach();
    ~SC_Roach();

    int field_118[390]; // 0x118-0x72F (0x618 bytes)
};

#endif // SC_ROACH_H
