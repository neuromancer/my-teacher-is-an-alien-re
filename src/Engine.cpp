#include "DrawEntry.h"
#include "Animation.h"
#include "VBuffer.h"

// DrawEntry implementation. This file used to carry a stale Engine model for
// the same original vtable as SC_Combat; keep only the unrelated destructor.
DrawEntry::~DrawEntry() {
    VBuffer* videoBuffer;
    void* childObject;

    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        delete videoBuffer;
        DrawEntry::m_videoBuffer = 0;
    }

    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete (Animation*)childObject;
        DrawEntry::m_childObject = 0;
    }
}

/* Function start: 0x431BE0 */
void* DrawEntry::Destroy(unsigned int flags) {
    VBuffer* videoBuffer;
    void* childObject;

    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        delete videoBuffer;
        DrawEntry::m_videoBuffer = 0;
    }

    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete (Animation*)childObject;
        DrawEntry::m_childObject = 0;
    }

    if ((flags & 1) != 0) {
        operator delete(this);
    }
    return this;
}
