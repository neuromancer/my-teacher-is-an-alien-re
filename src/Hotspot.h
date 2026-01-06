#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"

class SpriteList;

class Hotspot : public Parser {
public:
    Hotspot();
    Sprite* sprite; // 0x88
    SpriteList* list1; // 0x8c
    SpriteList* list2; // 0x90
    SpriteList* list3; // 0x94
    char label[32]; // 0x98
    char mouse[24]; // 0xb8
    int field_D0; // 0xd0
    int field_D4; // 0xd4
    int field_D8; // 0xd8
    int field_DC; // 0xdc
    int field_E0; // 0xe0
    int field_E4; // 0xe4
    int field_E8; // 0xe8
    int field_EC; // 0xec
    
    char pad_F0[44]; // 0xf0 -> 0x11c matched
    
    int state; // 0x11c
    int dialogParseFileNumber; // 0x120
    int parseFileIndex; // 0x124
    int dialog; // 0x128
    int rect_x; // 0x12c
    int rect_y; // 0x130
    int rect_w; // 0x134
    int rect_h; // 0x138

    virtual ~Hotspot();
    int LBLParse(char* line);
    unsigned char Do();
    int SetState(int state);
    int GetState();
    void CleanupSprites();
    int Update(int, int, int);
    int Draw_40d610();
};

#endif // HOTSPOT_H
