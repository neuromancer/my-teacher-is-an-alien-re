#ifndef ZBUFFER_H
#define ZBUFFER_H

#include "Timer.h"
#include "Handler.h"

#include "LinkedList.h"

class Sprite;
class Projectile;

// ZBuffer extends Handler in the full game.
// Demo used "ZBuffer" as a handler class but the full game uses SC_ZBuffer instead.
// This class is kept for utility methods (CleanUpVBuffer, ResetItems, Exit)
// and field layout compatibility.
class ZBuffer : public Handler {
public:
    ~ZBuffer(); // 0x427710

    Timer timer;        // 0xA8
    int itemCount;         // 0xB0
    Projectile** items;    // 0xB4

    int Exit(SC_Message* msg);  // 0x44B5A0
    void CleanUpVBuffer();      // 0x401C80
    void ResetItems();          // 0x4279A0
};

#endif // ZBUFFER_H
