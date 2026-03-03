#ifndef SC_ZBUFFER_H
#define SC_ZBUFFER_H

#include "Handler.h"
#include "Timer.h"

// SC_ZBuffer - Z-buffer update handler (case 34/0x22)
// This is the same class referred to as "ZBuffer" in the demo build.
// Error strings confirm original name: "SC_ZBuffer::Update", "SC_ZBuffer::AddMessage"
// Constructor: 0x414220
// Size: 0xC0
// Vtable: 0x4612A8
class SC_ZBuffer : public Handler {
public:
    SC_ZBuffer();
    ~SC_ZBuffer();

    void Init(SC_Message* msg);           // 0x414370
    void Update(int param1, int param2);  // 0x4143A0

    Timer timer;        // 0xA8
    int field_BC;       // 0xBC
};

#endif // SC_ZBUFFER_H
