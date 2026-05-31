#ifndef SC_DODGEORVILLE_H
#define SC_DODGEORVILLE_H

#include "SC_Combat.h"
#include "SpriteAction.h"

// 8-byte embedded object with inline ctor, trivial dtor (no SEH cleanup needed)
struct DodgeDim {
    int x;
    int y;
    DodgeDim() { x = 0; y = 0; }
};

// SC_DodgeOrville - Dodge Orville stink bomb mini-game (case 67/0x43)
// Constructor: 0x428840
// Size: 0x168
// Vtable: 0x4614C8
// Extends SC_Combat (0x118)
// Engine vtable mapping: [1]=OnProcessStart, [2]=OnProcessEnd, [5]=AddMessage, [6]=ShutDown, [11]=ProcessLose
class SC_DodgeOrville : public SC_Combat {
public:
    SC_DodgeOrville();
    ~SC_DodgeOrville();

    virtual int LBLParse(char* line);              // [0]  0x4298A0
    virtual void OnProcessStart();                 // [1]  0x4297D0 - overrides Parser::OnProcessStart
    virtual void OnProcessEnd();                   // [2]  0x429860 - overrides Parser::OnProcessEnd
    virtual void Init(SC_MessageParser* msg);            // [4]  0x4289C0
    int AddMessage(SC_MessageParser* msg);   // [5]  0x428AF0
    void ShutDown(SC_MessageParser* msg);     // [6]  0x428A40 - overrides Handler::ShutDown
    virtual void Update(int p1, int p2);           // [7]  0x428AD0
    virtual int Exit(SC_MessageParser* msg);             // [8]  0x428B30
    virtual void ProcessLose();        // [11] 0x428B60 - overrides SC_Combat::ProcessLose
    virtual void ProcessAction(int action, int* data); // [12] 0x4295C0

    void UpdateGame();                 // 0x429110
    void UpdateReticle();              // 0x429380
    void ThrowBomb();                  // 0x4294A0

    int reticlePos;       // 0x118 - reticle position (0-4, 2=center)
    int cursorDir;        // 0x11C - cursor movement direction (-1, 0, 1)
    int roundReset;        // 0x120 - reset to 0 each round
    DodgeDim throwState;  // 0x124-0x12B (field_0 = throwCount, field_4 = maxThrows) — dtor-less (not unwound)
    Sprite* barFillSprite;// 0x12C - stink bar fill level sprite
    Sprite* barBgSprite;  // 0x130 - stink bar background sprite
    SlimeDim clipStart;   // 0x134-0x13B (x1=0, y1=0 clip region start)
    DodgeDim clipEnd;     // 0x13C-0x143 (x2, y2 clip region end)
    DodgeDim hitCount;    // 0x144-0x14B (field_0=current hits, field_4=max hits)
    SlimeDim barPos;      // 0x14C-0x153 (x, y position of stink bar) — has dtor (unwound at state 2)
    int hitSoundIdx;      // 0x154 - escalating hit sound index
    int cineIds[3];       // 0x158-0x163 — cinematic animation IDs (indexed 0-2)
    int field_164;        // 0x164 — unused (zeroed by memset)
};

#endif // SC_DODGEORVILLE_H
