#ifndef RENDERENTRY_H
#define RENDERENTRY_H

#include "GlyphRect.h"

// RenderEntry - base class for render queue entries
// vtable: 0x46102C (full game), 0x431058 (demo)
// Size: 0x14 (20 bytes)
// Layout:
//   0x00: vtable pointer (4 bytes)
//   0x04: GlyphRect rect (16 bytes)
// Scalar deleting destructor: 0x414710 (full game), 0x4016A0 (demo)
struct RenderEntry {
    GlyphRect rect;  // 0x04 - the rectangle to render (vtable is implicit at 0x00)

    virtual ~RenderEntry() {}
};

#endif // RENDERENTRY_H
