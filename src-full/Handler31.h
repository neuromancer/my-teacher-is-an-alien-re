#ifndef HANDLER31_H
#define HANDLER31_H

#include "IconBar.h"

class SC_Message;
class Sprite;
class Palette;
class SC_Question;
struct LinkedList;
struct Queue;

// Handler31 - Handler case 31 (0x1F)
// Size: 0xD0 bytes
// vtable: 0x4612E8
// Constructor: 0x416E70
// Destructor: 0x416FC0
// Extends IconBar (which is 0xA8 in full game = same as Handler)
//
// Layout:
//   0x00-0xA7: IconBar base class (= Handler = Parser + 6 handler fields)
//   0xA8: field_A8
//   0xAC: field_AC
//   0xB0: field_B0
//   0xB4: Palette* palette
//   0xB8: Sprite* optionSprite   (elements\option2.smk)
//   0xBC: Sprite* optionHiSprite (elements\option2h.smk)
//   0xC0: SC_Question* activeQuestion
//   0xC4: SC_Question* placeholder
//   0xC8: Queue* questionQueue
//   0xCC: int resetSprites
class Handler31 : public IconBar {
public:
    Handler31();
    ~Handler31();

    virtual int LBLParse(char* line);            // 0x418060
    virtual int AddMessage(SC_Message* msg);    // 0x417750
    virtual int ShutDown(SC_Message* msg);     // 0x4173D0
    virtual void Update(int param1, int param2); // 0x417500
    int CheckDuplicateQuestion(int param);     // 0x417D50

    int field_A8;           // 0xA8
    int field_AC;           // 0xAC
    int field_B0;           // 0xB0
    Palette* palette;       // 0xB4 - PALETTE
    Sprite* optionSprite;   // 0xB8 - elements\option2.smk
    Sprite* optionHiSprite; // 0xBC - elements\option2h.smk (highlight)
    SC_Question* activeQuestion;  // 0xC0 - currently active question
    SC_Question* placeholder;     // 0xC4 - PLACEHOLDER question
    Queue* questionQueue;         // 0xC8 - queue of SC_Question*
    int resetSprites;       // 0xCC - flag to reset sprite states
};

#endif // HANDLER31_H
