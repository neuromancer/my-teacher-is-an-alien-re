#ifndef SC_PRHOTSPOT_H
#define SC_PRHOTSPOT_H

#include "Parser.h"

class Sprite;
class Sample;
class LinkedList;

// SC_PRHotSpot - Interactive hotspot for PracticeRoom mini-games
// Constructor: 0x429B60
// Destructor: 0x429C10
// Vtable: 0x461508
// Base: Parser (0x90 bytes)
class SC_PRHotSpot : public Parser {
public:
    SC_PRHotSpot(int hotspotId, int owner); // 0x429B60
    ~SC_PRHotSpot();                        // 0x429C10

    void Update();                          // 0x429DF0
    int CheckCollision(void* msg);          // 0x42A010

    int state;            // 0x90 - 0=disabled, 1=idle, 2=hover, 3=clicked
    int boundsLeft;       // 0x94 - left X bound
    int boundsTop;        // 0x98 - top Y bound
    int boundsRight;      // 0x9c - right X bound
    int boundsBottom;     // 0xa0 - bottom Y bound
    int hotspotId;        // 0xa4 - hotspot identifier
    Sprite* sprite;       // 0xa8 - display sprite
    Sample* hoverSound;   // 0xac - sound played on hover enter
    Sample* clickSound;   // 0xb0 - sound played on click
    Sample* exitSound;    // 0xb4 - sound played on hover exit
    int owner;            // 0xb8 - owner pointer from constructor
    LinkedList* actionList; // 0xbc - list of SpriteActions to execute
};

#endif // SC_PRHOTSPOT_H
