#ifndef SCI_SCHEDULE_H
#define SCI_SCHEDULE_H

#include "IconBar.h"

class SC_Message;

// SCI_Schedule - School schedule/map screen (case 39/0x27)
// Constructor: 0x434C10, Destructor: 0x434FB0
// Size: 0xD8, Vtable: 0x4616F8
// Extends IconBar
// References: "mis\\schedule.mis", "schedual\\schedule.col", "HOTSPOT"
class SCI_Schedule : public IconBar {
public:
    SCI_Schedule();                                 // 0x434C10
    ~SCI_Schedule();                                // 0x434FB0

    virtual int LBLParse(char* param);              // 0x4356E0
    virtual void Init(SC_Message* msg);             // 0x4350C0
    virtual int AddMessage(SC_Message* msg);        // 0x435440
    virtual int ShutDown(SC_Message* msg);          // 0x4353E0
    virtual void Update(int param1, int param2);    // 0x435520
    virtual int Exit(SC_Message* msg);              // 0x435500

    int field_A8[12];   // 0xA8-0xD7 (0x30 bytes)
};

#endif // SCI_SCHEDULE_H
