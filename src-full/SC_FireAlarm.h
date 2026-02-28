#ifndef SC_FIREALARM_H
#define SC_FIREALARM_H

#include "Handler.h"

// SC_FireAlarm - Fire alarm puzzle handler (case 64/0x40)
// Constructor: 0x407290
// Size: 0x150
// Vtable: 0x4610C8
// References: "CB_FireAlarm.mis", "SC_FireAlarm"
class SC_FireAlarm : public Handler {
public:
    SC_FireAlarm();
    ~SC_FireAlarm();

    int field_A8[42];   // 0xA8-0x14F (0xA8 bytes)
};

#endif // SC_FIREALARM_H
