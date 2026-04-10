#ifndef ICONBAR_H
#define ICONBAR_H

#include "Handler.h"
#include "SC_Question.h"
#include "GlyphRect.h"

class Sprite;
class SoundList;
class Sample;

// Simple rect struct for icon bar bounds (no destructor)
struct Rect {
    int x1;
    int y1;
    int x2;
    int y2;
    Rect() { x1 = 0; y1 = 0; x2 = 0; y2 = 0; }
};

// IconBarButton - Individual button on the icon bar
// Size: 0xE0 bytes
// Contains embedded SC_Message at offset 0x08 for message template
class IconBarButton {
public:
    IconBarButton();   // 0x402CD0
    ~IconBarButton();  // 0x402D60

    Sprite* sprite;         // 0x00 - SMK animation sprite
    int field_04;           // 0x04
    SC_Message message;     // 0x08 - embedded SC_Message (0xC0 bytes)
    GlyphRect bounds;       // 0xC8 - button bounds (uses GlyphRect for proper destructor)
    int enabled;            // 0xD8 - button is enabled/active
    Sample* clickSound;     // 0xDC - click sound sample
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

    // Virtual method overrides (from Handler)
    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    int Exit(SC_Message* msg);

    // Non-virtual methods
    void PlayButtonSound(int buttonIndex);  // 0x403300

    // Icon bar rect (0xA0-0xAF)
    GlyphRect barBounds;     // 0xA0 - icon bar bounds

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
