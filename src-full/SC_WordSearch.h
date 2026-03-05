#ifndef SC_WORDSEARCH_H
#define SC_WORDSEARCH_H

#include "Handler.h"

// SC_WordSearch - Word search puzzle handler (case 81/0x51)
// Constructor: 0x435800
// Size: 0x838
// Vtable: 0x461730
// References: "PUZ_WAHOO_WON", grid-based word game
// Largest handler in the game
// TypedArray at +0x4D4 (size=0x10, count=0x20)
class SC_WordSearch : public Handler {
public:
    SC_WordSearch();
    ~SC_WordSearch();

    // Virtual overrides
    virtual void Init(SC_Message* msg);       // 0x435BE0 (+0x10)
    virtual int AddMessage(SC_Message* msg);  // 0x436490 (+0x14)
    virtual int ShutDown(SC_Message* msg);    // 0x435D40 (+0x18)
    virtual void Update(int param1, int param2); // 0x435F10 (+0x1C)
    virtual int Exit(SC_Message* msg);        // 0x437080 (+0x20)

    virtual int LBLParse(char* param_1);      // 0x4370F0 (+0x00)

    void FUN_436790();                        // 0x436790
    void FUN_4368F0();                        // 0x4368F0

    int field_A8[484];  // 0xA8-0x837 (0x790 bytes)
};

#endif // SC_WORDSEARCH_H
