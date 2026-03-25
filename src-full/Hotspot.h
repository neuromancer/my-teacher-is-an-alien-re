#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "MMPlayer.h"
#include "GlyphRect.h"
#include "SpriteAction.h"
#include "HotspotAction.h"

// T_Hotspot - "Teacher Hotspot"
// Full game: Size 0xD8, vtable 0x4619C8
// Constructor: 0x445830, Destructor: 0x4458D0
// Extends Parser (0x90 bytes)
class T_Hotspot : public Parser {
public:
    T_Hotspot(int param_1);         // 0x445830
    ~T_Hotspot();                   // 0x4458D0

    // Fields (full game layout, Parser base = 0x90)
    int parentHandlerId;            // 0x90
    int parentModuleParam;          // 0x94
    int hotspotHandle;              // 0x98 - HANDLE value
    int currentIndex;               // 0x9C
    int flags;                      // 0xA0 - bit flags (2=CONTROLCAPTUREOFF, 4+8=TUCKER, 4+0x10=MARSH, 4+0x20=DANIELS)
    SlimeDim hotspotPos;            // 0xA4 - top-left (left, top)
    int hotspotRight;               // 0xAC
    int hotspotBottom;              // 0xB0
    HotspotAction* items[8];        // 0xB4-0xD3
    HotspotAction* currentAction;   // 0xD4

    int LBLParse(char* line);       // 0x445A30
    void StopAll();                 // 0x445970
    void SelectItem();              // 0x4459D0
    void DoItem(int param);         // 0x4459A0
};

#endif // HOTSPOT_H
