#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Queue.h"

class MouseControl {
public:
    MouseControl();
    ~MouseControl();

private:
    void** vtable;
    int field_0x4[33];
    Queue* m_queue;
};

#endif // MOUSECONTROL_H
