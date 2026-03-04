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

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    void Update(int param1, int param2);
    int Exit(SC_Message* msg);

    int field_A8;    // 0xA8 - Palette*
    int field_AC;    // 0xAC - Sprite*
};

#endif // SC_DEMOSCREEN_H
