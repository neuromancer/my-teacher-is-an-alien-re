#include "CombatWeapon.h"
#include "SC_Combat.h"
#include "GameEngine.h"
#include "globals.h"

CombatWeapon::~CombatWeapon() {}

/* Function start: 0x408E60 */
void CombatWeapon::OnHit() {
    int* handler = (int*)g_GameEngine_0046a6ec->m_activeHandler;
    int* vtbl = (int*)*handler;
    ((void (__fastcall*)(int*, int))vtbl[12])(handler, 0);
}

/* Function start: 0x408E40 */
int CombatWeapon::CheckTargetHit(int param) {
    int* handler = (int*)g_GameEngine_0046a6ec->m_activeHandler;
    int* vtbl = (int*)*handler;
    return ((int (__fastcall*)(int*, int, int))vtbl[11])(handler, 0, param);
}
