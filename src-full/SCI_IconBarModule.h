#ifndef SCI_ICONBARMODULE_H
#define SCI_ICONBARMODULE_H

#include "IconBar.h"

// SCI_IconBarModule - Icon bar module handler (case 32/0x20)
// Constructor: 0x401000
// Size: 0x130
// Vtable: 0x461000
// Extends IconBar
// References: "NUM_ACTIONS"
class SCI_IconBarModule : public IconBar {
public:
    SCI_IconBarModule();
    ~SCI_IconBarModule();

    int field_A8[34];   // 0xA8-0x12F (0x88 bytes)
};

#endif // SCI_ICONBARMODULE_H
