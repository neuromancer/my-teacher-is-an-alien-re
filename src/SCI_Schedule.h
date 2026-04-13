#ifndef SCI_SCHEDULE_H
#define SCI_SCHEDULE_H

#include "IconBar.h"

class SC_MessageParser;
class Palette;
class Sprite;
class T_MenuHotspot;

// SCI_Schedule - School schedule/map screen (case 39/0x27)
// Constructor: 0x434C10, Destructor: 0x434FB0
// Size: 0xD8, Vtable: 0x4616F8
// Extends IconBar
// References: "mis\\schedule.mis", "schedual\\schedule.col", "HOTSPOT"
class SCI_Schedule : public IconBar {
public:
    SCI_Schedule();                                 // 0x434C10
    ~SCI_Schedule();                                // 0x434FB0

    virtual int LBLParse(char* param);              // 0x4356E0
    virtual void Init(SC_MessageParser* msg);             // 0x4350C0
    virtual int AddMessage(SC_MessageParser* msg);        // 0x435440
    virtual int ShutDown(SC_MessageParser* msg);          // 0x4353E0
    virtual void Update(int param1, int param2);    // 0x435520
    virtual int Exit(SC_MessageParser* msg);              // 0x435500

    Palette* palette;           // 0xA8 — "schedual\\schedule.col"
    Sprite* bgSprite;           // 0xAC — main background
    Sprite* selBox1;            // 0xB0 — selection highlight box 1
    Sprite* selBox2;            // 0xB4 — selection highlight box 2
    Sprite* tardiesSprite;      // 0xB8 — tardies counter
    Sprite* cutsSprite;         // 0xBC — cuts counter
    Sprite* scheduleSprite;     // 0xC0 — schedule overlay
    int field_C4;               // 0xC4
    int selectionState;         // 0xC8 — 0=box1, 1=box2
    int renderX;                // 0xCC — selection box X position
    int renderY;                // 0xD0 — selection box Y position
    T_MenuHotspot* mapHotspot;  // 0xD4 — HOTSPOT (0x1A8 bytes)
};

#endif // SCI_SCHEDULE_H
