#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Parser.h"
#include "Queue.h"

class MouseControl : public Parser {
public:
    MouseControl();
    ~MouseControl();

private:
    int field_0x88[3];  // Parser ends at 0x88, so from 0x88 to 0x94
    Queue* m_queue;     // 0x94
};

#endif // MOUSECONTROL_H
