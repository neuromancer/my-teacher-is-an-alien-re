#ifndef SC_SPACESHIPNAV_H
#define SC_SPACESHIPNAV_H

#include "SC_Combat.h"

class SpaceShipEngine;

// SC_SpaceShipNav - Spaceship navigation handler (case 70/0x46)
// Constructor: 0x447520
// Size: 0x120
// Vtable: 0x4619D8
// Extends SC_Combat (0x118)
// References: "NAVIGATION", "SC_SpaceShipNav::Process_Action"
class SC_SpaceShipNav : public SC_Combat {
public:
    SC_SpaceShipNav();
    ~SC_SpaceShipNav();

    virtual void OnProcessStart();              // [1]  0x447B40
    virtual void OnProcessEnd();               // [2]  0x447B50
    virtual void Init(SC_MessageParser* msg);        // [4]  0x447630
    virtual int AddMessage(SC_MessageParser* msg);   // [5]  0x447810
    void ShutDown(SC_MessageParser* msg);  // 0x447790
    virtual void Update(int p1, int p2);       // [7]  0x4477F0
    virtual int Exit(SC_MessageParser* msg);         // [8]  0x447910
    virtual void ProcessLose();                // [11] 0x447940
    virtual void ProcessAction(int action, int* data); // [12] 0x447AD0
    void UpdateCursor();            // 0x447950
    int LBLParse(char* line);       // 0x447B60

    SpaceShipEngine* engine;  // 0x118
    int field_0x11C;          // 0x11C
};

#endif // SC_SPACESHIPNAV_H
