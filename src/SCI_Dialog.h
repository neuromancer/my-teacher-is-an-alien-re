#ifndef SCI_DIALOG_H
#define SCI_DIALOG_H

#include "IconBar.h"
#include "SC_Question.h"

class SC_Message;
class MMPlayer2;
class MouseControl;
class Sprite;
class Queue;

// DialogQuestion is SC_Question (destructor 0x4067e0, Finalize 0x4069b0)
typedef SC_Question DialogQuestion;

// SCI_Dialog - DialogPlayer handler
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
class SCI_Dialog : public IconBar {
public:
    SCI_Dialog();
    ~SCI_Dialog();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual int LBLParse(char* line);

    // SCI_Dialog-specific methods
    DialogQuestion* GetDialogByIndex(int index);
    DialogQuestion* FindDialogById(int id);

    // Fields
    MouseControl* field_600;
    MMPlayer2* field_604;
    Sprite* field_608;
    Sprite* field_60C;
    Queue* field_610;
    DialogQuestion* field_614;
    int field_618[10];
    int field_640;
    int field_644;
    int field_648;
    int field_64C;
};

#endif
