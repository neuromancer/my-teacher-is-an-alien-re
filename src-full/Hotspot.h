#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "MMPlayer.h"
#include "GlyphRect.h"
#include "SpriteAction.h"

// T_Hotspot - "Teacher Hotspot"
// Full game: Size 0xD8, vtable 0x4619C8
// Constructor: 0x445830, Destructor: 0x4458D0
// Extends Parser (0x90 bytes)
class T_Hotspot : public Parser {
public:
    T_Hotspot(int param_1);         // 0x445830
    ~T_Hotspot();                   // 0x4458D0

    // Fields (full game layout, Parser base = 0x90)
    int field_90;                   // 0x90
    int field_94;                   // 0x94
    int field_98;                   // 0x98 - constructor param
    int currentIndex;               // 0x9C
    int field_A0;                   // 0xA0
    SlimeDim dim_A4;                // 0xA4
    int field_AC;                   // 0xAC
    int field_B0;                   // 0xB0
    void* items[8];                 // 0xB4-0xD3
    int field_D4;                   // 0xD4

    int LBLParse(char* line);       // 0x445A30
    void StopAll();                 // 0x445970
    void SelectItem();              // 0x4459D0
    void DoItem(int param);         // 0x4459A0
};

#endif // HOTSPOT_H
