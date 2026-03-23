#ifndef SC_DODGEORVILLE_H
#define SC_DODGEORVILLE_H

#include "SC_Combat.h"
#include "SpriteAction.h"

// 8-byte embedded object with inline ctor, trivial dtor (no SEH cleanup needed)
struct DodgeDim {
    int field_0;
    int field_4;
    DodgeDim() { field_0 = 0; field_4 = 0; }
};

// SC_DodgeOrville - Dodge Orville stink bomb mini-game (case 67/0x43)
// Constructor: 0x428840
// Size: 0x168
// Vtable: 0x4614C8
// Extends SC_Combat (0x118)
// Engine vtable mapping: [5]=AddMessage, [6]=Cleanup, [11]=ProcessTargets
class SC_DodgeOrville : public SC_Combat {
public:
    SC_DodgeOrville();
    ~SC_DodgeOrville();

    void Cleanup(int flag);            // 0x428A40 - overrides Engine::VirtCleanup [6]
    int AddMessage(SC_Message* msg);   // 0x428AF0 - overrides Engine::CleanupSubsystems [5]
    void ProcessTargets();             // 0x428B60 - overrides Engine::ProcessTargets [11]

    void UpdateGame();                 // 0x429110
    void UpdateReticle();              // 0x429380
    void ThrowBomb();                  // 0x4294A0
    void InitGameState();              // 0x4297D0
    void InitReset();                  // 0x429860

    int reticlePos;       // 0x118 - reticle position (0-4, 2=center)
    int cursorDir;        // 0x11C - cursor movement direction (-1, 0, 1)
    int field_120;        // 0x120 - reset to 0 each round
    SlimeDim throwState;  // 0x124-0x12B (field_0 = throwCount, field_4 = maxThrows)
    Sprite* barFillSprite;// 0x12C - stink bar fill level sprite
    Sprite* barBgSprite;  // 0x130 - stink bar background sprite
    SlimeDim clipStart;   // 0x134-0x13B (x1=0, y1=0 clip region start)
    DodgeDim clipEnd;     // 0x13C-0x143 (x2, y2 clip region end)
    DodgeDim hitCount;    // 0x144-0x14B (field_0=current hits, field_4=max hits)
    DodgeDim barPos;      // 0x14C-0x153 (x, y position of stink bar)
    int hitSoundIdx;      // 0x154 - escalating hit sound index
    int field_158;        // 0x158
    int field_15C;        // 0x15C
    int field_160;        // 0x160
    int field_164;        // 0x164
};

#endif // SC_DODGEORVILLE_H
