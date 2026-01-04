#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "ZBufferManager.h"

struct SoundCommand;

// SoundManager inherits from ZBufferManager
// All methods from 0x41B760 to 0x41Cxxx belong to this class hierarchy
// The global DAT_0043698c is a ZBufferManager* (base class pointer)
class SoundManager : public ZBufferManager {
public:
    // SoundManager adds no additional members - all fields are in ZBufferManager
    // The "m_mode" in ZBufferManager is the same as "m_state" at offset 0x98
    
    void PlayAnimationSound(void* data, int priority, int x, int y, int mode, int scale1, int scale2); // 0x41BE20
    void QueueCommand(SoundCommand* cmd); // 0x41C2C0
    
    // ShowSubtitle is inherited from ZBufferManager at 0x41C000
};

// Note: g_SoundManager is actually the same as DAT_0043698c (ZBufferManager*)
// Both globals point to the same object
extern SoundManager* g_SoundManager;

#endif
