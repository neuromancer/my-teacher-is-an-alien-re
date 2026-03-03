#ifndef ZBUFFER_H
#define ZBUFFER_H

#include "Timer.h"
#include "Handler.h"

class Message;
#include "LinkedList.h"

// ZBuffer extends Handler in the full game (same layout as SC_ZBuffer).
// Demo code used "ZBuffer" but error strings confirm the real name is "SC_ZBuffer".
// Handler base provides: handlerId (0x90), moduleParam (0x94), etc.
class ZBuffer : public Handler {
public:
    Timer timer; // 0xA8

    int Exit(SC_Message* msg);
    void Update(int, int);
    int AddMessage(int);
    int ProcessMessage(Message* msg);
    void CleanUpVBuffer();
};

#endif // ZBUFFER_H
