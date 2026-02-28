#ifndef SC_SLIME_H
#define SC_SLIME_H

#include "Handler.h"

// SC_Slime - Slime puzzle handler (case 63/0x3F)
// Constructor: 0x40CF60
// Size: 0x180
// Vtable: 0x461218
// References: "cb_slime.mis", TypedArray
class SC_Slime : public Handler {
public:
    SC_Slime();
    ~SC_Slime();

    int field_A8[54];   // 0xA8-0x17F (0xD8 bytes)
};

#endif // SC_SLIME_H
