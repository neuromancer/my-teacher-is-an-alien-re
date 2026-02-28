#ifndef SC_DODGEORVILLE_H
#define SC_DODGEORVILLE_H

#include "SC_Combat.h"

// SC_DodgeOrville - Dodge Orville stink bomb mini-game (case 67/0x43)
// Constructor: 0x428840
// Size: 0x168
// Vtable: 0x4614C8
// Extends SC_Combat (0x118)
class SC_DodgeOrville : public SC_Combat {
public:
    SC_DodgeOrville();
    ~SC_DodgeOrville();

    int field_118[20];  // 0x118-0x167 (0x50 bytes)
};

#endif // SC_DODGEORVILLE_H
