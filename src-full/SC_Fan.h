#ifndef SC_FAN_H
#define SC_FAN_H

#include "Handler.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

// 8-byte embedded object with inline ctor, trivial dtor (no SEH cleanup needed)
struct FanDim {
    int field_0;
    int field_4;
    FanDim() { field_0 = 0; field_4 = 0; }
};

// SC_Fan - Fan puzzle handler (case 62/0x3E)
// Constructor: 0x40EFF0
// Destructor: 0x40F190
// Cleanup: 0x40F420
// Size: 0x1C8
// Vtable: 0x461258
class SC_Fan : public Handler {
public:
    SC_Fan();
    ~SC_Fan();

    virtual int LBLParse(char* line);          // [0] 0x4104B0
    virtual void OnProcessEnd();               // [2] 0x410390
    virtual void Init(SC_Message* msg);        // [4] 0x40F290
    virtual int AddMessage(SC_Message* msg);   // [5] 0x40F6C0
    virtual void Update(int param1, int param2); // [7] 0x40F660
    virtual int Exit(SC_Message* msg);         // [8] 0x40F6F0

    void Cleanup(int flag);            // [6] 0x40F420 (vtable ShutDown slot)

    void ProcessRound();               // 0x40F760
    void DisplaySprites(int frame);    // 0x40FBD0
    void State0Handler();              // 0x40FEA0
    void State2Handler();              // 0x40FF40
    void State4Handler();              // 0x40FFC0
    void RenderFan();                  // 0x410060

    void* field_A8;       // 0xA8 - SpriteAction*
    void* field_AC;       // 0xAC - SpriteAction*
    int field_B0;         // 0xB0
    SlimeDim dim_B4;      // 0xB4 - SlimeDim (8 bytes)
    void* field_BC;       // 0xBC - TimeOut*
    void* field_C0;       // 0xC0 - Sprite*
    void* field_C4;       // 0xC4 - Palette*
    void* field_C8;       // 0xC8 - Sprite*
    void* field_CC;       // 0xCC - Sprite*
    InvSlotItem invSlot_D0; // 0xD0 - InvSlotItem (16 bytes)
    void* field_E0;       // 0xE0 - Sprite*
    void* field_E4;       // 0xE4 - Sprite*
    void* field_E8;       // 0xE8 - Sprite*
    void* field_EC;       // 0xEC - Sprite*
    int field_F0;         // 0xF0
    int field_F4;         // 0xF4
    int field_F8;         // 0xF8
    int field_FC;         // 0xFC
    InvSlotItem invSlots_100[4]; // 0x100 - TypedArray (4 x 16 bytes)
    int field_140;        // 0x140
    void* field_144;      // 0x144
    InvSlotItem invSlot_148; // 0x148 - InvSlotItem (16 bytes)
    InvSlotItem invSlot_158; // 0x158 - InvSlotItem (16 bytes)
    FanDim dim_168;       // 0x168 - FanDim (8 bytes, trivial dtor)
    SlimeDim dim_170;     // 0x170 - SlimeDim (8 bytes)
    int field_178;        // 0x178
    int field_17C;        // 0x17C
    int field_180;        // 0x180
    int field_184;        // 0x184
    int field_188;        // 0x188
    int field_18C;        // 0x18C
    int field_190;        // 0x190
    int field_194;        // 0x194
    void* field_198[11];  // 0x198 - Sample* array (11 elements)
    int field_1C4;        // 0x1C4
};

#endif // SC_FAN_H
