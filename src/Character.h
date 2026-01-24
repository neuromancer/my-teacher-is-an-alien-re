#ifndef CHARACTER_H
#define CHARACTER_H

#include <windows.h>
#include "GlyphRect.h"

// Forward declarations
struct CharSprite;
struct PriorityQueue;
struct PriorityQueueNode;

// PriorityQueueNode - Node in the priority queue
// Size: 0xC bytes (12 bytes)
struct PriorityQueueNode {
    PriorityQueueNode* next;    // +0x00
    PriorityQueueNode* prev;    // +0x04
    CharSprite* data;           // +0x08

    PriorityQueueNode(CharSprite* s); // 0x404470
};

// PriorityQueue - Priority queue for CharSprites
// Size: 0x10 bytes (16 bytes)
struct PriorityQueue {
    PriorityQueueNode* head;    // +0x00 (List end/Last node)
    PriorityQueueNode* tail;    // +0x04 (List start/First node)
    PriorityQueueNode* current; // +0x08
    int type;                   // +0x0C

    void AddAfterCurrent(CharSprite* sprite);
};

// CharSprite - Sprite for character display
// Size: 0x5C bytes (92 bytes)
struct CharSprite {
    int field_00;               // +0x00
    char name[64];              // +0x04 - name/label string
    int priority;               // +0x44
    int field_48;               // +0x48
    GlyphRect bounds;           // +0x4c

    CharSprite();               // 0x408880
};

// Character - Game character object
// Size: 0x40 bytes (64 bytes)
// Used for peter, susan, duncan character data
//
// Layout:
//   0x00: int characterType    (-1 initially, 0=peter, 1=susan, 2=duncan)
//   0x04-0x20: int abilities[8] (8 ability flags)
//   0x24-0x34: padding (zeros)
//   0x38: PriorityQueue* queue
//   0x3C: unused pointer
class Character {
public:
    // Constructor - Address: 0x403700
    Character(char* name);

    int characterType;          // +0x00
    int abilities[8];           // +0x04-0x20 (offsets 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20)
    int padding[5];             // +0x24-0x34
    PriorityQueue* queue;       // +0x38
    void* field_3c;             // +0x3C
};

// External functions used by Character

#endif // CHARACTER_H
