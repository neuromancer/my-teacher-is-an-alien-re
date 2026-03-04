#ifndef SCI_AFTERSCHOOLMENU2_H
#define SCI_AFTERSCHOOLMENU2_H

#include "IconBar.h"

class SC_Message;

// SCI_AfterSchoolMenu2 - During School Menu handler (case 37/0x25)
// Error string: "SCI_SchoolMenu"
// Constructor: 0x41E030
// Destructor: 0x41E0E0
// Size: 0xF8
// Vtable: 0x461388
// Extends IconBar
class SCI_AfterSchoolMenu2 : public IconBar {
public:
    SCI_AfterSchoolMenu2();
    ~SCI_AfterSchoolMenu2();

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    int LBLParse(char* line);

    int IsCharacterActive();
    void SetupOptions();
    void PlayMenuSound();

    // Fields 0xA8-0xF7 (0x50 bytes = 20 ints)
    int field_A8;           // 0xA8 - palette pointer
    int field_AC;           // 0xAC - bg object pointer
    int field_B0;           // 0xB0 - okay sprite pointer
    int field_B4;           // 0xB4 - back/cancel sprite pointer
    int characters[3];      // 0xB8 - character sprites (P, S, D)
    int options[9];         // 0xC4 - activity option sprites
    int field_E8;           // 0xE8 - sound object pointer
    int selectedOption;     // 0xEC - currently selected option (-1 = none)
    int field_F0;           // 0xF0 - priority/message param from BGSND
    int field_F4;           // 0xF4
};

#endif // SCI_AFTERSCHOOLMENU2_H
