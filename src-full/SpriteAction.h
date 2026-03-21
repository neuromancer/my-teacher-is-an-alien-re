#ifndef SPRITEACTION_H
#define SPRITEACTION_H

// SlimeDim - generic 8-byte coordinate/dimension pair, destructor at 0x401120.
// The name is legacy; the type is also used for mouse coordinates.
// Inline constructor triggers SEH in containing class constructors.
struct SlimeDim {
    int field_0;
    int field_4;
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

    SpriteAction() {}
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
