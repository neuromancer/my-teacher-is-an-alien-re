#ifndef SCI_PRACTICEROOM_H
#define SCI_PRACTICEROOM_H

#include "Handler.h"

// SCI_PracticeRoom - Practice room handler (case 43/0x2B)
// Constructor: 0x42A8D0
// Size: 0xD0
// Vtable: 0x461520
// References: "IN_PRACTICEROOM", "SCI_PracticeRoom"
class SCI_PracticeRoom : public Handler {
public:
    SCI_PracticeRoom();
    ~SCI_PracticeRoom();

    int field_A8[10];   // 0xA8-0xCF (0x28 bytes)
};

#endif // SCI_PRACTICEROOM_H
