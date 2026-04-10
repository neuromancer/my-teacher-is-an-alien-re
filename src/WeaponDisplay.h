#ifndef WEAPONDISPLAY_H
#define WEAPONDISPLAY_H

#include "RockThrower.h"

// WeaponDisplay - Alternate weapon display (used when WEAPON is not ROCKTHROWER)
// Constructor: inlined in SC_CombatBase::LBLParse
// Vtable: 0x4615B8 (6 entries)
// Size: 0xB0 (same as Weapon, no extra fields)
//
// vtable layout:
//   [0] LBLParse        0x405CD0 (inherited)
//   [1] OnProcessStart  0x401140 (inherited)
//   [2] OnProcessEnd    0x401150 (inherited)
//   [3] ~WeaponDisplay  0x42D130 (sdtor)
//   [4] OnHit           0x427E50 (override)
//   [5] DrawCrosshairs  0x427DC0 (override)
class WeaponDisplay : public Weapon {
public:
    virtual ~WeaponDisplay();
    virtual void OnHit();
    virtual void DrawCrosshairs();
};

#endif // WEAPONDISPLAY_H
