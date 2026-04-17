#ifndef SCI_SEARCHSCREEN_H
#define SCI_SEARCHSCREEN_H

#include "SC_Combat.h"

class SC_MessageParser;

// SCI_SearchScreen - Search screen handler (full game: case 72/0x48)
// Constructor: 0x40B420
// Vtable: 0x461178
// Size: 0x118 (same as SC_Combat, no additional fields)
// Extends SC_Combat (0x118 bytes)
//
// NOTE: In the demo, this was case 11/0x0B extending IconBar (0x648 bytes).
//       The full game completely redesigned this class.
//
// field_A8[0..N] used by ANNOUNCE labels (indexed stores)
// field_A8[20..N] used by ENDPERIODANNOUNCE labels (offset 0xF8 = 0xA8 + 0x50)
class SCI_SearchScreen : public SC_Combat {
public:
    SCI_SearchScreen();                         // 0x40B420
    ~SCI_SearchScreen();                        // 0x40B4B0

    virtual int LBLParse(char* line);           // [0]  0x40BAD0
    virtual void OnProcessStart();             // [1]  0x40BAB0
    virtual void OnProcessEnd();               // [2]  0x40BAC0
    virtual void Init(SC_MessageParser* msg);        // [4]  0x40B510
    virtual int ShutDown(SC_MessageParser* msg);     // [6]  0x40B780
    virtual int AddMessage(SC_MessageParser* msg);   // [5]  0x40B7E0
    virtual void Update(int p1, int p2);       // [7]  0x40B7C0
    virtual int Exit(SC_MessageParser* msg);         // [8]  0x40B830
    virtual void ProcessLose();                // [11] 0x40B860
    virtual void ProcessAction(int action, int* data); // [12] 0x40BA10
};

#endif // SCI_SEARCHSCREEN_H
