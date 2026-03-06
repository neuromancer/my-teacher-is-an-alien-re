#ifndef SC_PODS_H
#define SC_PODS_H

#include "Handler.h"

// SC_Pods - Pods scene handler (case 68/0x44)
// Constructor: 0x4415E0
// Size: 0xB0
// Vtable: 0x461918
// References: "cb_Pods.mis"
class SC_Pods : public Handler {
public:
    SC_Pods();
    ~SC_Pods();

    virtual int LBLParse(char* line);       // 0x442090
    virtual void Init(SC_Message* msg);     // 0x441700
    virtual int AddMessage(SC_Message* msg); // 0x441AE0
    virtual int ShutDown(SC_Message* msg);  // 0x4419E0

    void HandleResult();                    // 0x441B60

    int field_A8[2];    // 0xA8-0xAF (0x08 bytes)
};

#endif // SC_PODS_H
