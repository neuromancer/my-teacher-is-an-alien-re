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
    void Clear();                           // 0x415220
    void AllocateBuckets(int size, int flag);  // 0x415270
    void* AllocateNode();                   // 0x4152D0
    void* Lookup(int index, int* outSlot);  // 0x415c10
    void Resize(int size, int flag);        // 0x415c50
    void* AllocEntry();                     // 0x415cb0
};

// CombatSprite - Manages combat sprite definitions
// Size: 0x98 bytes (Parser + 16 bytes)
// vtable: 0x4314a8
//
// Layout:
//   0x00-0x87: Parser base class
//   0x88: SpriteHashTable* spriteTable
//   0x8c-0x94: reserved fields
class CombatSprite : public Parser {
public:
    CombatSprite();      // 0x415410
    virtual ~CombatSprite();  // 0x415480

    virtual int LBLParse(char* line);
    void* FindSprite(unsigned int param_1); // 0x415580
    int PlayById(unsigned int param_1);  // 0x4155E0
    int ProcessFrame(int frame);         // 0x415B90
    void ParseSpriteData(char* line);  // 0x415960

    // Member variables
    SpriteHashTable* spriteTable;  // 0x88
    int field_0x8c;                // 0x8c
    int field_0x90;                // 0x90
    int field_0x94;                // 0x94
};

// Global variables used by CombatSprite parsing
extern SpriteHashTable* g_CurrentSprite;   // 0x436348
extern int g_CurrentSpriteIndex;           // 0x43634c

// Helper functions for hash table cleanup
void FreePointerArray(void** arr, int count);         // 0x415340
void FreeNestedHashTables(void** arr, int count);     // 0x415380

#endif // COMBATSPRITE_H
