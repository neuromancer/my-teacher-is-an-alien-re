#ifndef SPRITELIST_H
#define SPRITELIST_H

// SpriteList class is now merged into MouseControl
// This header provides backwards compatibility
#include "MouseControl.h"

struct SpriteListNode;

// SpriteNode - linked list header structure used by OptionMenu and others
// This is the same layout as PriorityQueue
struct SpriteNode {
    SpriteListNode* head;     // 0x00
    SpriteListNode* tail;     // 0x04
    SpriteListNode* current;  // 0x08
};

// SpriteListNode - individual node in the linked list
// This is the same layout as PriorityQueueNode
struct SpriteListNode {
    SpriteListNode* next;     // 0x00
    SpriteListNode* prev;     // 0x04
    void* data;               // 0x08
};

// SpriteData - the data stored in each node
// This is the same layout as CharSprite
struct SpriteData {
    int field_0x00;           // 0x00
    char name[64];            // 0x04
    int priority;             // 0x44
    int state;                // 0x48
    int rect_x;               // 0x4c
    int rect_y;               // 0x50
    int rect_w;               // 0x54
    int rect_h;               // 0x58
};

// SpriteList is typedef'd to MouseControl in MouseControl.h

#endif // SPRITELIST_H
