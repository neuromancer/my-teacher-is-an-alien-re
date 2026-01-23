#ifndef HANDLER9_H
#define HANDLER9_H

#include "IconBar.h"
#include "SC_Question.h"
#include "SC_Dialog.h"

class SC_Message;
class MouseControl;
class Sprite;
class Queue;

// DialogQuestion is SC_Question (destructor 0x4067e0, Finalize 0x4069b0)
typedef SC_Question DialogQuestion;

// Handler9 - DialogPlayer handler
// Size: 0x650 bytes
// vtable: 0x431170
// Inherits from IconBar (0x600 bytes)
// handlerId: 9 at offset 0x88
//
// Layout:
//   0x00-0x5FF: IconBar base class
//   0x600: MouseControl*
//   0x604: SC_Dialog*
//   0x608: Sprite* (button)
//   0x60C: Sprite* (hilite)
//   0x610: Queue* (dialog queue)
//   0x614: DialogQuestion* (current dialog)
//   0x618-0x63F: int[10] (ambient controllers state)
//   0x640-0x64F: remaining fields
class Handler9 : public IconBar {
public:
    Handler9();
    ~Handler9();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler9-specific fields
    MouseControl* mouseControl;     // 0x600
    SC_Dialog* dialog;           // 0x604
    Sprite* buttonSprite;           // 0x608
    Sprite* hiliteSprite;           // 0x60C
    Queue* dialogQueue;             // 0x610
    DialogQuestion* currentDialog;  // 0x614
    int ambientState[10];           // 0x618-0x63F
    int field_640;                  // 0x640
    int field_644;                  // 0x644
    int field_648;                  // 0x648
    int field_64C;                  // 0x64C
};

#endif // HANDLER9_H
