#ifndef SC_EXTBRIDGE_H
#define SC_EXTBRIDGE_H

#include "Handler.h"
#include "SpriteAction.h"

// SC_ExtBridge - Exterior bridge scene handler (case 65/0x41)
// Constructor: 0x4399E0
// Size: 0xC0
// Vtable: 0x4617D8
class SC_ExtBridge : public Handler {
public:
    int field_A8;       // 0xA8 - SpriteAction*
    int field_AC;       // 0xAC - Parser-derived* (virtual delete)
    int field_B0;       // 0xB0 - dtor FUN_0041dc10
    SlimeDim dim;       // 0xB4 (8 bytes)
    int field_BC;       // 0xBC

    SC_ExtBridge();
    ~SC_ExtBridge();

    virtual void Init(SC_Message* msg);  // 0x439BB0
};

#endif // SC_EXTBRIDGE_H
