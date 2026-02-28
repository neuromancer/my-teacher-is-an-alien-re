#ifndef SCI_SCHEDULE_H
#define SCI_SCHEDULE_H

#include "IconBar.h"

// SCI_Schedule - School schedule/map screen (case 39/0x27)
// Constructor: 0x434C10
// Size: 0xD8
// Vtable: 0x4616F8
// Extends IconBar
// References: "schedule.mis", "schedule.col", "MapScene"
class SCI_Schedule : public IconBar {
public:
    SCI_Schedule();
    ~SCI_Schedule();

    int field_A8[12];   // 0xA8-0xD7 (0x30 bytes)
};

#endif // SCI_SCHEDULE_H
