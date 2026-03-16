#ifndef SCI_ICONBARMODULE_H
#define SCI_ICONBARMODULE_H

#include "IconBar.h"
#include "Sprite.h"

class SC_Message;
class TimeOut;
class MMPlayer;

// SCI_IconBarModule - Icon bar module handler (case 32/0x20)
// Constructor: 0x401000
// Size: 0x130, Vtable: 0x461000
// Extends IconBar
class SCI_IconBarModule : public IconBar {
public:
    SCI_IconBarModule();
    ~SCI_IconBarModule();

    void Init(SC_Message* msg);            // 0x4013E0
    int ShutDown(SC_Message* msg);         // 0x401CF0
    void Update(int param1, int param2);   // 0x401D80
    int AddMessage(SC_Message* msg);       // 0x401E40
    int FindClickedIcon(int x, int y);     // 0x4020A0
    void UpdateCursor();                   // 0x402100
    void Serialize(void* param);           // 0x4021C0

    int field_A8;               // 0xA8
    int mode;                   // 0xAC — 0=normal icon bar, nonzero=alternate mode
    int field_B0;               // 0xB0
    int field_B4;               // 0xB4
    int hasBoundaryRect;        // 0xB8 — enables boundary rect hit test
    int field_BC;               // 0xBC
    int field_C0;               // 0xC0
    int skipActionsCount;       // 0xC4 — if nonzero, skip NUM_ACTIONS increment
    int field_C8;               // 0xC8
    int field_CC;               // 0xCC
    int boundLeft;              // 0xD0 — boundary rect left
    int boundTop;               // 0xD4 — boundary rect top
    int boundRight;             // 0xD8 — boundary rect right
    int boundBottom;            // 0xDC — boundary rect bottom
    TimeOut* timeout;           // 0xE0 — inactivity timeout
    int field_E4;               // 0xE4
    MMPlayer* mmPlayer;         // 0xE8 — background animation/cursor
    Sprite* icons[15];          // 0xEC-0x127 — icon sprite pointers
    int field_128;              // 0x128
    int field_12C;              // 0x12C
};

#endif // SCI_ICONBARMODULE_H
