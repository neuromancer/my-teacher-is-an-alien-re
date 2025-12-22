#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Parser.h"
#include "Sprite.h"

class SpriteList;

class Hotspot : public Parser {
public:
    Sprite* sprite; // 0x88
    SpriteList* list1; // 0x8c
    SpriteList* list2; // 0x90
    SpriteList* list3; // 0x94
    char label[32]; // 0x98
    char mouse[32]; // 0xb8
    char pad2[0x11c - 0xb8 - 32];
    int state; // 0x11c
    int dialogParseFileNumber; // 0x120
    int parseFileIndex; // 0x124
    int dialog; // 0x128
    int rect_x; // 0x12c
    int rect_y; // 0x130
    int rect_w; // 0x134
    int rect_h; // 0x138

    int LBLParse(char* line);
    unsigned char Do();
    int SetState(int state);
    int GetState();
    void CleanupSprites();
    int Update(int, int, int);
};

#endif // HOTSPOT_H
