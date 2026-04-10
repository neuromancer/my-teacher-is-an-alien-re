#ifndef MMPLAYER2_H
#define MMPLAYER2_H

#include "Parser.h"
#include "ZBufferManager.h"

class Sprite;

// MMPlayer2 (formerly SC_Dialog) - Dialog sprite queue manager
// Size: 0xC0 bytes
// Constructor: 0x408970
// Destructor: 0x408A40
// vtable: 0x4311a0
// Inherits from Parser (0x88 bytes)
//
// Layout:
//   0x00-0x87: Parser base class
//   0x88-0xbb: fields (14 ints = 56 bytes)
//   0xbc: ZBQueue* (sprite queue)
class MMPlayer2 : public Parser {
public:
    MMPlayer2();
    ~MMPlayer2();

    virtual int LBLParse(char* line); // 0x409030
    void AddSprite(Sprite* spr);    // 0x408C40
    void StopAll();         // 0x408B60
    void PreDraw();         // 0x408BD0
    int Draw();            // 0x408EE0
    int DrawWithStates(int* states); // 0x408F80

    int field_88;           // 0x88 - set to 0x54 in constructor
    int field_8c;           // 0x8c
    int field_90;           // 0x90 - set to 1 in constructor
    int field_94;           // 0x94
    int field_98;           // 0x98
    int field_9c;           // 0x9c
    int field_a0;           // 0xa0
    int field_a4;           // 0xa4
    int field_a8;           // 0xa8
    int field_ac;           // 0xac
    int field_b0;           // 0xb0
    int field_b4;           // 0xb4
    int field_b8;           // 0xb8
    ZBQueue* spriteList;    // 0xbc
};

#endif // MMPLAYER2_H
