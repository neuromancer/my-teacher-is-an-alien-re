#ifndef ONSCREENMESSAGE_H
#define ONSCREENMESSAGE_H

#include "Timer.h"

// OnScreenMessage structure
// Size: 0x118 bytes
// This is a single message entry used by SC_OnScreenMessage
struct OnScreenMessage {
    char text[0x100];           // 0x00 - text buffer (256 bytes)
    Timer timer;                // 0x100 - Timer object (20 bytes)
    unsigned int duration;      // 0x114 - display duration in ms

    void* Init(char* message, unsigned int duration);
    int Update(int index);
};

#endif // ONSCREENMESSAGE_H
