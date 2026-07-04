#ifndef SC_CRYSTALPUZZLE_H
#define SC_CRYSTALPUZZLE_H

#include "Handler.h"
#include "GlyphRect.h"

class Sprite;
class Palette;
class Sample;
class SC_MessageParser;
class SpriteAction;

// SC_CrystalPuzzle - Force field crystal puzzle (case 38/0x26)
// Constructor: 0x44EBA0
// Destructor: 0x44F4D0
// Size: 0x1C0
// Vtable: 0x461C58
//
// Field offsets verified from assembly:
//   DisplayButtons (0x450890): buttonStates at [this+0x11C], buttons at [this+0xC4]
//   CheckSolution  (0x450670): floorStates at [this+0x110], buttonStates at [this+0x11C],
//                              puzzleSolved at [this+0x140]
//   OnClick        (0x450400): needsUpdate at [this+0x144], rects at [this+0x14C+]
class SC_CrystalPuzzle : public Handler {
public:
    SC_CrystalPuzzle();                         // 0x44EBA0
    ~SC_CrystalPuzzle();                        // 0x44F4D0

    // Virtual overrides
    virtual void Init(SC_MessageParser* msg);         // 0x44F710
    virtual int AddMessage(SC_MessageParser* msg);    // 0x44FAE0
    virtual void ShutDown(SC_MessageParser* msg);      // 0x44FA20
    virtual void Update(int p1, int p2);        // 0x44FE40
    virtual int Exit(SC_MessageParser* msg);          // 0x44FE20
    int FinalizeExit();                               // 0x450110

    // Puzzle methods
    void OnClick(int x, int y);                 // 0x450400
    void OnButtonClick(int buttonIndex);        // 0x450600
    int CheckSolution();                        // 0x450670
    void ResetPuzzle(int idx, int val);          // 0x450760
    void DisplayButtons();                      // 0x450890
    void DisplayMap();                          // 0x4508F0
    void DisplayLitDoors();                     // 0x450980
    void DisplayThisFloorRow();                 // 0x4509F0
    void PlayPuzzleSound(int idx, int loop);    // 0x450B10

    // Fields (0xA8-0x1BF, verified from constructor + method assembly)
    SpriteAction* field_A8;     // 0xA8
    Palette* palette;           // 0xAC
    Sprite* puzztest;           // 0xB0
    Sprite* litdoors;           // 0xB4
    Sprite* paths1;             // 0xB8
    Sprite* paths2;             // 0xBC
    Sprite* paths3;             // 0xC0
    Sprite* buttons1;           // 0xC4 — also accessed as array [this+0xC4 + i*4]
    Sprite* buttons2;           // 0xC8
    Sprite* buttons3;           // 0xCC
    Sprite* lowfloor;           // 0xD0
    Sprite* midfloor;           // 0xD4
    Sprite* topfloor;           // 0xD8
    Sprite* mapSprite;          // 0xDC — full game only ("combats\kidface.smk")
    Sample* sound1;             // 0xE0
    Sample* sound2;             // 0xE4
    int field_E8;               // 0xE8
    int enabledButtons[9];      // 0xEC-0x10F — initialized to 1
    int floorStates[3];         // 0x110-0x11B — set by CheckSolution
    int buttonStates[9];        // 0x11C-0x13F — toggled on click
    int puzzleSolved;           // 0x140
    int field_144;              // 0x144
    int needsUpdate;            // 0x148 — initialized to 1
    GlyphRect rect1;            // 0x14C — (0,0,0x197,0x1aa) main area
    GlyphRect rect2;            // 0x15C — (0x1d7,0x3b,0x25c,0xbb) map area
    GlyphRect rect3;            // 0x16C — (0x1de,0xf1,0x25b,0x16d) button area
    GlyphRect rect4;            // 0x17C — (0x73,0x11d,0x12f,0x149)
    GlyphRect rect5;            // 0x18C — (0x32,0x14a,0x172,0x192)
    GlyphRect rect6;            // 0x19C — exit rect 1
    GlyphRect rect7;            // 0x1AC — exit rect 2
    int puzzleFlags;              // 0x1BC — flags
};

#endif // SC_CRYSTALPUZZLE_H
