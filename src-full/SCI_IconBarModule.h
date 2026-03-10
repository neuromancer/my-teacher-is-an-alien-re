#ifndef SCI_ICONBARMODULE_H
#define SCI_ICONBARMODULE_H

#include "IconBar.h"
#include "Sprite.h"

class SC_Message;

// SCI_IconBarModule - Icon bar module handler (case 32/0x20)
// Constructor: 0x401000
// Size: 0x130
// Vtable: 0x461000
// Extends IconBar
// References: "NUM_ACTIONS"
class SCI_IconBarModule : public IconBar {
public:
    SCI_IconBarModule();
    ~SCI_IconBarModule();

    int ShutDown(SC_Message* msg);         // 0x401CF0
    void Update(int param1, int param2);   // 0x401D80
    int AddMessage(SC_Message* msg);       // 0x401E40
    int FindClickedIcon(int x, int y);     // 0x4020A0
    void UpdateCursor();                   // 0x402100
    void Serialize(void* param);           // 0x4021C0

    int field_A8;       // 0xA8
    int field_AC;       // 0xAC - mode flag (0=normal, nonzero=alternate)
    int field_B0;       // 0xB0
    int field_B4;       // 0xB4
    int field_B8;       // 0xB8 - boundary check flag
    int field_BC;       // 0xBC
    int field_C0;       // 0xC0
    int field_C4;       // 0xC4 - skip NUM_ACTIONS flag
    int field_C8;       // 0xC8
    int field_CC;       // 0xCC
    int field_D0;       // 0xD0 - bounding rect left
    int field_D4;       // 0xD4 - bounding rect top
    int field_D8;       // 0xD8 - bounding rect right
    int field_DC;       // 0xDC - bounding rect bottom
    void* field_E0;     // 0xE0 - timeout object pointer
    int field_E4;       // 0xE4
    void* field_E8;     // 0xE8 - cursor/resource pointer
    Sprite* icons[15];  // 0xEC-0x127 - array of 15 icon sprite pointers
    int field_128;      // 0x128
    int field_12C;      // 0x12C
};

#endif // SCI_ICONBARMODULE_H
