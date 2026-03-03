#ifndef SC_GAME_H
#define SC_GAME_H

#include "Handler.h"
#include "Timer.h"
#include "Sprite.h"
#include "GlyphRect.h"

// SC_Game - Game state handler (case 2)
// Full game version of SearchScreen (demo)
// Constructor: 0x431D20
// Destructor body: 0x431F30
// Size: 0xF8
// Vtable: 0x461678
class SC_Game : public Handler {
public:
    SC_Game();
    ~SC_Game();

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    void Update(int param1, int param2);

    Timer timer;            // 0xA8 (size 0x14)
    Sprite* sprite;         // 0xBC
    int scrollOffset;       // 0xC0
    int selectedRow;        // 0xC4
    GlyphRect upScroll;     // 0xC8 (size 0x10)
    GlyphRect downScroll;   // 0xD8 (size 0x10)
    int textX;              // 0xE8
    int textY;              // 0xEC
    int rowHeight;          // 0xF0
    int field_F4;           // 0xF4
};

#endif // SC_GAME_H
