#ifndef SC_PODS_H
#define SC_PODS_H

#include "Handler.h"

class SpriteAction;
class Engine;

// SC_Pods - Pods scene handler (case 68/0x44)
// Constructor: 0x4415E0
// Size: 0xB0
// Vtable: 0x461918
// References: "cb_Pods.mis"
class SC_Pods : public Handler {
public:
    SC_Pods();
    ~SC_Pods();

    virtual int LBLParse(char* line);       // 0x4420B3
    virtual void Init(SC_MessageParser* msg);     // 0x441700
    virtual int AddMessage(SC_MessageParser* msg); // 0x441AE0
    virtual void ShutDown(SC_MessageParser* msg);  // 0x4419E0
    virtual void Update(int p1, int p2);          // 0x441AA0
    virtual int Exit(SC_MessageParser* msg);      // 0x441B20

    void HandleResult();                    // 0x441B60

    int resultAction;   // 0xA8 — SpriteAction* for scene result
    int combatEngine;   // 0xAC — Engine*/SC_CombatBase* for combat logic
};

#endif // SC_PODS_H
