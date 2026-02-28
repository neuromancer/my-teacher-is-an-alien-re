#ifndef SCI_AFTERSCHOOLMENU2_H
#define SCI_AFTERSCHOOLMENU2_H

#include "IconBar.h"

// SCI_AfterSchoolMenu2 - After school menu variant (case 37/0x25)
// Constructor: 0x41E030
// Size: 0xF8
// Vtable: 0x461388
// Extends IconBar
class SCI_AfterSchoolMenu2 : public IconBar {
public:
    SCI_AfterSchoolMenu2();
    ~SCI_AfterSchoolMenu2();

    int field_A8[20];   // 0xA8-0xF7 (0x50 bytes)
};

#endif // SCI_AFTERSCHOOLMENU2_H
