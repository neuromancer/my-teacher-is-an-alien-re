#ifndef T_MENUHOTSPOT_H
#define T_MENUHOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "GlyphRect.h"
#include "Memory.h"

struct Queue;

// T_MenuButton - Simple menu button widget (extends Parser)
// Full game: Size 0xA8, Constructor 0x421A50, Vtable 0x4613D0
// Standalone destructor COMDAT at 0x43B720 (cleans sprite only)
//
// Full game field layout (Parser base = 0x90):
//   0x90: sprite       — Sprite* for button visual
//   0x94: bounds       — GlyphRect hit-test rectangle {left, top, right, bottom}
//   0xA4: activeRight
class T_MenuButton : public Parser {
public:
    T_MenuButton(char* name, int* rect); // 0x421A50

    ~T_MenuButton() {
        if (sprite != 0) {
            delete sprite;
            sprite = 0;
        }
    }

    void Update();                  // 0x420F00
    void Cleanup();                 // 0x420EF0
    void SimpleUpdate();            // 0x421BC0 - simple hover check

    // Fields (full game layout, after Parser 0x90 base)
    Sprite* sprite;                 // 0x90
    GlyphRect bounds;               // 0x94 (16 bytes: left, top, right, bottom)
    int activeRight;                // 0xA4
};

// T_MenuHotspot - Full menu hotspot (extends Parser)
// Full game: Size 0x1A8, Constructor 0x420CE0, Vtable 0x4613B8, Destructor 0x420D90
//
// Full game field layout (Parser base = 0x90):
//   0x90: sprite       - non-owned sentinel/control value
//   0x94: state        - 0/1 idle-selected, 2/3 hover, -1 disabled
//   0x98: mouseMode    - parsed MOUSE value
//   0x9C: bounds       - GlyphRect hit-test rectangle
//   0xAC-0x19B: soundEntries[60] — sound data (20 entries * 3 ints each, stride 0xC)
//   0x19C: cursor      — Sprite* for rollover cursor animation
//   0x1A0: messageQueue — Queue* for SpriteAction messages
//   0x1A4: hotspotParam — constructor parameter (hotspot index/id)
class T_MenuHotspot : public Parser {
public:
    T_MenuHotspot(int param);       // 0x420CE0 (full game)
    ~T_MenuHotspot();               // 0x420D90

    virtual int LBLParse(char* param); // 0x421080
    void Update() { ((T_MenuButton*)this)->Update(); }
    void StopCursorSound();         // 0x420EF0
    void ProcessSpriteActions();    // 0x421020

    Sprite* sprite;                 // 0x90
    int state;                      // 0x94
    int mouseMode;                  // 0x98
    GlyphRect bounds;               // 0x9C
    int soundEntries[60];           // 0xAC-0x19B — sound data (20 entries * 3 ints each, stride 0xC)
    Sprite* cursor;                 // 0x19C
    Queue* messageQueue;            // 0x1A0 — Queue* for SpriteAction messages
    int hotspotParam;               // 0x1A4
};

#endif // T_MENUHOTSPOT_H
