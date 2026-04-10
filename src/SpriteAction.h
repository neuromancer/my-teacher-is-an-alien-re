#ifndef SPRITEACTION_H
#define SPRITEACTION_H

#include <string.h>

// SlimeDim - generic 8-byte coordinate/dimension pair.
// Destructor: 0x401120 (empty/trivial, only reached via SEH unwind funclets).
// Constructor: inline, zeros both fields.
//
// IMPORTANT - DO NOT CHANGE constructor/destructor presence:
// Evidence from original binary:
//   - SEH funclets (e.g. 0x444B5B, 0x407503) JMP to 0x401120 with ECX=this+SlimeDim_offset
//   - SpriteAction constructor (0x444A40) has 1 SEH state for mousePos (SlimeDim at +0x1C)
//   - Classes with multiple SlimeDim members (SC_FireAlarm, EngineInfoParser) have
//     incrementing SEH states per member, proving each is constructed individually
//   - 0x401120 is never CALLed directly, only via SEH unwind — body is likely just RET
//
// Both constructor AND destructor must exist to generate correct SEH frames.
// Removing either breaks similarity for: SpriteAction, EngineInfoParser, SC_FireAlarm,
// SC_Roach, and any class containing SlimeDim members.
struct SlimeDim {
    int x;
    int y;
    SlimeDim() { x = 0; y = 0; }
    ~SlimeDim();
};

// SpriteAction - chainable action/message payload.
// Original class name: "SpriteAction" (string at 0x46C1E0)
// Constructor: 0x444A40 (__thiscall, 10 params)
// Destructor: 0x444AF0
// Size: 0x38 (14 ints equivalent)
// Field meanings are evidenced by the 0x444B70 dump routine.
class SpriteAction {
public:
    int addressType;           // 0x00
    int addressValue;          // 0x04
    int fromType;              // 0x08
    int fromValue;             // 0x0C
    int instruction;           // 0x10
    int extra1;                // 0x14
    int extra2;                // 0x18
    SlimeDim mousePos;         // 0x1C-0x23
    int button1;               // 0x24
    int button2;               // 0x28
    int lastKey;               // 0x2C
    int time;                  // 0x30
    SpriteAction* childAction; // 0x34 - owned nested SpriteAction*

    SpriteAction() { mousePos.x = 0; mousePos.y = 0; memset(this, 0, sizeof(SpriteAction)); }
    SpriteAction(int, int, int, int, int, int, int, int, int, int);
    ~SpriteAction();
    SpriteAction* CopyFrom(SpriteAction*);  // 0x444920
    void Print(int);                         // 0x444B70
    void Serialize(void*);                   // 0x444CD0
};

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
void EnqueueSpriteAction(void*);
void EnqueueSpriteAction(SpriteAction*);

#endif // SPRITEACTION_H
