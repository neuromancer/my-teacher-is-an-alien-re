#include "CombatWeapon.h"
#include "GameEngine.h"
#include "Handler.h"
#include "globals.h"

class CombatWeaponHandler : public Handler {
public:
    virtual int CheckCombatWeaponTarget(int param) = 0;
    virtual void ProcessCombatWeaponHit() = 0;
};

CombatWeapon::~CombatWeapon() {}

/* Function start: 0x408E60 */
void CombatWeapon::OnHit() {
    ((CombatWeaponHandler*)g_GameEngine_0046a6ec->m_activeHandler)->ProcessCombatWeaponHit();
}

/* Function start: 0x408E40 */
int CombatWeapon::CheckTargetHit(int param) {
    return ((CombatWeaponHandler*)g_GameEngine_0046a6ec->m_activeHandler)->CheckCombatWeaponTarget(param);
}
