#include "DrawEntry.h"
#include "SoundCommand.h"
#include "Memory.h"

// Forward declaration for video buffer cleanup
struct VideoBufferData;
void __fastcall ReleaseVideoBuffer(VideoBufferData* data);

DrawEntry::~DrawEntry() {
    // Destructor called via vtable[0]
}

/* Function start: 0x411080 */
void* DrawEntry::Cleanup(int freeFlag) {
    VideoBufferData* videoBuffer;
    SoundCommand* childObject;
    
    // Release video buffer at offset 0x04
    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        ReleaseVideoBuffer(videoBuffer);
        FreeMemory(videoBuffer);
        DrawEntry::m_videoBuffer = 0;
    }
    
    // Destroy child object at offset 0x08 via delete (calls virtual destructor)
    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete childObject;
        DrawEntry::m_childObject = 0;
    }
    
    // Optionally free self
    if ((freeFlag & 1) != 0) {
        FreeMemory(this);
    }
    
    return this;
}

