#ifndef SCI_SEARCHSCREEN_H
#define SCI_SEARCHSCREEN_H

#include "IconBar.h"
#include "Palette.h"
#include "MouseControl.h"
#include "SC_Dialog.h"
#include "Hotspot.h"
#include "Sample.h"

class SC_Message;

// SCI_SearchScreen - Script/Scene handler
// Size: 0x648 bytes
// vtable: 0x431210
// Inherits from IconBar (0x600 bytes)
// handlerId: 11 (0xb) at offset 0x88
//
// Layout:
//   0x00-0x5FF: IconBar base class
//   0x600: Palette* (destructor 0x41ea80)
//   0x604: MouseControl* (destructor 0x41f360)
//   0x608: SC_Dialog* (destructor 0x408a40)
//   0x60C-0x633: DialogControl* array[10] (destructor 0x4092e0)
//   0x634-0x647: remaining fields (12 bytes)
class SCI_SearchScreen : public IconBar {
public:
    SCI_SearchScreen();
    ~SCI_SearchScreen();

    // Virtual method overrides
    virtual int LBLParse(char* line);
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

private:
    int FindControlAtMouse();       // 0x40B230
    int GetActiveControlCount();    // 0x40B2B0

public:
    Palette* field_600;              // 0x600
    MouseControl* field_604;         // 0x604
    SC_Dialog* field_608;            // 0x608
    DialogControl* field_60C[10];    // 0x60C-0x633
    DialogControl* field_634;        // 0x634
    Sample* field_638;               // 0x638
    int field_63C;      // 0x63C
    int field_640;      // 0x640
    int field_644;      // 0x644
};

#endif // SCI_SEARCHSCREEN_H
