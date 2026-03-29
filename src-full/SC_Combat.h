#ifndef SC_COMBAT_H
#define SC_COMBAT_H

#include "Handler.h"

class SpriteAction;
class Sprite;
class Palette;
class SoundList;

// SC_Combat - Base class for combat/mini-game handlers
// Constructor: 0x449100
// Size: 0x118
// Vtable: 0x461A90
// Extends Handler (0xA8)
// Layout:
//   0x00-0xA7: Handler base
//   0xA8-0x117: SC_Combat fields (0x1C DWORDs, zeroed)
class SC_Combat : public Handler {
public:
    int* statusPtr;            // 0xA8 — pointer to game status flags
    int field_0xAC[21];        // 0xAC-0xFF
    SpriteAction* pendingAction; // 0x100 — pending action message
    Palette* field_0x104;      // 0x104
    Sprite* bgSprite;          // 0x108 — background/animation sprite
    Sprite* field_0x10C;       // 0x10C
    SoundList* bgSound;        // 0x110 — background sound list
    int field_0x114;           // 0x114

    SC_Combat();
    ~SC_Combat();

    virtual int LBLParse(char* line);          // [0]  0x449600
    virtual void OnProcessEnd();               // [2]  0x449520
    virtual void Init(SC_Message* msg);        // [4]  0x449260
    virtual int AddMessage(SC_Message* msg);   // [5]  0x449400
    virtual int ShutDown(SC_Message* msg);     // [6]  0x449320
    virtual void Update(int p1, int p2);       // [7]  0x449480
    virtual int Exit(SC_Message* msg);         // [8]  0x449410
    virtual void ProcessLose();                // [11] 0x4494E0
    virtual void ProcessAction(int action, int* data); // [12] 0x4491B0
};

#endif // SC_COMBAT_H
