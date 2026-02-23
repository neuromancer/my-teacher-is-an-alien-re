#ifndef DRAWENTRY_H
#define DRAWENTRY_H

// Forward declarations
class VBuffer;
struct SoundCommand;

// DrawEntry - represents an entry in the draw/render queue (m_queueA4)
// Size: 0x0C (12 bytes)
// No vtable - destructor is non-virtual
// 0x411080 is the compiler-generated scalar deleting destructor (base dtor body inlined)
// Used by ZBufferManager for managing render queue entries
class DrawEntry {
public:
    int field_0;                     // 0x00
    VBuffer* m_videoBuffer;          // 0x04 - pointer to video buffer data
    SoundCommand* m_childObject;     // 0x08 - pointer to child render/sound command
    ~DrawEntry();
};

#endif // DRAWENTRY_H

