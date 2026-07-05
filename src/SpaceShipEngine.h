#ifndef SPACESHIPENGINE_H
#define SPACESHIPENGINE_H

#include "SC_CombatBase.h"

class SpriteAction;

// SpaceShipEngine - Spaceship combat engine (SC_CombatBase-derived)
// Constructor: 0x447C30 (SEH)
// Destructor: 0x447CB0 (SEH)
// Scalar deleting destructor: 0x447C90
// Destructor funclet 0x447CF7 calls SC_CombatBase::~SC_CombatBase (0x42BDA0)
// Vtable: 0x461A10 (18 entries)
// Size: 0xF0 (same as SC_CombatBase)
//
// vtable layout (only override vs SC_CombatBase shown):
//   [13] HandleAction  0x447D00 (override)
//   All other slots inherited from SC_CombatBase
//
// Error string: "EngineSpaceShipNav::OnLogicMessage - unsupported instruction %d"
class SpaceShipEngine : public SC_CombatBase {
public:
    SpaceShipEngine();                       // 0x447C30
    virtual ~SpaceShipEngine();              // 0x447CB0
    virtual int HandleAction(SpriteAction* param);    // 0x447D00
};

#endif // SPACESHIPENGINE_H
