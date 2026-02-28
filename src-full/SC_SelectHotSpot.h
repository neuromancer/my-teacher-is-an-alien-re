#ifndef SC_SELECTHOTSPOT_H
#define SC_SELECTHOTSPOT_H

#include "Handler.h"

// SC_SelectHotSpot - Hotspot selection handler (case 45/0x2D)
// Constructor: 0x405C30
// Size: 0xC0
// Vtable: 0x461068
// References: "SC_SelectHotSpot::Update()"
class SC_SelectHotSpot : public Handler {
public:
    SC_SelectHotSpot();
    ~SC_SelectHotSpot();

    int field_A8[6];    // 0xA8-0xBF (0x18 bytes)
};

#endif // SC_SELECTHOTSPOT_H
