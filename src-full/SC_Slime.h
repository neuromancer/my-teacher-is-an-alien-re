#ifndef SC_SLIME_H
#define SC_SLIME_H

#include "Handler.h"
#include "InvSlotItem.h"
#include "SpriteAction.h"

class Sprite;
class Palette;
class SC_Message;

// 8-byte embedded object, ctor 0x40D0D0 (zeros 2 fields), no dtor
struct SlotPair {
    int field_0;
    int field_4;
    SlotPair();
};


// SC_Slime - Slime puzzle handler (case 63/0x3F)
// Constructor: 0x40CF60
// Size: 0x180
// Vtable: 0x461218
// References: "cb_slime.mis", "CB_Slime", "SC_Slime"
class SC_Slime : public Handler {
public:
    SC_Slime();
    ~SC_Slime();

    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    void ResetSprites();         // 0x40E0D0
    void UpdateArmSprites();     // 0x40E070

    int field_A8;       // 0xA8 - action/message object pointer
    int field_AC;       // 0xAC
    SlimeDim dimB0;     // 0xB0-0xB7 (width/height)
    int field_B8;       // 0xB8 - TimeOut*
    Sprite* spriteBC;   // 0xBC - slime meter sprite
    Palette* palette;   // 0xC0 - palette
    Sprite* spriteC4;   // 0xC4 - sprite
    Sprite* spriteC8;   // 0xC8 - sprite
    Sprite* spriteCC;   // 0xCC - sprite
    int field_D0;       // 0xD0
    int field_D4;       // 0xD4
    InvSlotItem invSlots[2];  // 0xD8-0xF7 (2 x 16 bytes)
    SlotPair pairs1[2];       // 0xF8-0x107 (2 x 8 bytes)
    SlotPair pairs2[2];       // 0x108-0x117 (2 x 8 bytes)
    Sprite* sprite118;  // 0x118
    Sprite* sprite11C;  // 0x11C
    Sprite* sprite120;  // 0x120
    Sprite* sprite124;  // 0x124
    Sprite* sprite128;  // 0x128
    Sprite* sprite12C;  // 0x12C
    int field_130;      // 0x130
    int field_134;      // 0x134
    int field_138;      // 0x138
    int field_13C;      // 0x13C
    InvSlotItem invSlot3;     // 0x140-0x14F (16 bytes)
    int field_150;      // 0x150 - object pointer
    int field_154;      // 0x154
    int field_158;      // 0x158
    int field_15C;      // 0x15C
    int field_160;      // 0x160
    int field_164;      // 0x164
    int field_168;      // 0x168
    int field_16C;      // 0x16C - object pointer
    int* field_170;     // 0x170 - dynamic array (7 elements)
    int field_174;      // 0x174
    int field_178;      // 0x178
    int field_17C;      // 0x17C - array size (7)
};

#endif // SC_SLIME_H
