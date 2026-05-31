#ifndef SC_FAKEROOM_H
#define SC_FAKEROOM_H

#include "SC_Combat.h"
#include "InvSlotItem.h"

// SC_FakeRoom - Fake room handler (case 73/0x49)
// Constructor: 0x4441E0
// Size: 0x130
// Vtable: 0x461988
// Extends SC_Combat (0x118)
// References: "SC_FakeRoom::Process_Action"
class SC_FakeRoom : public SC_Combat {
public:
    int stateFlags;     // 0x118 — flags (bit 0: dragging)
    Rect hitbox;        // 0x11C-0x12B — drag hitbox (has dtor; unwound at state 1)
    int field_0x12C;    // 0x12C

    SC_FakeRoom();                                 // 0x4441E0
    virtual ~SC_FakeRoom();                        // 0x444280 (sdtor)

    virtual int LBLParse(char* line);              // [0]  0x4448A0
    virtual void OnProcessEnd();                   // [2]  0x444840
    virtual void Init(SC_MessageParser* msg);            // [4]  0x444320
    virtual int AddMessage(SC_MessageParser* msg);       // [5]  0x4443E0
    virtual void ShutDown(SC_MessageParser* msg);         // [6]  0x444380
    virtual void Update(int p1, int p2);           // [7]  0x4443C0
    virtual int Exit(SC_MessageParser* msg);             // [8]  0x4444B0
    virtual void ProcessLose();                    // [11] 0x4444E0
    virtual void ProcessAction(int action, int* data); // [12] 0x4447B0

    void RenderFrame();                            // 0x444680
};

#endif // SC_FAKEROOM_H
