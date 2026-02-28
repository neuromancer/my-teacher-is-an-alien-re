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

    int field_A8[484];  // 0xA8-0x837 (0x790 bytes)
};

#endif // SC_WORDSEARCH_H
