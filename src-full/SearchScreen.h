#ifndef SEARCHSCREEN_H
#define SEARCHSCREEN_H

#include "Handler.h"
#include "Timer.h"
#include "Sprite.h"
#include "GlyphRect.h"

// SearchScreen - Command 5 Handler (Game State Handler)
// Size: 0xF0
// vtable: 0x4312e8
// Layout:
//   0x00-0x9F: Handler base class
//   0xA0-0xB3: Timer (size 0x14)
//   0xB4: Sprite* (gamestat.smk)
//   0xB8-0xEC: various fields/rectangles
class SearchScreen : public Handler {
public:
    SearchScreen();
    ~SearchScreen();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // SearchScreen-specific fields (handlerId through field_9C are in Handler base)
    Timer timer;        // 0xA0 - 0xB3 (size 0x14)

    Sprite* sprite;     // 0xB4

    // Scroll / selection state
    int scrollOffset;   // 0xB8 - first visible row index
    int selectedRow;    // 0xBC - currently selected row index

    // Up scroll rectangle (left, top, right, bottom)
    GlyphRect upScroll;   // 0xC0-0xCF

    // Down scroll rectangle (left, top, right, bottom)
    GlyphRect downScroll; // 0xD0-0xDF

    // Text layout
    int textX;          // 0xE0 - x position for state labels
    int textY;          // 0xE4 - y offset for first row
    int rowHeight;      // 0xE8 - height of each row
    int visibleRows;    // 0xEC - number of visible rows (14 = 0xe)
};

#endif // SEARCHSCREEN_H
