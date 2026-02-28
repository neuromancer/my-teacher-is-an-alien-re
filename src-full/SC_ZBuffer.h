#ifndef SC_ZBUFFER_H
#define SC_ZBUFFER_H

#include "Handler.h"
#include "Timer.h"

// SC_ZBuffer - Z-buffer update handler (case 34/0x22)
// Constructor: 0x414220
// Size: 0xC0
// Vtable: 0x4612A8
// References: "SC_ZBuffer::Update"
class SC_ZBuffer : public Handler {
public:
    SC_ZBuffer();
    ~SC_ZBuffer();

    void Update(int param1, int param2);  // 0x4143A0

    Timer timer;        // 0xA8
    int field_BC;       // 0xBC
};

#endif // SC_ZBUFFER_H
