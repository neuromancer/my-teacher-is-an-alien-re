#ifndef SC_COMBAT_H
#define SC_COMBAT_H

#include "Handler.h"

// SC_Combat - Base class for combat/mini-game handlers
// Constructor: 0x449100
// Size: 0x118
// Vtable: 0x461A90
// Extends Handler (0xA8)
// Layout:
//   0x00-0xA7: Handler base
//   0xA8-0x117: SC_Combat fields (0x1C DWORDs, zeroed)
class SC_Combat : public Handler {
public:
    SC_Combat();
    ~SC_Combat();

    int field_A8[28];   // 0xA8-0x117 (0x1C DWORDs = 0x70 bytes)
};

#endif // SC_COMBAT_H
