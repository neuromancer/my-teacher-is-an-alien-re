#ifndef SC_BGSND_H
#define SC_BGSND_H

#include "Handler.h"

// SC_BgSnd - Background sound handler (case 5)
// Constructor: 0x4392E0
// Size: 0xD0
// Vtable: 0x4617A8
// References: "GS_MUSICOFF", "SC_BgSnd::AddMessage"
class SC_BgSnd : public Handler {
public:
    SC_BgSnd();
    ~SC_BgSnd();

    int field_A8[10];   // 0xA8-0xCF (0x28 bytes)
};

#endif // SC_BGSND_H
