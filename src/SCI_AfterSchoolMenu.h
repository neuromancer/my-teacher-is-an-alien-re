#ifndef SCI_AFTERSCHOOLMENU_H
#define SCI_AFTERSCHOOLMENU_H

#include "IconBar.h"
#include "Hotspot.h"
#include "InputManager.h"

class SC_Message;
class Palette;
class Sprite;
class Sample;
class MMPlayer;
class Hotspot;
class OptionMenu;

// SoundSlot - structure for character voice samples
// Size: 8 bytes
struct SoundSlot {
    int enabled;        // +0x00
    Sample* sample;     // +0x04
};

// SCI_AfterSchoolMenu - Command 10 Handler (After School Menu / Demo Choice Screen)
// Size: 0x6A8 bytes
// vtable: 0x431110
// Inherits from IconBar (0x600 bytes)
//
// This handler displays the main menu after the intro, allowing the
// player to select a character (Peter, Susan, Duncan) and start gameplay.
//
// Layout:
//   0x000-0x5FF: IconBar base class
//   0x600: Palette* palette
//   0x604: MouseControl* background
//   0x608: void* choiceScreen (OptionMenu)
//   0x60C: void* goButton (CharButton)
//   0x610: void* cancelButton (CharButton)
//   0x614: void* characters[3] (CharSprite array - peter, susan, duncan)
//   0x620: SoundSlot sounds[8] (stride 8, total 0x40 bytes)
//   0x660: Sample* currentSound
//   0x664: int savedCharacterIndex
//   0x668: int savedSubmenuIndex
//   0x66C: int currentCharacterIndex
//   0x670: int currentSubmenuIndex
//   0x674: int hoverCharacterIndex
//   0x678: int hoverSubmenuIndex
//   0x67C: int needsRefresh
//   0x680: int isInitialized
//   0x684: int prevHoverCharacter
//   0x688: int prevCharacter
//   0x68C: int prevSubmenuHover
//   0x690: int prevSubmenu
//   0x694: int confirmFlag
//   0x698: int updateFlag
//   0x69C: int characterCount
//   0x6A0: Sample* ambientSound
//   0x6A4: int playSoundsFlag
class SCI_AfterSchoolMenu : public IconBar {
public:
    SCI_AfterSchoolMenu();
    ~SCI_AfterSchoolMenu();

    // Virtual method overrides
    virtual int LBLParse(char* line);
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // SCI_AfterSchoolMenu methods
    void ResetHoverState();
    int IsSelectionComplete();
    void ResetSelection();
    void PlayCharacterSound(int soundIndex);
    void RenderGoButton();
    void RenderCharacters();
    void RenderChoiceScreen(int characterIndex);
    void ProcessCharacterHover(MousePoint pt);
    void ProcessSubmenuHover(MousePoint pt);
    void ProcessGoButtonHover(MousePoint pt, T_Hotspot* goButton, int* confirmFlag);
    void DisplaySubmenuHover(int mouseX, int mouseY);

    void SetCharacterOption(int characterIndex);
    void SetSubmenuOption(int submenuIndex, int state);
    void PlaySoundsIfNeeded();
    void FillOptionQueue(); // Replaces SetCharacterOptionInternal based on original name

    // SCI_AfterSchoolMenu-specific fields starting at 0x600
    Palette* palette;           // 0x600
    MMPlayer* background;   // 0x604
    OptionMenu* choiceScreen;   // 0x608
    T_Hotspot* goButton;          // 0x60C - CharButton*
    T_Hotspot* cancelButton;      // 0x610 - CharButton*
    T_Hotspot* characters[3];     // 0x614 - CharSprite* array (peter, susan, duncan)
    SoundSlot sounds[8];        // 0x620 - 0x660 (8 * 8 = 0x40 bytes)
    Sample* currentSound;       // 0x660
    int savedCharacterIndex;    // 0x664
    int savedSubmenuIndex;      // 0x668
    int currentCharacterIndex;  // 0x66C
    int currentSubmenuIndex;    // 0x670
    int hoverCharacterIndex;    // 0x674
    int hoverSubmenuIndex;      // 0x678
    int needsRefresh;           // 0x67C
    int isInitialized;          // 0x680
    int prevHoverCharacter;     // 0x684
    int prevCharacter;          // 0x688
    int prevSubmenuHover;       // 0x68C
    int prevSubmenu;            // 0x690
    int confirmFlag;            // 0x694
    int updateFlag;             // 0x698
    int characterCount;         // 0x69C
    Sample* ambientSound;       // 0x6A0
    int playSoundsFlag;         // 0x6A4
};

// Global character objects created in SCI_AfterSchoolMenu constructor
// DAT_00435a74: peter character
// DAT_00435a78: susan character
// DAT_00435a7c: duncan character
// DAT_00435a80: currently selected character

#endif // SCI_AFTERSCHOOLMENU_H
