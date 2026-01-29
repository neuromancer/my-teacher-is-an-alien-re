#ifndef DRAWENTRY_H
#define DRAWENTRY_H

// Forward declarations
class VBuffer;
struct SoundCommand;

// DrawEntry - represents an entry in the draw/render queue (m_queueA4)
// Size: 0x0C (12 bytes minimum)
// vtable: unknown
// Used by ZBufferManager for managing render queue entries
class DrawEntry {
public:
    // Virtual destructor at vtable offset 0x00
    virtual ~DrawEntry();
    
    VBuffer* m_videoBuffer;  // 0x04 - pointer to video buffer data
    SoundCommand* m_childObject;     // 0x08 - pointer to child render/sound command
};

#endif // DRAWENTRY_H

