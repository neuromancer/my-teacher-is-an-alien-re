#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "MouseControl.h"

// SpriteList is typedef'd to MouseControl in MouseControl.h

class Hotspot : public Parser {
public:
    Hotspot();                      // 0x40D300
    virtual ~Hotspot();             // 0x40D3A0

    Sprite* sprite;                 // 0x88
    SpriteList* list1;              // 0x8C
    SpriteList* list2;              // 0x90
    SpriteList* list3;              // 0x94
    char label[32];                 // 0x98
    char mouse[24];                 // 0xB8
    int field_D0;                   // 0xD0 - set to 1 in constructor
    int field_D4;                   // 0xD4
    int field_D8;                   // 0xD8 - set to 1 in constructor
    int field_DC;                   // 0xDC
    int field_E0;                   // 0xE0
    int field_E4;                   // 0xE4
    int field_E8;                   // 0xE8
    int field_EC;                   // 0xEC
    // Total size: 0xF0 bytes
    
    int LBLParse(char* line);
    unsigned char Do();
    int Draw_40d610();              // 0x40D610
};

// T_Hotspot - "Teacher Hotspot", also known as DialogControl or CharSprite/CharButton base
// Size: 0x140 bytes
// vtable: 0x4311b8
class T_Hotspot : public Parser {
public:
    T_Hotspot();                    // 0x409230
    virtual ~T_Hotspot();           // 0x4092E0

    Sprite* sprite;                 // 0x88
    SpriteList* list1;              // 0x8C
    SpriteList* list2;              // 0x90
    SpriteList* list3;              // 0x94
    char label[32];                 // 0x98
    char mouse[24];                 // 0xB8
    int field_D0;                   // 0xD0
    int field_D4;                   // 0xD4
    int field_D8;                   // 0xD8
    int field_DC;                   // 0xDC
    int field_E0;                   // 0xE0
    int field_E4;                   // 0xE4
    int field_E8;                   // 0xE8
    int field_EC;                   // 0xEC
    char pad_F0[40];                // 0xF0
    int enabled;                    // 0x118 - set to 1
    int state;                      // 0x11C - set to 1
    int dialogParseFileNumber;      // 0x120 - set to -1
    int parseFileIndex;             // 0x124 - set to -1
    int dialog;                     // 0x128
    int rect_x;                     // 0x12C
    int rect_y;                     // 0x130
    int rect_w;                     // 0x134
    int rect_h;                     // 0x138
    int field_13C;                  // 0x13C

    int LBLParse(char* line);       // 0x409620
    unsigned char Do();             // 0x409400
    int SetState(int newState);     // 0x409440
    int GetState();                 // 0x409470
    void Exit();                    // 0x4094A0
    int Update(int, int, int);      // 0x4094F0
};

// Compatibility aliases
typedef T_Hotspot DialogControl;
typedef T_Hotspot HotspotManager;

#endif // HOTSPOT_H
