#ifndef SC_CRYSTALPUZZLE_H
#define SC_CRYSTALPUZZLE_H

#include "Handler.h"

// SC_CrystalPuzzle - Force field crystal puzzle (case 38/0x26)
// Constructor: 0x44EBA0
// Size: 0x1C0
// Vtable: 0x461C58
// References: "puz\ForceField\Puzztest.col", "SC_CrystalPuzzle::Process_Action"
class SC_CrystalPuzzle : public Handler {
public:
    SC_CrystalPuzzle();
    ~SC_CrystalPuzzle();

    int field_A8[70];   // 0xA8-0x1BF (0x118 bytes)
};

#endif // SC_CRYSTALPUZZLE_H
