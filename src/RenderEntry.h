#ifndef RENDERENTRY_H
#define RENDERENTRY_H

#include "GlyphRect.h"

// RenderEntry - base class for render queue entries
// vtable: 0x431058
// Size: 0x14 (20 bytes)
struct RenderEntry {
    GlyphRect rect;  // 0x00 - the rectangle to render (no vtable in struct)
    
    ~RenderEntry();  // destructor
};

#endif // RENDERENTRY_H
