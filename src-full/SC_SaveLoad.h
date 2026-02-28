#ifndef SC_SAVELOAD_H
#define SC_SAVELOAD_H

#include "Handler.h"

// SC_SaveLoad - Save/load dialog handler (case 47/0x2F)
// Constructor: 0x421C40
// Size: 0xD8
// Vtable: 0x4613E8
// References: "save_yn.smk", yes/no/cancel buttons, "menu.col"
class SC_SaveLoad : public Handler {
public:
    SC_SaveLoad();
    ~SC_SaveLoad();

    int field_A8[12];   // 0xA8-0xD7 (0x30 bytes)
};

#endif // SC_SAVELOAD_H
