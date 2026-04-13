#ifndef SCI_SCHOOLMENU_H
#define SCI_SCHOOLMENU_H

#include "IconBar.h"

class SC_MessageParser;
class Palette;
class MMPlayer;
class Sample;
class T_MenuHotspot;

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

    void Init(SC_MessageParser* msg);
    int AddMessage(SC_MessageParser* msg);
    int ShutDown(SC_MessageParser* msg);
    void Update(int param1, int param2);
    int Exit(SC_MessageParser* msg);
    int LBLParse(char* line);
    void OnProcessStart();

    int IsCharacterActive();
    void SetupOptions();
    void PlayMenuSound();

    // Fields 0xA8-0xF7 (0x50 bytes)
    Palette* palette;       // 0xA8 - background palette
    MMPlayer* background;   // 0xAC - background animation
    T_MenuHotspot* okayButton;    // 0xB0 - "OKAY" confirm button
    T_MenuHotspot* cancelButton;  // 0xB4 - "CANCEL" back button
    T_MenuHotspot* characters[3]; // 0xB8 - character buttons [P=Peter, S=Susan, D=Duncan]
    T_MenuHotspot* options[9];    // 0xC4 - activity option buttons [rooms 1-9]
    Sample* menuSound;      // 0xE8 - menu click sound
    int selectedOption;     // 0xEC - currently selected option index (-1 = none)
    int bgSoundId;          // 0xF0 - background sound handle (from BGSND label)
    int field_F4;           // 0xF4
};

#endif // SCI_SCHOOLMENU_H
