#ifndef SC_GAUNTLET_H
#define SC_GAUNTLET_H

#include "SC_Combat.h"
#include "InvSlotItem.h"  // for Rect

// GauntletEntry — 0x1C byte element with ctor/dtor (ctor 0x42E660, dtor 0x42E6D0)
struct GauntletEntry {
    int fields[7]; // 0x00-0x1B
    GauntletEntry();
    ~GauntletEntry();
};

// SC_Gauntlet - Android gauntlet mini-game (case 80/0x50)
// Constructor: 0x42E540
// Size: 0x560
// Vtable: 0x461610
// Extends SC_Combat (0x118)
class SC_Gauntlet : public SC_Combat {
public:
    SC_Gauntlet();                                  // 0x42E540
    ~SC_Gauntlet();                                 // 0x42E7C0

    virtual int LBLParse(char* line);                // 0x42F910
    virtual void OnProcessEnd();                    // 0x42F800
    virtual void Init(SC_MessageParser* msg);             // 0x42E870
    virtual void ShutDown(SC_MessageParser* msg);          // 0x42E8F0
    virtual void Update(int p1, int p2);            // 0x42E9B0
    virtual int AddMessage(SC_MessageParser* msg);        // 0x42E9D0
    virtual int Exit(SC_MessageParser* msg);              // 0x42EB70

    virtual void ProcessLose();                         // [11] 0x42EBA0
    virtual void ProcessAction(int action, int* data);  // [12] 0x42F5E0

    void ProcessGrid(int row, int col);             // 0x42F220
    void ResetGrid();                               // 0x42F570
    void RenderGrid();                              // 0x42EFC0

    int moveState[2];              // 0x118-0x11F
    int crystalState[2];              // 0x120-0x127
    Rect boardBounds;              // 0x128-0x137 (boardLeft,Top,Right,Bottom)
    Sprite* fgSprite;              // 0x138
    GauntletEntry entries[36];     // 0x13C-0x52B (36 * 0x1C = 0x3F0)
    Sprite* cellSprites[7];        // 0x52C-0x547
    Rect exitBounds;               // 0x548-0x557 (exitLeft,Top,Right,Bottom)
    Sprite* exitSprite;            // 0x558
    int field_55C;                 // 0x55C
};

#endif // SC_GAUNTLET_H
