#ifndef DRAWENTRY_H
#define DRAWENTRY_H

// Forward declarations
class VBuffer;

// DrawEntry - represents an entry in the draw/render queue (m_queueA4)
// Size: 0x0C (12 bytes)
// No vtable - destructor is non-virtual
// 0x411080 is the compiler-generated scalar deleting destructor (base dtor body inlined)
// Used by ZBufferManager for managing render queue entries
class DrawEntry {
public:
    int priority;                    // 0x00
    VBuffer* m_videoBuffer;          // 0x04 - pointer to video buffer data
    void* m_childObject;             // 0x08 - queued child object with virtual deleting destructor
    void* Destroy(unsigned int flags);
    ~DrawEntry();
};

#endif // DRAWENTRY_H
