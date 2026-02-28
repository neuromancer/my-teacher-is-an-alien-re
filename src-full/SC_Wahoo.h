#ifndef SC_WAHOO_H
#define SC_WAHOO_H

#include "Handler.h"

// SC_Wahoo - Wahoo mini-game handler (case 61/0x3D)
// Constructor: 0x437830
// Size: 0x168
// Vtable: 0x461768
// References: "SC_Wahoo"
class SC_Wahoo : public Handler {
public:
    SC_Wahoo();
    ~SC_Wahoo();

    int field_A8[48];   // 0xA8-0x167 (0xC0 bytes)
};

#endif // SC_WAHOO_H
