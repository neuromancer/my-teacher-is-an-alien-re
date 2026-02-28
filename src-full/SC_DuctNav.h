#ifndef SC_DUCTNAV_H
#define SC_DUCTNAV_H

#include "Handler.h"

// SC_DuctNav - Duct navigation handler (case 46/0x2E)
// Constructor: 0x43AF10
// Size: 0x258
// Vtable: 0x461850
// References: "SC_DuctNav", "DUCT_SWITCH", "SaveGame__.sav"
class SC_DuctNav : public Handler {
public:
    SC_DuctNav();
    ~SC_DuctNav();

    int field_A8[108];  // 0xA8-0x257 (0x1B0 bytes)
};

#endif // SC_DUCTNAV_H
