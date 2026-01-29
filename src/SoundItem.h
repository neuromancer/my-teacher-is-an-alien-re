#ifndef SOUNDITEM_H
#define SOUNDITEM_H

#include "Timer.h"
#include "Sample.h"
#include "Memory.h"

// SoundItem class - represents a sound being played
// Size: 0x20 bytes
// vtable: 0x431238
// Layout:
//   0x00: vtable pointer (implicit, from virtual destructor)
//   0x04-0x17: Timer (size 0x14)
//   0x18: soundId
//   0x1C: Sample* soundPtr
class SoundItem {
public:
    Timer timer;            // 0x04 - 0x17
    int soundId;            // 0x18
    Sample* soundPtr;       // 0x1C

    SoundItem(int sndId);           // 0x40B5D0
    virtual ~SoundItem() {
        if (soundPtr != 0) {
            soundPtr->Unload();
            FreeMemory(soundPtr);
            soundPtr = 0;
        }
    }
    int IsFinished();               // 0x40B700
    void Resume();                  // 0x40B750 - Play without loop
    void Start();                   // 0x40B770 - Play with loop
    void AdjustVolume(int delta);   // 0x40B790
    void SetVolume(int volume);     // 0x40B7C0
};

#endif // SOUNDITEM_H
