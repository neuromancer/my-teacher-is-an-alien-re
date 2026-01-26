#ifndef HANDLER4_H
#define HANDLER4_H

#include "IconBar.h"
#include "Palette.h"
#include "Sprite.h"
#include "Sample.h"
#include "GlyphRect.h"

class SC_Message;

// Handler4 - Command 4 Handler (Forcefield Puzzle / SCIpuzz1)
// Size: 0x6F0 bytes
// vtable: 0x431288
// Inherits from IconBar
// Layout:
//   0x00-0x5FF: IconBar base class
//   0x600: Palette* palette
//   0x604-0x62C: Sprite* pointers for various puzzle elements
//   0x630-0x634: Sample* sound pointers
//   0x638-0x65F: int soundStates[10]
//   0x660-0x66B: int floorStates[3]
//   0x66C-0x68F: int buttonStates[9]
//   0x690: int puzzleSolved
//   0x694: int initialized
//   0x698: int needsUpdate
//   0x69C-0x6EF: rect coordinates for clickable regions
class Handler4 : public IconBar {
public:
    Handler4();
    ~Handler4();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler4 methods
    void ResetPuzzle();
    void OnClick(int x, int y);
    void OnButtonClick(int buttonIndex);
    int CheckSolution();
    void DisplayButtons();
    void DisplayPaths();
    void DisplayLitDoors();
    void DisplayFloors();
    void PlaySound(int index, int loop);

    // Handler4-specific fields starting at 0x600
    Palette* palette;       // 0x600
    Sprite* puzztest;       // 0x604
    Sprite* litdoors;       // 0x608
    Sprite* paths1;         // 0x60C
    Sprite* paths2;         // 0x610
    Sprite* paths3;         // 0x614
    Sprite* buttons1;       // 0x618
    Sprite* buttons2;       // 0x61C
    Sprite* buttons3;       // 0x620
    Sprite* lowfloor;       // 0x624
    Sprite* midfloor;       // 0x628
    Sprite* topfloor;       // 0x62C
    Sample* sound1;         // 0x630
    Sample* sound2;         // 0x634
    int soundStates[10];    // 0x638
    int floorStates[3];     // 0x660
    int buttonStates[9];    // 0x66C
    int puzzleSolved;       // 0x690
    int initialized;        // 0x694
    int needsUpdate;        // 0x698
    GlyphRect rect1;        // 0x69C
    GlyphRect rect2;        // 0x6AC
    GlyphRect rect3;        // 0x6BC
    GlyphRect rect4;        // 0x6CC
    GlyphRect rect5;        // 0x6DC
    int field_6EC;          // 0x6EC padding
};

#endif // HANDLER4_H
