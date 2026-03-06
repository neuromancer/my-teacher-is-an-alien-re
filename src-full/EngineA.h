#ifndef ENGINEA_H
#define ENGINEA_H

#include "SC_CombatBase.h"

// EngineA (EngineNavigateDucts) - Duct navigation engine
// Constructor: 0x43A4C0, Destructor: 0x43A540
// Scalar deleting destructor: 0x43A520
// Vtable: 0x461808 (18 entries, inherits most from SC_CombatBase)
// Size: 0xF0 (no additional fields beyond SC_CombatBase)
//
// Overrides from SC_CombatBase:
//   [3]  destructor:    0x43A520 (sdtor) / 0x43A540 (body)
//   [13] HandleAction:  0x43A590
class EngineA : public SC_CombatBase {
public:
    EngineA();
    virtual ~EngineA();

    virtual int HandleAction(int* param);  // vtable[13] 0x43A590
};

#endif // ENGINEA_H
