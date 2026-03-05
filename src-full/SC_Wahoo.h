#ifndef SC_WAHOO_H
#define SC_WAHOO_H

#include "Handler.h"

// SC_Wahoo - Wahoo mini-game handler (case 61/0x3D)
// Constructor: 0x437830
// Size: 0x168
// Vtable: 0x461768
// References: "SC_Wahoo"
class SC_Wahoo : public Handler {
public:
    SC_Wahoo();
    ~SC_Wahoo();

    // Virtual overrides
    virtual void Init(SC_Message* msg);       // 0x437A40 (+0x10)
    virtual int AddMessage(SC_Message* msg);  // 0x4381E0 (+0x14)
    virtual int ShutDown(SC_Message* msg);    // 0x437C60 (+0x18)
    virtual void Update(int param1, int param2); // 0x437EA0 (+0x1C)
    virtual int Exit(SC_Message* msg);        // 0x438280 (+0x20)

    virtual int LBLParse(char* param_1);      // 0x438630 (+0x00)

    int FUN_438F10(int param_1);              // 0x438F10 (+0x2C)

    void ProcessState();                      // 0x4382F0
    void OnProcessEnd();                      // 0x438470

    void* field_A8;      // 0xA8 - SpriteAction*
    void* field_AC;      // 0xAC - Palette*
    void* field_B0;      // 0xB0 - Sprite*
    void* field_B4;      // 0xB4 - Sprite*
    void* field_B8;      // 0xB8 - Sprite*
    void* field_BC;      // 0xBC - Sprite*
    void* field_C0;      // 0xC0 - Sprite*
    void* field_C4;      // 0xC4 - unknown object (dtor 0x410fd0)
    void* field_C8;      // 0xC8 - TimeOut*
    int field_CC;        // 0xCC
    int field_D0;        // 0xD0
    int field_D4;        // 0xD4
    int field_D8;        // 0xD8
    void* field_DC[14];  // 0xDC-0x110 - Sample* array
    void* field_114;     // 0x114 - SoundList*
    int field_118;       // 0x118
    int field_11C;       // 0x11C
    void* field_120;     // 0x120
    void* field_124;     // 0x124
    void* field_128;     // 0x128
    int field_12C;       // 0x12C
    int field_130;       // 0x130
    int field_134;       // 0x134 - width (0x140)
    int field_138;       // 0x138 - height (0xF0)
    void* field_13C;     // 0x13C - Sprite*
    int field_140;       // 0x140 - gamestate value
    int field_144;       // 0x144
    int field_148;       // 0x148
    int field_14C;       // 0x14C
    int field_150;       // 0x150
    int field_154;       // 0x154
    int field_158;       // 0x158
    int field_15C;       // 0x15C
    int field_160;       // 0x160
    int field_164;       // 0x164 - flags
};

#endif // SC_WAHOO_H
