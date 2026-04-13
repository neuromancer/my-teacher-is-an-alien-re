#ifndef SC_RATS_H
#define SC_RATS_H

#include "Handler.h"

class SpriteAction;
class SC_CombatBase;

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
    void Init(SC_MessageParser* msg);
    int AddMessage(SC_MessageParser* msg);
    int ShutDown(SC_MessageParser* msg);
    void Update(int param1, int param2);
    int Exit(SC_MessageParser* msg);

    void ProcessState();
    void State0Handler();
    void State1Handler();
    void State2Handler();
    void State3Handler();
    void State4Handler();

    SpriteAction* actionData;  // 0xA8 - sprite action for combat state transitions
    SC_CombatBase* combatEngine; // 0xAC - EngineB* for the rats combat engine
};

#endif // SC_RATS_H
