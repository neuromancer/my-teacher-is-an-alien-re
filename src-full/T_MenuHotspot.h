#ifndef T_MENUHOTSPOT_H
#define T_MENUHOTSPOT_H

#include "Parser.h"
#include "Sprite.h"

// T_MenuHotspot - Menu button hotspot (extends Parser)
// Constructor: 0x421A50
// Size: 0xA8
// Vtable: 0x4613D0
class T_MenuHotspot : public Parser {
public:
    T_MenuHotspot(char* name, int* rect);
    ~T_MenuHotspot() {
        if (sprite != 0) {
            delete sprite;
            sprite = 0;
        }
    }
    virtual int LBLParse(char* param);
    void Update();
    void CleanupMessages();

    int field_88;       // 0x88
    int field_8C;       // 0x8C
    Sprite* sprite;     // 0x90
    int rect_left;      // 0x94
    int rect_top;       // 0x98
    int rect_right;     // 0x9C
    int rect_bottom;    // 0xA0
    int field_A4;       // 0xA4
};

#endif // T_MENUHOTSPOT_H
