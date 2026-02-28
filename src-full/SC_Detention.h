#ifndef SC_DETENTION_H
#define SC_DETENTION_H

#include "Handler.h"

class SC_Message;

// SC_Detention - Detention scene handler (case 44/0x2C)
// Constructor: 0x4098C0, Destructor: 0x409A60
// Size: 0x1E8
// Vtable: 0x461148
// References: "detention.mis", "SC_Detention::Init"
//   Reads: PERIOD, ACTIONS, NUM_ACTIONS, IN_PRACTICEROOM,
//          COMBAT_POD_AVAILABLE, PLAYED_ANNOUNCEMENT
class SC_Detention : public Handler {
public:
    SC_Detention();
    ~SC_Detention();

    virtual int AddMessage(SC_Message* msg);  // 0x409AD0
    virtual int Exit(SC_Message* msg);        // 0x409F70

    void InitDetention();           // 0x40A9B0
    void GoToClass();               // 0x40A700
    void HandleInput3C();           // 0x40A780
    void ProcessDetention();        // 0x40A820
    void HandleInput3F();           // 0x40A8D0
    void SetupDetentionState();     // 0x40B030
    void DrawDetention();           // 0x40AD50
    void HandleCombat();            // 0x40AB90
    void HandleInput40();           // 0x40AB30
    void HandlePractice();          // 0x40AC70
    void ResetAnimations();         // 0x40A130
    void ProcessPeriodAction(int param);  // 0x40AD80

    int field_A8[18];     // 0xA8-0xEF
    int field_F0[22];     // 0xF0-0x147
    int field_148;        // 0x148 - actions count for current period
    int field_14C;        // 0x14C - PLAYED_ANNOUNCEMENT state index
    int field_150;        // 0x150
    int field_154;        // 0x154 - NUM_ACTIONS state index
    int field_158;        // 0x158 - IN_PRACTICEROOM state index
    int field_15C;        // 0x15C - COMBAT_POD_AVAILABLE state index
    int field_160;        // 0x160
    char field_164[32];   // 0x164-0x183 - "%c_DETENTION_SUSPENSION"
    char field_184[32];   // 0x184-0x1A3 - "%c_TATTLES"
    char field_1A4[32];   // 0x1A4-0x1C3 - "%c_LATE_CUT"
    char field_1C4[36];   // 0x1C4-0x1E7 - "%c_IN_DETENTION"
};

#endif // SC_DETENTION_H
