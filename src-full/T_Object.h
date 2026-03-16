#ifndef T_OBJECT_H
#define T_OBJECT_H

#include "Parser.h"
#include "Sprite.h"
#include "LinkedList.h"

// T_Object — Inventory item object
// Size: 0xC8, Constructor: 0x40C630, Destructor: 0x40C6E0
// Vtable: 0x461200 (LBLParse override at 0x40CA20)
// Parsed from "mis\\obj0001.mis" with key "[OBJ%5.5d]"
// Error string: "T_Object"
//
// Labels: INVENTORYSPR, DISCRIPTION, ILLEGAL, MESSAGE, END
class T_Object : public Parser {
public:
    T_Object(int itemId);           // 0x40C630
    ~T_Object();                    // 0x40C6E0
    virtual int LBLParse(char*);    // 0x40CA20

    void StopSound();                          // 0x40C830
    void Display(int x, int y, int visible);  // 0x40C840
    void Reset();                              // 0x40C890
    int HandleMsg(int* msg);                   // 0x40C9E0

    // Fields (after Parser 0x90 base, 0x38 bytes = 14 dwords)
    int field_90;           // 0x90 — flags
    int itemId;             // 0x94 — object handle/ID
    int field_98;           // 0x98 — secondary ID or state
    char description[32];   // 0x9C — description text (parsed from DISCRIPTION)
    int field_BC;           // 0xBC
    Sprite* sprite;         // 0xC0 — inventory sprite (parsed from INVENTORYSPR)
    LinkedList* actionList; // 0xC4 — queue of SpriteActions (parsed from MESSAGE)
};

#endif // T_OBJECT_H
