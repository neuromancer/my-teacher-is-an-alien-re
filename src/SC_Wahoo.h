#ifndef SC_WAHOO_H
#define SC_WAHOO_H

#include "Handler.h"
#include "InvSlotItem.h"
#include "SpriteAction.h"

class Palette;
class Sprite;
class VBuffer;
class TimeOut;
class SoundList;
class Sample;

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
    virtual void Init(SC_MessageParser* msg);       // 0x437A40 (+0x10)
    virtual int AddMessage(SC_MessageParser* msg);  // 0x4381E0 (+0x14)
    virtual void ShutDown(SC_MessageParser* msg);    // 0x437C60 (+0x18)
    virtual void Update(int param1, int param2); // 0x437EA0 (+0x1C)
    virtual int Exit(SC_MessageParser* msg);        // 0x438280 (+0x20)

    virtual int LBLParse(char* param_1);      // 0x438630 (+0x00)

    int ProcessClick(int param_1);              // 0x438F10 (+0x2C)
    virtual void OnCombatResult();              // 0x438EF0 (+0x30)

    void ProcessState();                      // 0x4382F0
    void OnProcessEnd();                      // 0x438470

    SpriteAction* spriteAction;   // 0xA8 - SpriteAction*
    Palette* palette;             // 0xAC - PALETTE
    Sprite* bgSprite;             // 0xB0 - BACKGROUND_SPRITE
    Sprite* resetSwitchSprite;    // 0xB4 - RESET_SWITCH_SPRITE
    Sprite* innerSprite;          // 0xB8 - INNER_SPRITE
    Sprite* middleSprite;         // 0xBC - MIDDLE_SPRITE
    Sprite* outerSprite;          // 0xC0 - OUTER_SPRITE
    VBuffer* detectionMask;       // 0xC4 - DETECTION_MASK
    TimeOut* timer;               // 0xC8 - TimeOut*
    Rect cursorHitbox;            // 0xCC-0xDB (Rect: left, top, right, bottom)
    void* sampleSlots[14];        // 0xDC-0x110 - Sample* array (slots 7-13)
    SoundList* soundList;         // 0x114 - SoundList*
    int field_118;                // 0x118
    int sound1;                   // 0x11C - SOUND id=1 sample
    Sample* missSound;            // 0x120 - SOUND id=2 (miss/incorrect)
    Sample* resetSound;           // 0x124 - SOUND id=3 (reset click)
    Sample* startSound;           // 0x128 - SOUND id=4 (start/intro)
    int matchSound;               // 0x12C - SOUND id=5 (match)
    int winSound;                 // 0x130 - SOUND id=6 (win)
    SlimeDim screenSize;          // 0x134-0x13B (width/height)
    Sprite* consoleSprite;        // 0x13C - CONSOLE_SPRITE
    int playRightBridge;          // 0x140 - PLAY_RIGHT_BRIDGE gamestate value
    Rect switchHitbox;            // 0x144-0x153 (switch area Rect)
    Rect playAreaHitbox;          // 0x154-0x163 (play area Rect)
    int gameFlags;                // 0x164 - flags (bit 0=won, bit 1=clicked)
};

#endif // SC_WAHOO_H
