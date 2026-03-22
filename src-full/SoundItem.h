#ifndef SOUNDITEM_H
#define SOUNDITEM_H

#include "Timer.h"
#include "Sample.h"
#include "Memory.h"

// SoundItem class - represents a sound being played
// Size: 0x20 bytes
// All method implementations are demo-only
// Class layout kept for SC_Sound field compatibility
class SoundItem {
public:
    Timer timer;            // 0x04 - 0x17
    int soundId;            // 0x18
    Sample* soundPtr;       // 0x1C

    SoundItem(int sndId);
    ~SoundItem() {
        if (soundPtr != 0) {
            soundPtr->Unload();
            operator delete(soundPtr);
            soundPtr = 0;
        }
    }

    // Demo-only methods - kept as declarations for SC_Sound demo code
    void Start();
    void Resume();
    void AdjustVolume(int delta);
    void SetVolume(int volume);
};

#endif // SOUNDITEM_H
