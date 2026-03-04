#ifndef SC_BGSND_H
#define SC_BGSND_H

#include "Handler.h"
#include "Timer.h"

class Sample;

// SC_BgSnd - Background sound handler (case 5)
// Constructor: 0x4392E0
// Size: 0xD0
// Vtable: 0x4617A8
// References: "GS_MUSICOFF", "SC_BgSnd::AddMessage"
class SC_BgSnd : public Handler {
public:
    SC_BgSnd();
    ~SC_BgSnd();

    void OnProcessEnd();                      // 0x439690
    void AddMessage(int soundHandle);         // 0x439740
    void SetVolume(int volume, int duration); // 0x4398C0

    int flags;          // 0xA8 - bit 0 = volume transition active
    int targetVol;      // 0xAC
    int volStep;        // 0xB0
    Timer timer;        // 0xB4 (20 bytes, 0xB4-0xC7)
    Sample* snd;        // 0xC8
    int sndId;          // 0xCC
};

#endif // SC_BGSND_H
