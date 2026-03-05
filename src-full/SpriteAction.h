#ifndef SPRITEACTION_H
#define SPRITEACTION_H

// SlimeDim - 8-byte embedded object, destructor at 0x401120
// Used as a member in SpriteAction, SC_Slime, SCI_PracticeRoom, etc.
// Inline constructor triggers SEH in containing class constructors.
struct SlimeDim {
    int field_0;
    int field_4;
    SlimeDim() { field_0 = 0; field_4 = 0; }
    ~SlimeDim();
};

// SpriteAction (aka SpriteAction) - Chainable action/message struct
// Original class name: "SpriteAction" (string at 0x46C1E0)
// Constructor: 0x444A40 (__thiscall, 10 params)
// Destructor: 0x444AF0
// Size: 0x38 (14 ints equivalent)
// field_34 is a chain pointer to another SpriteAction (recursively destroyed)
class SpriteAction {
public:
    int field_00;   // 0x00 - ADDRESS type
    int field_04;   // 0x04 - ADDRESS id
    int field_08;   // 0x08 - FROM type
    int field_0C;   // 0x0C - FROM id
    int field_10;   // 0x10 - INSTRUCTION
    int field_14;   // 0x14 - EXTRA1
    int field_18;   // 0x18 - EXTRA2
    SlimeDim dim;   // 0x1C-0x23 - MOUSE (x, y)
    int field_24;   // 0x24 - BUTTON1
    int field_28;   // 0x28 - BUTTON2
    int field_2C;   // 0x2C - LASTKEY
    int field_30;   // 0x30 - TIME
    int field_34;   // 0x34 - chain pointer (SpriteAction*)

    SpriteAction(int, int, int, int, int, int, int, int, int, int);
    ~SpriteAction();
    SpriteAction* CopyFrom(SpriteAction*);  // 0x444920
    void Print(int);                         // 0x444B70
    void Serialize(void*);                   // 0x444CD0
};

#endif // SPRITEACTION_H
