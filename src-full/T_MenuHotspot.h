#ifndef T_MENUHOTSPOT_H
#define T_MENUHOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "GlyphRect.h"

// T_MenuHotspot - Menu button/hotspot widget (extends Parser)
// Full game: Size 0x1A8, Constructor 0x420CE0, Vtable 0x4613B8
// LBLParse: 0x421080 (error string: "T_MenuHotspot")
//
// Full game field layout (Parser base = 0x90):
//   0x90: enabled      — visibility/active flag
//   0x94: state        — selection state (0=normal, 1=selected, 2=active, -1=disabled)
//   0x98: field_98
//   0x9C: bounds       — GlyphRect hit-test rectangle {left, top, right, bottom}
//   0xAC-0x19B: soundEntries[60] — sound data (20 entries of 3 ints each)
//   0x19C: cursor      — Sprite* for rollover cursor animation
class T_MenuHotspot : public Parser {
public:
    T_MenuHotspot(int param);       // 0x420CE0 (full game)
    T_MenuHotspot(char* name, int* rect); // 0x421A50
    ~T_MenuHotspot();               // 0x420D90
    virtual int LBLParse(char* param); // 0x421080

    void Update();                  // 0x420F00
    void Cleanup();                 // 0x420EF0
    void StopCursorSound();         // 0x420EF0
    void SimpleUpdate();            // 0x421BC0 - simple hover check

    // Fields (full game layout, after Parser 0x90 base)
    Sprite* sprite;                 // 0x90
    GlyphRect bounds;               // 0x94 (16 bytes: left, top, right, bottom)
    int field_A4;                   // 0xA4
    int field_A8;                   // 0xA8
    int soundEntries[60];           // 0xAC-0x19B — sound data (20 entries * 3 ints each, stride 0xC)
    Sprite* cursor;                 // 0x19C
    int messageQueue;               // 0x1A0 — Queue* for SpriteAction messages
    int hotspotParam;               // 0x1A4 — constructor parameter (hotspot index/id)
};

#endif // T_MENUHOTSPOT_H
