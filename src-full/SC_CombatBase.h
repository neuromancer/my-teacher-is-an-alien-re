#ifndef SC_COMBATBASE_H
#define SC_COMBATBASE_H

#include "Parser.h"

// SC_CombatBase - Base class for combat/exploration engines
// Constructor: 0x42BCD0, Destructor: 0x42BDA0
// Scalar deleting destructor: 0x42BD80
// Vtable: 0x461550 (18 entries)
// Size: 0xF0
//
// Both EngineA and EngineB inherit from this class in the full game.
//
// vtable layout:
//   [0]  LBLParse           0x42CB10
//   [1]  OnProcessStart     0x401140 (inherited)
//   [2]  OnProcessEnd       0x401150 (inherited)
//   [3]  ~SC_CombatBase     0x42BD80 (sdtor)
//   [4]  Initialize         0x42C240
//   [5]  CleanupAll         0x42C630
//   [6]  ResetState         0x42C230
//   [7]  ProcessInput       0x42C960
//   [8]  method8            0x42C070
//   [9]  method9            0x42C050
//   [10] method10           0x42BFC0
//   [11] method11           0x42C120
//   [12] method12           0x40BC80
//   [13] HandleAction       0x42BD70
//   [14] StopAndCleanup     0x42BF20
//   [15] SetupViewport      0x42C8A0
//   [16] RenderState          0x42C920
//   [17] UpdateAndCheck     0x40BC90
class SC_CombatBase : public Parser {
public:
    // Fields from 0x90 to 0xEF (24 dwords, all zeroed in constructor)
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9C;
    int field_0xA0;
    int field_0xA4;
    int field_0xA8;
    int field_0xAC;
    int field_0xB0;
    int field_0xB4;
    int field_0xB8;
    int field_0xBC;
    int field_0xC0;
    int field_0xC4;
    int field_0xC8;
    int field_0xCC;
    int field_0xD0;
    int field_0xD4;
    int field_0xD8;
    int field_0xDC;
    int field_0xE0;
    int field_0xE4;
    int field_0xE8;
    int field_0xEC;

    SC_CombatBase();
    virtual ~SC_CombatBase();

    virtual int LBLParse(char* line);       // [0] 0x42CB10
    // [1] OnProcessStart - inherited from Parser (0x401140)
    // [2] OnProcessEnd - inherited from Parser (0x401150)
    // [3] destructor (above)
    virtual void Initialize();              // [4] 0x42C240
    virtual void CleanupAll();              // [5] 0x42C630
    virtual void ResetState();              // [6] 0x42C230
    virtual void ProcessInput();            // [7] 0x42C960
    virtual void method8();                 // [8] 0x42C070
    virtual void method9();                 // [9] 0x42C050
    virtual void method10();               // [10] 0x42BFC0
    virtual void method11();               // [11] 0x42C120
    virtual int method12();                // [12] 0x40BC80
    virtual int HandleAction(int* param);  // [13] 0x42BD70
    virtual void StopAndCleanup();         // [14] 0x42BF20
    virtual void SetupViewport();          // [15] 0x42C8A0
    virtual void RenderState();              // [16] 0x42C920
    virtual int UpdateAndCheck();          // [17] 0x40BC90
};

#endif // SC_COMBATBASE_H
