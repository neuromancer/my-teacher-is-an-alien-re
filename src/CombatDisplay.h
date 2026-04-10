#ifndef COMBATDISPLAY_H
#define COMBATDISPLAY_H

#include "RockThrower.h"

// CombatDisplay - Default weapon display (crosshair/meter overlay)
// Constructor: inlined in SC_CombatBase::Initialize
// Destructor: 0x42C5C0 (sdtor), base dtor 0x4275E0
// Vtable: 0x4615A0 (6 entries)
// Size: 0xB0 (same as Weapon, no extra fields)
//
// vtable layout:
//   [0] LBLParse        0x405CD0 (inherited Parser default)
//   [1] OnProcessStart  0x401140 (inherited)
//   [2] OnProcessEnd    0x401150 (inherited)
//   [3] ~CombatDisplay  0x42C5C0 (sdtor)
//   [4] OnHit           0x426EB0 (override - render crosshair lines)
//   [5] DrawCrosshairs  0x426E30 (override)
class CombatDisplay : public Weapon {
public:
    virtual ~CombatDisplay();
    virtual void OnHit();
    virtual void DrawCrosshairs();
};

#endif // COMBATDISPLAY_H
