#ifndef COMBATSPRITE_H
#define COMBATSPRITE_H

#include "Parser.h"

struct HashNode;         // defined in HashTable.h (next 0x0, bucketIndex 0x4, key 0x8, reserved 0xC)
struct SpriteDataEntry;  // defined in CombatSprite.cpp (index 0x0, spriteIdx 0x4)

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

    // Methods
    void Clear();                              // 0x43E250
    void AllocateBuckets(int size, int flag);
    void* AllocateNode();                      // 0x4422E0
    HashNode* Lookup(volatile int index, int* outSlot);  // 0x4097B0
    void Resize(int size, int flag);           // 0x4097F0
    HashNode* AllocEntry();                    // 0x44BF30
};

// CombatSprite - Manages combat sprite definitions
// Full game vtable: 0x461138
// Size: 0xA0 bytes (Parser(0x90) + 0x10)
class CombatSprite : public Parser {
public:
    CombatSprite();           // 0x408FB0
    virtual ~CombatSprite();  // 0x409020

    virtual void OnProcessStart();             // 0x409210
    virtual int LBLParse(char* line);          // 0x40923E
    void* FindSprite(unsigned int param_1);    // 0x409120
    int PlayById(unsigned int param_1);        // 0x409180
    int ProcessFrame(int frame);               // 0x409730
    void ParseSpriteData(char* line);          // 0x409500

    // Member variables (after Parser 0x90 base)
    SpriteHashTable* spriteTable;      // 0x90
    SpriteDataEntry* currentEntry;     // 0x94 — current sprite data entry
    int currentNode;                   // 0x98 — current linked list node
    int entryList;                     // 0x9C — entry list pointer from hash lookup
};

// Global variables used by CombatSprite parsing
// g_CurrentSprite_004686f8 — declared in globals.h
// g_CurrentSpriteIndex_004686fc, g_CurrentSprite_004686f8 — declared in globals.h

// Helper functions for hash table cleanup
void __cdecl CleanupSpriteHashArray(void** array, int count);  // 0x408F20
void FreePointerArray(void** arr, int count);

#endif // COMBATSPRITE_H
