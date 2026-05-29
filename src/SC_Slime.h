#ifndef SC_SLIME_H
#define SC_SLIME_H

#include "Handler.h"
#include "InvSlotItem.h"
#include "SpriteAction.h"

class Sprite;
class TimeOut;
class SoundList;
class SlimeTable;
class Sample;
class Palette;
class SC_MessageParser;

// 8-byte embedded object, ctor 0x40D0D0 (zeros 2 fields), no dtor
struct SlotPair {
    int count;
    int max;
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

    virtual int LBLParse(char* line);              // [0]  0x40E1E0
    virtual void OnProcessEnd();                   // [2]  0x40E0D0
    virtual void Init(SC_MessageParser* msg);
    virtual int AddMessage(SC_MessageParser* msg);
    virtual void ShutDown(SC_MessageParser* msg);
    virtual void Update(int param1, int param2);   // [7]  0x40D6C0
    virtual int Exit(SC_MessageParser* msg);
    virtual int HandleInput(Sprite* spr);          // [11] 0x40DB20
    virtual void ProcessHit();                     // [12] 0x40DB00
    virtual void ProcessAction(int action, int* data); // [13] 0x40EB60

    void ProcessSprite(Sprite* spr); // 0x40DF30
    void SendResultMessage();    // 0x40D7C0
    void UpdateArmSprites();     // 0x40E070
    void CheckTimerExpired(Sprite* spr); // 0x40DEB0

    SpriteAction* spriteAction;    // 0xA8 - action/message object pointer
    int gamePhase;      // 0xAC
    SlimeDim screenSize;     // 0xB0-0xB7 (width/height)
    TimeOut* timer;          // 0xB8 - TimeOut*
    Sprite* consoleSprite;   // 0xBC - CONSOLE_SPRITE
    Palette* palette;        // 0xC0 - PALETTE
    Sprite* bgSprite;        // 0xC4 - BACKGROUND_SPRITE
    Sprite* leftSwitchSprite;  // 0xC8 - LEFT_SWITCH_SPRITE
    Sprite* rightSwitchSprite; // 0xCC - RIGHT_SWITCH_SPRITE
    Sprite* leftSwitchActive;  // 0xD0 - left switch active sprite (alias)
    Sprite* rightSwitchActive; // 0xD4 - right switch active sprite (alias)
    Rect invSlots[2];        // 0xD8-0xF7 (2 x 16 bytes)
    SlotPair closedShots[2]; // 0xF8-0x107 (2 x 8 bytes) - CLOSED_SHOTS_NEEDED
    SlotPair openedShots[2]; // 0x108-0x117 (2 x 8 bytes) - OPENED_SHOTS_NEEDED
    Sprite* leftArmSprite;   // 0x118 - LEFT_ARM_SPRITE
    Sprite* rightArmSprite;  // 0x11C - RIGHT_ARM_SPRITE
    Sprite* leftArmActive;   // 0x120 - left arm active (alias from LEFT_ARM_SPRITE)
    Sprite* rightArmActive;  // 0x124 - right arm active (alias from RIGHT_ARM_SPRITE)
    Sprite* armMaskSprite;   // 0x128 - ARM_MASK_SPRITE
    Sprite* slimeMeterSprite;// 0x12C - SLIME_METER_SPRITE
    int hitCount;            // 0x130 - current hit count
    int maxHits;             // 0x134 - STUDENT_HITS_ALLOWED
    int slimeRound;          // 0x138
    int tentacleShotsNeeded; // 0x13C - TENTACLE_SHOTS_NEEDED
    Rect targetRect;         // 0x140-0x14F (16 bytes)
    SoundList* soundList;    // 0x150 - sound list object
    int field_154;           // 0x154
    Sample* sound1;          // 0x158 - SOUND id=1 sample
    Sample* sound2;          // 0x15C - SOUND id=2 sample
    Sample* sound3;          // 0x160 - SOUND id=3 sample
    Sample* sound4;          // 0x164 - SOUND id=4 sample
    Sample* sound5;          // 0x168 - SOUND id=5 sample
    SlimeTable* slimeTable;  // 0x16C - slime sound table
    int* gameResult;         // 0x170 - dynamic array (7 elements)
    int field_174;           // 0x174
    int field_178;           // 0x178
    int gameResultSize;      // 0x17C - array size (7)
};

#endif // SC_SLIME_H
