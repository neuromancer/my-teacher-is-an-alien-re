#ifndef SC_GAME_H
#define SC_GAME_H

#include "Handler.h"

// SC_Game - Game state handler (case 2)
// Constructor: 0x431D20
// Size: 0xF8
// Vtable: 0x461678
// References: "gamestat.smk", serialization support
class SC_Game : public Handler {
public:
    SC_Game();
    ~SC_Game();

    int field_A8[20];   // 0xA8-0xF7 (0x50 bytes)
};

#endif // SC_GAME_H
