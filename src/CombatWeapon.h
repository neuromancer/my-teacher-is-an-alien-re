#ifndef COMBATWEAPON_H
#define COMBATWEAPON_H

#include "RockThrower.h"

// CombatWeapon - Crosshair weapon for mini-game handlers (SC_FireAlarm, SC_Slime, SC_Fan, SC_Wahoo)
// Scalar deleting destructor: 0x408E70
// Destructor funclet 0x408EC9 calls RockThrower::~RockThrower (0x427710)
// Vtable: 0x461118 (7 entries)
// Size: 0xC8 (same as RockThrower)
//
// vtable layout:
//   [0] LBLParse        0x427B20 (shared with RockThrower)
//   [1] OnProcessStart  0x401140 (inherited)
//   [2] OnProcessEnd    0x401150 (inherited)
//   [3] ~CombatWeapon   0x408E70 (sdtor)
//   [4] OnHit           0x408E60 (override — delegates to activeHandler vtable[12])
//   [5] DrawCrosshairs  0x4279D0 (shared with RockThrower)
//   [6] CheckTargetHit  0x408E40 (override — delegates to activeHandler vtable[11])
//
// OnHit and CheckTargetHit are vtable dispatch trampolines:
// they forward to the active handler's extended virtual slots [11] and [12],
// which are defined by handlers like SC_FireAlarm (HandleClick, PlaySound).
class CombatWeapon : public RockThrower {
public:
    CombatWeapon(Parser* parent) : RockThrower(parent) {}
    virtual ~CombatWeapon();

    virtual void OnHit();              // 0x408E60
    virtual int CheckTargetHit(int);   // 0x408E40
};

#endif // COMBATWEAPON_H
