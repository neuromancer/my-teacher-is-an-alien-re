#ifndef SC_ROACH_H
#define SC_ROACH_H

#include "SC_Combat.h"
#include "SpriteAction.h"

class Sprite;

// NavCrystal - Crystal piece for SC_Roach puzzle
// Constructor: 0x418690, LBLParse: 0x418850
// Size: 0x4DC
// Vtable: 0x461320
// Extends Parser
class NavCrystal : public Parser {
public:
    NavCrystal(int id);

    int crystalId;               // 0x90
    int field_94;                // 0x94
    int pad_98[0x100];           // 0x98-0x497
    SlimeDim dimArray1[4];       // 0x498-0x4B7
    SlimeDim dimArray2[4];       // 0x4B8-0x4D7
    Sprite* sprite;              // 0x4D8
};

struct CrystalSource {
    int hitboxLeft;   // +0x00
    int hitboxTop;    // +0x04
    int hitboxRight;  // +0x08
    int hitboxBottom; // +0x0C
    int pickupLeft;   // +0x10
    int pickupTop;    // +0x14
    int pickupRight;  // +0x18
    int pickupBottom; // +0x1C
    int field_0x20;   // +0x20
    int field_0x24;   // +0x24
    int crystalPtr;   // +0x28 — assigned crystal object
};

struct GridCell {
    int values[4];    // +0x00..+0x0C — pattern values per cell row
    int pad[4];       // +0x10..+0x1C
};

// SC_Roach - Crystal thought puzzle handler (case 71/0x47)
// Constructor: 0x418C20
// Size: 0x730
// Vtable: 0x461330
// Extends SC_Combat (0x118)
// References: "SC_Roach::Process_Action", "ThotsLvl.cpp"
//
// Field layout:
//   0x118-0x277: CrystalSource[8] — source slots (stride 0x2C)
//   0x278-0x287: grid pattern base (not directly used as separate)
//   0x288-0x707: GridCell[36] — 6x6 grid hitboxes (stride 0x20)
//   0x6F8: int* currentPiece — currently held crystal
//   0x6FC-0x71B: int* crystals[8] — crystal object pointers
//   0x71C: void* progressObj — allocated progress tracker
//   0x720: Sprite* circleSprite
//   0x724: Sprite* barSprite
//   0x728-0x72F: padding
class SC_Roach : public SC_Combat {
public:
    CrystalSource sources[8];    // 0x118-0x277 (8 * 0x2C = 0x160)
    int gridValues[4 * 36];      // 0x278-0x397 (pattern grid data)
    GridCell gridHitboxes[36];   // 0x398 — WRONG, let me recalc...
    // Grid: 6x6=36 cells, each cell 0x20 bytes:
    //   +0x00..+0x0F: 4 pattern values
    //   +0x10..+0x1F: hitbox (left, top, right, bottom)
    int grid[36 * 8];            // 0x278-0x6F7 (36 cells * 32 bytes = 0x480)
    int* currentPiece;           // 0x6F8
    int* crystals[8];            // 0x6FC-0x71B
    void* progressObj;           // 0x71C
    Sprite* circleSprite;        // 0x720
    Sprite* barSprite;           // 0x724
    int field_0x728;             // 0x728
    int field_0x72C;             // 0x72C

    SC_Roach();                                    // 0x418C20
    virtual ~SC_Roach();                           // 0x418EF0 (sdtor)

    virtual int LBLParse(char* line);              // [0]  0x419F50
    virtual void OnProcessEnd();                   // [2]  0x419D70
    virtual void Init(SC_Message* msg);            // [4]  0x418FB0
    virtual int AddMessage(SC_Message* msg);       // [5]  0x419220
    virtual int ShutDown(SC_Message* msg);         // [6]  0x419010
    virtual void Update(int p1, int p2);           // [7]  0x419200
    virtual int Exit(SC_Message* msg);             // [8]  0x419350
    virtual void ProcessLose();                    // [11] 0x419380
    virtual void ProcessAction(int action, int* data); // [12] 0x419C40

    int TryPlacePiece(int* msg);                   // 0x4198B0
    int TryDropOnSource(int* msg);                 // 0x419A10
    int PickFromGrid(int* msg);                    // 0x419AE0
    int PickFromSource(int* msg);                  // 0x419BC0
};

#endif // SC_ROACH_H
