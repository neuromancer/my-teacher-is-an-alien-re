#ifndef ONSCREENMESSAGE_H
#define ONSCREENMESSAGE_H

#include "Timer.h"
#include "Sample.h"
#include "Memory.h"

// OnScreenMessage structure (DEMO version)
// Size: 0x118 bytes
// Constructor: 0x40A1D0
// This is a single message entry used by SC_OnScreenMessage (demo)
struct OnScreenMessage {
    char text[0x100];           // 0x00 - text buffer (256 bytes)
    Timer timer;                // 0x100 - Timer object (20 bytes)
    unsigned int duration;      // 0x114 - display duration in ms

    OnScreenMessage(char* message, unsigned int dur);
    int Update(int index);
};

// SoundEntry - Full game sound list entry for SC_OnScreenMessage
// Size: 0x30 bytes (vptr at 0x00, wiped by the ctor's memset — original quirk)
// Constructor: 0x447FF0
// Vtable: 0x461A58 (1 entry: LogId at 0x448120 — writes "hIam %d" to message log)
// Methods: Update(0x448140), FadeVolume(0x448220)
struct SoundEntry {
    // vptr occupies 0x00 (stored as 0x461a58 by ctor/dtor, zeroed by ctor memset)
    int activeFlags;            // 0x04 - flags (byte at +0x04 bit 0 used)
    int targetVolume;           // 0x08
    int volumeStep;             // 0x0C
    Timer timer;                // 0x10 - Timer object (size 0x14)
    int soundId;                // 0x24
    Sample* sample;             // 0x28
    int gameStateVal;           // 0x2C

    SoundEntry(int id);

    ~SoundEntry() {
        if (sample != 0) {
            delete sample;
            sample = 0;
        }
    }

    virtual void LogId();       // 0x448120 - only vtable entry

    int SoundUpdate();
    void FadeVolume(int volume, unsigned int duration);
};

#endif // ONSCREENMESSAGE_H
