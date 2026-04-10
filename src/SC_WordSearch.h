#ifndef SC_WORDSEARCH_H
#define SC_WORDSEARCH_H

#include "Handler.h"
#include "Timer.h"

class Palette;
class Sprite;
class Sample;
class SpriteAction;

// SC_WordSearch - Word search puzzle handler (case 81/0x51)
// Constructor: 0x435800
// Size: 0x838
// Vtable: 0x461730
// References: "PUZ_WAHOO_WON", grid-based word game
// Largest handler in the game
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
    virtual void OnProcessEnd();             // 0x42F800

    void Render();                           // 0x42EFC0
    void PlaceWord(int row, int col);        // 0x42F220
    void DispatchResult();                   // 0x436790
    void InitWordList();                     // 0x4368F0

    // Resource pointers (0xA8-0xBF)
    Palette* palette;          // 0xA8
    Sprite* bgSprite;          // 0xAC
    Sample* correctSound;      // 0xB0
    Sample* incorrectSound;    // 0xB4
    Sample* startSound;        // 0xB8
    Sample* introSound;        // 0xBC

    // Inline timer (0xC0-0xD3, 20 bytes)
    Timer timer;               // 0xC0

    // Mixed-use area: word list during init, then grid cells/cursor/game data
    // Words: 32 slots × 32 chars at 0xD4-0x4D3
    // Grid cells, cursor hotspot, exit bounds, sprites etc. interspersed
    char gameData[0x748];      // 0xD4-0x81B

    // End fields (0x81C-0x837)
    int gameFlags;             // 0x81C - bit flags (bit 0 = PUZ_WAHOO_WON check)
    Sprite* puzzleSprite1;     // 0x820
    Sprite* puzzleSprite2;     // 0x824
    Sprite* timerSprite;       // 0x828
    Sprite* scoreSprite;       // 0x82C
    SpriteAction* resultAction;// 0x830
    int field_834;             // 0x834
};

#endif // SC_WORDSEARCH_H
