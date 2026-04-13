#ifndef SCI_DIALOG_H
#define SCI_DIALOG_H

#include "IconBar.h"
#include "SC_Question.h"

class SC_MessageParser;
class Sprite;
class Palette;

// DialogQuestion is SC_Question (destructor 0x4067e0, Finalize 0x4069b0)
typedef SC_Question DialogQuestion;

// SCI_Dialog - Dialog handler layout for the full game
// In the full game, Handler31 (handler 0x1F) is the actual class.
// SCI_Dialog exists as a type alias for Handler31's layout, used by
// SC_Question (which stores a SCI_Dialog* at 0x120) and by
// GetDialogByIndex/FindDialogById methods.
//
// Layout must match Handler31 (extends IconBar = 0xA8 in full game):
//   0xA8: field_A8
//   0xAC: field_AC
//   0xB0: maxQuestions
//   0xB4: Palette*
//   0xB8: Sprite* (button)
//   0xBC: Sprite* (hilite)
//   0xC0: SC_Question* currentQuestion
//   0xC4: SC_Question* (secondary question)
//   0xC8: LinkedList* dialogQueue
//   0xCC: field_CC
class SCI_Dialog : public IconBar {
public:
    // Full game methods
    DialogQuestion* GetDialogByIndex(int index);  // 0x417C80
    DialogQuestion* FindDialogById(int id);       // 0x417E60

    // Fields (matching Handler31 layout)
    int field_A8;                   // 0xA8
    int field_AC;                   // 0xAC
    int maxQuestions;                // 0xB0
    Palette* field_B4;              // 0xB4
    Sprite* field_B8;               // 0xB8 — button sprite
    Sprite* field_BC;               // 0xBC — hilite sprite
    DialogQuestion* currentQuestion; // 0xC0 — current question
    DialogQuestion* field_C4;       // 0xC4 — secondary question
    Queue* dialogQueue;             // 0xC8 — dialog queue (was field_610 in demo)
    int field_CC;                   // 0xCC — set by SC_Question
};

#endif
