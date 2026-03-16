#ifndef SCI_SCHOOLMENU_H
#define SCI_SCHOOLMENU_H

#include "IconBar.h"

class SC_Message;

// SCI_SchoolMenu - During School Menu handler (case 37/0x25)
// Error string: "SCI_SchoolMenu"
// Constructor: 0x41E030
// Destructor: 0x41E0E0
// Size: 0xF8
// Vtable: 0x461388
// Extends IconBar
class SCI_SchoolMenu : public IconBar {
public:
    SCI_SchoolMenu();
    ~SCI_SchoolMenu();

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    int LBLParse(char* line);

    int IsCharacterActive();
    void SetupOptions();
    void PlayMenuSound();

    // Fields 0xA8-0xF7 (0x50 bytes = 20 ints)
    int palette;            // 0xA8 - Palette* for background palette
    int background;         // 0xAC - MMPlayer* background animation
    int okayButton;         // 0xB0 - SprInit* (0x1A8) "OKAY" confirm button
    int cancelButton;       // 0xB4 - SprInit* (0x1A8) "CANCEL" back button
    int characters[3];      // 0xB8 - SprInit* character buttons [P=Peter, S=Susan, D=Duncan]
    int options[9];         // 0xC4 - SprInit* activity option buttons [rooms 1-9]
    int menuSound;          // 0xE8 - Sample* menu click sound
    int selectedOption;     // 0xEC - currently selected option index (-1 = none)
    int bgSoundId;          // 0xF0 - background sound handle (from BGSND label)
    int field_F4;           // 0xF4
};

#endif // SCI_SCHOOLMENU_H
