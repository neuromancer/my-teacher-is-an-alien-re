#ifndef SPRITELIST_H
#define SPRITELIST_H

#include "Parser.h"
#include "Sprite.h"

// Node structure for sprite linked list
struct SpriteNode {
    void* head;     // 0x00
    void* tail;     // 0x04
    void* current;  // 0x08
};

class SpriteList : public Parser {
public:
    int field_0x8c;           // 0x8c - flags
    unsigned char field_0x8d; // 0x8d - byte flags
    char pad_8e[2];           // padding
    int field_0x90;           // 0x90 - done/result flag
    SpriteNode* spriteList;   // 0x94 - pointer to sprite list

    void Init();
    int DoAll();
};

#endif // SPRITELIST_H
