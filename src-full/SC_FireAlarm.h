#ifndef SC_FIREALARM_H
#define SC_FIREALARM_H

#include "Handler.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

class Sprite;
class Palette;
class SlimeTable;

// SC_FireAlarm - Fire alarm puzzle handler (case 64/0x40)
// Constructor: 0x407290
// Size: 0x150
// Vtable: 0x4610C8
// References: "CB_FireAlarm.mis", "SC_FireAlarm"
class SC_FireAlarm : public Handler {
public:
    SC_FireAlarm();
    ~SC_FireAlarm();

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    int Exit(SC_Message* msg);
    void SendResultMessage();       // 0x4079E0
    void ResetState();              // 0x407BB0
    int HandleClick(int* param);    // 0x407C20
    void Render();                  // 0x407E50
    void ProcessFrame();            // 0x407EE0

    SpriteAction* field_A8;           // 0xA8 - SpriteAction*
    int field_AC;           // 0xAC
    int field_B0;           // 0xB0
    SlimeDim dim_B4;        // 0xB4-0xBB
    Sprite* field_BC;           // 0xBC
    int field_C0;           // 0xC0
    Sprite* field_C4;           // 0xC4
    Sprite* field_C8;           // 0xC8
    Rect slot_CC;    // 0xCC-0xDB
    Sprite* field_DC;           // 0xDC
    SlimeDim dim_E0;        // 0xE0-0xE7
    SlimeDim dim_E8;        // 0xE8-0xEF
    SlimeDim dim_F0;        // 0xF0-0xF7
    Sprite* field_F8;           // 0xF8
    SlimeDim dim_FC;        // 0xFC-0x103
    SlimeDim dim_104;       // 0x104-0x10B
    Sprite* field_10C;          // 0x10C
    Sprite* field_110;          // 0x110
    SlimeDim dim_114;       // 0x114-0x11B
    Rect slot_11C;   // 0x11C-0x12B
    SlimeDim dim_12C;       // 0x12C-0x133
    int field_134;          // 0x134
    SlimeDim dim_138;       // 0x138-0x13F
    int field_140;          // 0x140
    int field_144;          // 0x144
    int field_148;               // 0x148 - SlimeTable*/Palette* (shared sound object)
    int field_14C;          // 0x14C
};

#endif // SC_FIREALARM_H
