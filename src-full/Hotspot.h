#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "MMPlayer.h"
#include "GlyphRect.h"
#include "SpriteAction.h"

// SpriteList is typedef'd to MMPlayer in MMPlayer.h

class Hotspot : public Parser {
public:
    Hotspot();                      // 0x40D300
    ~Hotspot();             // 0x40D3A0

    MMPlayer* hotspot;          // 0x88
    MMPlayer* right_tool;       // 0x8C
    MMPlayer* wrong_tool;       // 0x90
    Queue* pre_message;             // 0x94
    Queue* message;                 // 0x98
    char label[32];                 // 0x9C
    char mouse[20];                 // 0xBC
    int field_D0;                   // 0xD0
    int field_D4;                   // 0xD4
    int state;                      // 0xD8
    int field_DC;                   // 0xDC
    GlyphRect rect;                 // 0xE0

    virtual int LBLParse(char* line); // 0x40D710
    int Do();                         // 0x40D610

    void QueueEvents(Queue* q);       // 0x40D6A0
};

// T_Hotspot - "Teacher Hotspot"
// Full game: Size 0xD8, vtable 0x4619C8
// Constructor: 0x445830, Destructor: 0x4458D0
// Extends Parser (0x90 bytes)
//
// Demo layout (used by SCI_SearchScreen demo code):
//   0x88: sprite, 0x8C-0x94: list1-3, 0x98: label[32],
//   0x118: enabled, 0x11C: state, 0x12C: rect
// Full game layout:
//   0x90-0xD7: new fields, 0x98: param, 0x9C: currentIndex,
//   0xA4: SlimeDim, 0xB4: items[8]
class T_Hotspot : public Parser {
public:
    T_Hotspot();                    // demo default
    T_Hotspot(int param_1);         // 0x445830
    ~T_Hotspot();                   // 0x4458D0

    // Demo layout fields (kept for SCI_SearchScreen demo code compatibility)
    Sprite* sprite;                 // 0x88
    SpriteList* list1;              // 0x8C
    int field_90;                   // 0x90
    int field_94;                   // 0x94
    int field_98;                   // 0x98 - full game: constructor param
    int currentIndex;               // 0x9C
    int field_A0;                   // 0xA0
    SlimeDim dim_A4;                // 0xA4
    int field_AC;                   // 0xAC
    int field_B0;                   // 0xB0
    void* items[8];                 // 0xB4-0xD3
    int field_D4;                   // 0xD4
    int field_D8;                   // 0xD8
    int field_DC;                   // 0xDC
    int field_E0;                   // 0xE0
    int field_E4;                   // 0xE4
    int field_E8;                   // 0xE8
    int field_EC;                   // 0xEC
    char pad_F0[40];                // 0xF0
    int enabled;                    // 0x118
    int state;                      // 0x11C
    int dialogParseFileNumber;      // 0x120
    int parseFileIndex;             // 0x124
    int dialog;                     // 0x128
    GlyphRect rect;                 // 0x12C
    int field_13C;                  // 0x13C

    int LBLParse(char* line);       // 0x445A30

    void StopAll();                  // 0x445970
    void SelectItem();              // 0x4459D0
    void DoItem(int param);         // 0x4459A0

    // Demo stubs (still referenced by SCI_SearchScreen, SC_SelectHotSpot)
    void Exit();
    int Update(int, int, int);
};

// Compatibility aliases
typedef T_Hotspot DialogControl;
typedef T_Hotspot HotspotManager;

#endif // HOTSPOT_H
