#ifndef COMBATSPRITE_H
#define COMBATSPRITE_H

#include "Parser.h"

// SpriteHashTable - Hash table for combat sprite management
// Size: 0x18 bytes
class SpriteHashTable {
public:
    void** buckets;     // 0x00 - pointer to bucket array
    int maxSize;        // 0x04 - max number of buckets (default 0x11 = 17)
    void* head;         // 0x08 - head of entry list
    void* tail;         // 0x0c - tail of entry list
    int count;          // 0x10 - number of entries
    int growSize;       // 0x14 - growth increment (default 0xa = 10)

    // Inline constructors
    SpriteHashTable() {
        head = 0;
        tail = 0;
        maxSize = 0;
        buckets = 0;
        count = 0;
        growSize = 10;
    }
    SpriteHashTable(int size) {
        buckets = 0;
        maxSize = size;
        head = 0;
        tail = 0;
        count = 0;
        growSize = 10;
    }

    // Methods - thiscall convention
    void* Lookup(int index, int* outSlot);  // 0x415c10
    void Resize(int size, int flag);        // 0x415c50
    void* AllocEntry();                     // 0x415cb0
};

// CombatSprite - Manages combat sprite definitions
// Size: 0x8C bytes (Parser + 1 pointer)
// vtable: part of combat engine
//
// Layout:
//   0x00-0x87: Parser base class
//   0x88: SpriteHashTable* spriteTable
class CombatSprite : public Parser {
public:
    CombatSprite();
    virtual ~CombatSprite();

    virtual int LBLParse(char* line);
    void ParseSpriteData(char* line);  // 0x415960

    // Member variables
    SpriteHashTable* spriteTable;  // 0x88
};

// Global variables used by CombatSprite parsing
extern SpriteHashTable* g_CurrentSprite;   // 0x436348
extern int g_CurrentSpriteIndex;           // 0x43634c

#endif // COMBATSPRITE_H
