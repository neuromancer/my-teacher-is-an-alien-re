#ifndef SC_DEMOSCREEN_H
#define SC_DEMOSCREEN_H

#include "Handler.h"

// SC_DemoScreen - Demo screen handler (case 41/0x29)
// Constructor: 0x44E5C0
// Size: 0xB0
// Vtable: 0x461C20
// References: "Demoscrn.smk", "Demoscrn.col"
class SC_DemoScreen : public Handler {
public:
    SC_DemoScreen();
    ~SC_DemoScreen();

    int field_A8[2];    // 0xA8-0xAF (0x08 bytes)
};

#endif // SC_DEMOSCREEN_H
