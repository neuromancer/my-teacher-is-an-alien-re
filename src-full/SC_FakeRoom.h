#ifndef SC_FAKEROOM_H
#define SC_FAKEROOM_H

#include "SC_Combat.h"

// SC_FakeRoom - Fake room handler (case 73/0x49)
// Constructor: 0x4441E0
// Size: 0x130
// Vtable: 0x461988
// Extends SC_Combat (0x118)
// References: "SC_FakeRoom::Process_Action"
class SC_FakeRoom : public SC_Combat {
public:
    SC_FakeRoom();
    ~SC_FakeRoom();

    int field_118[6];   // 0x118-0x12F (0x18 bytes)
};

#endif // SC_FAKEROOM_H
