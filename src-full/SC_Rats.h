#ifndef SC_RATS_H
#define SC_RATS_H

#include "Handler.h"

// SC_Rats - Rats scene handler (case 66/0x42)
// Constructor: 0x451700
// Size: 0xB0
// Vtable: 0x461CE0
// References: "cb_rats.mis"
class SC_Rats : public Handler {
public:
    SC_Rats();
    ~SC_Rats();

    int LBLParse(char* param);
    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    void Update(int param1, int param2);
    int Exit(SC_Message* msg);

    void ProcessState();
    void State0Handler();
    void State1Handler();
    void State2Handler();
    void State3Handler();
    void State4Handler();

    int field_A8;    // 0xA8 - sprite data pointer
    int field_AC;    // 0xAC - engine info object pointer
};

#endif // SC_RATS_H
