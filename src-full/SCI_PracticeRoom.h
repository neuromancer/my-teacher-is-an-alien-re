#ifndef SCI_PRACTICEROOM_H
#define SCI_PRACTICEROOM_H

#include "Handler.h"

// SCI_PracticeRoom - Practice room handler (case 43/0x2B)
// Constructor: 0x42A8D0
// Size: 0xD0
// Vtable: 0x461520
// References: "IN_PRACTICEROOM", "SCI_PracticeRoom"
class SCI_PracticeRoom : public Handler {
public:
    SCI_PracticeRoom();
    ~SCI_PracticeRoom();

    // Virtual overrides
    virtual void Init(SC_Message* msg);       // 0x42A9F0 (+0x10)
    virtual int AddMessage(SC_Message* msg);  // 0x42AE80 (+0x14)
    virtual int ShutDown(SC_Message* msg);    // 0x42AC20 (+0x18)
    virtual void Update(int param1, int param2); // 0x42AD80 (+0x1C)
    virtual int Exit(SC_Message* msg);        // 0x42B030 (+0x20)

    virtual int LBLParse(char* param_1);      // 0x42B3B0 (+0x00)

    int field_A8[10];   // 0xA8-0xCF (0x28 bytes)
};

#endif // SCI_PRACTICEROOM_H
