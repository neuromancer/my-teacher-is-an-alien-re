#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"
#include "MouseControl.h"
#include "GlyphRect.h"

// SpriteList is typedef'd to MouseControl in MouseControl.h

class Hotspot : public Parser {
public:
    Hotspot();                      // 0x40D300
    virtual ~Hotspot();             // 0x40D3A0

    MouseControl* hotspot;          // 0x88
    MouseControl* right_tool;       // 0x8C
    MouseControl* wrong_tool;       // 0x90
    Queue* pre_message;             // 0x94
    Queue* message;                 // 0x98
    char label[32];                 // 0x9C
    char mouse[20];                 // 0xBC
    int field_D0;                   // 0xD0
    int field_D4;                   // 0xD4
    int state;                      // 0xD8
    int field_DC;                   // 0xDC
    GlyphRect rect;                 // 0xE0
    int field_F0;                   // 0xF0
    
    virtual int LBLParse(char* line); // 0x40D710
    unsigned char Do();               // 0x40D610
    
    void QueueEvents(Queue* q);       // 0x40D6A0
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
    GlyphRect rect;                 // 0x12C
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
