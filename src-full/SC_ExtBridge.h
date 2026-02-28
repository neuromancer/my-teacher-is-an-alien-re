#ifndef SC_EXTBRIDGE_H
#define SC_EXTBRIDGE_H

#include "Handler.h"

// SC_ExtBridge - Exterior bridge scene handler (case 65/0x41)
// Constructor: 0x4399E0
// Size: 0xC0
// Vtable: 0x4617D8
// Simple handler with minimal fields
class SC_ExtBridge : public Handler {
public:
    SC_ExtBridge();
    ~SC_ExtBridge();

    int field_A8[6];    // 0xA8-0xBF (0x18 bytes)
};

#endif // SC_EXTBRIDGE_H
