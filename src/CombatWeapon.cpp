#include "CombatWeapon.h"
#include "SC_Combat.h"
#include "GameEngine.h"
#include "globals.h"

CombatWeapon::~CombatWeapon() {}

/* Function start: 0x408E60 */
void CombatWeapon::OnHit() {
    // Trampoline: tail call to activeHandler->vtable[12]
    // Original asm: mov eax,[g_GameEngine]; mov ecx,[eax+18h]; mov eax,[ecx]; jmp [eax+30h]
    ((SC_Combat*)g_GameEngine_0046a6ec->m_activeHandler)->ProcessAction(0, 0);
}

/* Function start: 0x408E40 */
int CombatWeapon::CheckTargetHit(int param) {
    // Trampoline: call activeHandler->vtable[11] with param
    // Original asm: mov eax,[g_GameEngine]; push param; mov ecx,[eax+18h]; mov eax,[ecx]; call [eax+2Ch]; ret 4
    ((SC_Combat*)g_GameEngine_0046a6ec->m_activeHandler)->ProcessLose();
    return 0;
}
