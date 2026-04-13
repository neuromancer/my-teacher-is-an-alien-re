#ifndef SC_FAN_H
#define SC_FAN_H

#include "Handler.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

class Sprite;
class Palette;
class TimeOut;
class VBuffer;
class Sample;

// 8-byte embedded object with inline ctor, trivial dtor (no SEH cleanup needed)
struct FanDim {
    int x;
    int y;
    FanDim() { x = 0; y = 0; }
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
    virtual void Init(SC_MessageParser* msg);        // [4] 0x40F290
    virtual int AddMessage(SC_MessageParser* msg);   // [5] 0x40F6C0
    virtual void Update(int param1, int param2); // [7] 0x40F660
    virtual int Exit(SC_MessageParser* msg);         // [8] 0x40F6F0
    virtual void HandleInput();                // [11] 0x40FC50
    virtual void ProcessHit();                 // [12] 0x40FC30

    void Cleanup(int flag);            // [6] 0x40F420 (vtable ShutDown slot)

    void ProcessRound();               // 0x40F760
    void DisplaySprites(int frame);    // 0x40FBD0
    void State0Handler();              // 0x40FEA0
    void State2Handler();              // 0x40FF40
    void State4Handler();              // 0x40FFC0
    void RenderFan();                  // 0x410060

    SpriteAction* mainAction;     // 0xA8 - main sprite action
    SpriteAction* loseAction;     // 0xAC - lose message action
    int field_B0;         // 0xB0
    SlimeDim dim_B4;      // 0xB4 - SlimeDim (8 bytes)
    TimeOut* timer;       // 0xBC - TimeOut*
    Sprite* consoleSprite;  // 0xC0 - CONSOLE_SPRITE
    Palette* palette;       // 0xC4 - PALETTE
    Sprite* bgSprite;       // 0xC8 - BACKGROUND_SPRITE
    Sprite* stopSwitchSprite; // 0xCC - STOP_SWITCH_SPRITE
    Rect invSlot_D0; // 0xD0 - Rect (16 bytes)
    Sprite* tlSprite;       // 0xE0 - TL_SPRITE
    Sprite* blSprite;       // 0xE4 - BL_SPRITE
    Sprite* trSprite;       // 0xE8 - TR_SPRITE
    Sprite* brSprite;       // 0xEC - BR_SPRITE
    int field_F0;         // 0xF0
    int field_F4;         // 0xF4
    int field_F8;         // 0xF8
    int field_FC;         // 0xFC
    Rect invSlots_100[4]; // 0x100 - TypedArray (4 x 16 bytes)
    int field_140;        // 0x140
    VBuffer* noiseMeterBuffer; // 0x144 - NOISE_METER_VBUFFER
    Rect invSlot_148; // 0x148 - Rect (16 bytes)
    Rect invSlot_158; // 0x158 - Rect (16 bytes)
    FanDim dim_168;       // 0x168 - FanDim (8 bytes, trivial dtor)
    SlimeDim dim_170;     // 0x170 - SlimeDim (8 bytes)
    int inputActive;      // 0x178
    int state;            // 0x17C
    int introPhase;       // 0x180
    int field_184;        // 0x184
    int field_188;        // 0x188
    int field_18C;        // 0x18C
    int exitPhase;        // 0x190
    int field_194;        // 0x194
    Sample* samples[11];  // 0x198 - Sample* array (11 elements)
    int bgSoundId;        // 0x1C4
};

#endif // SC_FAN_H
