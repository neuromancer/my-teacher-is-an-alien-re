#ifndef ICONBAR_H
#define ICONBAR_H

#include "Handler.h"

class Sprite;
class SoundList;

// IconBarButton - Individual button on the icon bar
// Size: 0xE0 bytes
struct IconBarButton {
    Sprite* sprite;     // 0x00 - SMK animation sprite
    char pad_04[0xC4];  // 0x04-0xC7 padding/unused
    int x1;             // 0xC8 - left bound
    int y1;             // 0xCC - top bound
    int x2;             // 0xD0 - right bound
    int y2;             // 0xD4 - bottom bound
    int enabled;        // 0xD8 - button is enabled/active
    char pad_DC[4];     // 0xDC-0xDF padding
};

// IconBar - Base class for handlers with icon bar UI
// Size: 0x600 bytes
// vtable: 0x431088
// Inherits from Handler
// Layout:
//   0x00-0x9F: Handler base class (Parser + Handler fields)
//   0xA0-0xAF: Icon bar rect bounds
//   0xB0: Sprite* iconbarSprite (elements\iconbar.smk)
//   0xB4: int field_B4
//   0xB8: IconBarButton buttons[6] (0xE0 * 6 = 0x540)
//   0x5F8: SoundList* soundList
//   0x5FC: int field_5FC (unused/padding)
class IconBar : public Handler {
public:
    IconBar();
    virtual ~IconBar();

    // Methods
    void InitIconBar(SC_Message* msg);
    void CleanupIconBar();
    int CheckButtonClick(SC_Message* msg);
    void DrawIconBar(int param1, int param2);

    // Icon bar rect (0xA0-0xAF)
    int barX1;          // 0xA0
    int barY1;          // 0xA4
    int barX2;          // 0xA8
    int barY2;          // 0xAC

    // Icon bar sprite (0xB0)
    Sprite* iconbarSprite;  // 0xB0
    int field_B4;           // 0xB4

    // Button array (0xB8 - 0x5F7, size 0x540 = 6 * 0xE0)
    IconBarButton buttons[6];

    // Sound list (0x5F8)
    SoundList* soundList;   // 0x5F8
    int field_5FC;          // 0x5FC
};

#endif // ICONBAR_H
