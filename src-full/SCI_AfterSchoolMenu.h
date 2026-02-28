#ifndef SCI_AFTERSCHOOLMENU_H
#define SCI_AFTERSCHOOLMENU_H

#include "IconBar.h"

class SC_Message;
class Sprite;

// SCI_AfterSchoolMenu - Handler case 31 (0x1F) - Main Menu (full game)
// Size: 0xD0 bytes
// vtable: 0x4612E8
// Constructor: 0x416E70
// Destructor: 0x416FC0
// Extends IconBar (which is 0xA8 in full game = same as Handler)
//
// Layout:
//   0x00-0xA7: IconBar base class (= Handler = Parser + 6 handler fields)
//   0xA8: field_A8
//   0xAC: field_AC
//   0xB0: field_B0
//   0xB4: int field_B4     (pointer, cleaned with FUN_0041DC10)
//   0xB8: Sprite* sprite1  (elements\option2.smk)
//   0xBC: Sprite* sprite2  (elements\option2h.smk)
//   0xC0: field_C0
//   0xC4: int field_C4     (pointer, cleaned with FUN_004148F0)
//   0xC8: int field_C8     (linked list pointer)
//   0xCC: field_CC
class SCI_AfterSchoolMenu : public IconBar {
public:
    SCI_AfterSchoolMenu();
    ~SCI_AfterSchoolMenu();

    int field_A8;       // 0xA8
    int field_AC;       // 0xAC
    int field_B0;       // 0xB0
    int field_B4;       // 0xB4
    Sprite* sprite1;    // 0xB8
    Sprite* sprite2;    // 0xBC
    int field_C0;       // 0xC0
    int field_C4;       // 0xC4
    int field_C8;       // 0xC8
    int field_CC;       // 0xCC
};

#endif // SCI_AFTERSCHOOLMENU_H
