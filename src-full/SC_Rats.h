#ifndef SC_RATS_H
#define SC_RATS_H

#include "Handler.h"

// SC_Rats - Rats scene handler (case 66/0x42)
// Constructor: 0x451700
// Size: 0xB0
// Vtable: 0x461CE0
// References: "cb_rats.mis"
class SC_Rats : public Handler {
public:
    SC_Rats();
    ~SC_Rats();

    int field_A8[2];    // 0xA8-0xAF (0x08 bytes)
};

#endif // SC_RATS_H
