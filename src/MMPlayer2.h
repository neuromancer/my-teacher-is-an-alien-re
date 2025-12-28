#ifndef MMPLAYER2_H
#define MMPLAYER2_H

#include "Parser.h"

class Queue;
class Sprite;

class MMPlayer2 : public Parser
{
public:
    // 0x88 to 0xbc
    // Assuming padding or unknown fields for now
    int field_88;
    int field_8c;
    int field_90;
    int field_94;
    int field_98;
    int field_9c;
    int field_a0;
    int field_a4;
    int field_a8;
    int field_ac;
    int field_b0;
    int field_b4;
    int field_b8;
    
    Queue* spriteList; // 0xbc

    /* Function start: 0x409030 */
    virtual int LBLParse(char* line);
    
    /* Function start: 0x408C40 */
    void AddSprite(Sprite* s);
};

#endif
