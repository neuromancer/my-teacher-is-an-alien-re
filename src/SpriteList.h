#ifndef SPRITELIST_H
#define SPRITELIST_H

// SpriteList class is now merged into MouseControl
// This header provides backwards compatibility
#include "MouseControl.h"

// SpriteNode - linked list header structure used by OptionMenu and others
// This is different from QueueNode - it's the list header, not an individual node
struct SpriteNode {
    void* head;     // 0x00
    void* tail;     // 0x04
    void* current;  // 0x08
};

// SpriteList is typedef'd to MouseControl in MouseControl.h

#endif // SPRITELIST_H
