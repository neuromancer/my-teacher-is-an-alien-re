#ifndef SC_PRHOTSPOT_H
#define SC_PRHOTSPOT_H

#include "Parser.h"

// SC_PRHotSpot - Interactive hotspot for PracticeRoom mini-games
// Constructor: 0x429B60
// Vtable: 0x461508
// Base: Parser (0x90 bytes)
class SC_PRHotSpot : public Parser {
public:
    SC_PRHotSpot(int param_1, int param_2); // 0x429B60
    ~SC_PRHotSpot(); // 0x429C10

    void Update();                  // 0x429DF0
    int CheckCollision(void* msg);  // 0x42A010

    int field_90;     // 0x90 - state (1=active, 2=hover, 3=hit)
    int field_94;     // 0x94 - min X
    int field_98;     // 0x98 - min Y
    int field_9c;     // 0x9c - max X
    int field_a0;     // 0xa0 - max Y
    int field_a4;     // 0xa4 - damage/power value
    int field_a8;     // 0xa8 - sprite pointer (main)
    int field_ac;     // 0xac - sprite pointer (hover sound)
    int field_b0;     // 0xb0 - sprite pointer (sound)
    int field_b4;     // 0xb4 - sprite pointer (hit sound)
    int field_b8;     // 0xb8 - param from constructor
    int field_bc;     // 0xbc - linked list pointer
};

#endif // SC_PRHOTSPOT_H
