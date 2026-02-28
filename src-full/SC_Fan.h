#ifndef SC_FAN_H
#define SC_FAN_H

#include "Handler.h"

// SC_Fan - Fan puzzle handler (case 62/0x3E)
// Constructor: 0x40EFF0
// Size: 0x1C8
// Vtable: 0x461258
// References: "SC_Fan", TypedArray for animations
class SC_Fan : public Handler {
public:
    SC_Fan();
    ~SC_Fan();

    int field_A8[72];   // 0xA8-0x1C7 (0x120 bytes)
};

#endif // SC_FAN_H
