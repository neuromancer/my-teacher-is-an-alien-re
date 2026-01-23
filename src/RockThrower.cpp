#include "RockThrower.h"
#include "globals.h"
#include "Engine.h"
#include <string.h>

extern Engine* g_CombatEngine;

/* Function start: 0x4165D0 */
RockThrower::RockThrower() {
    // Zero Weapon fields 0x98, 0x9c first (redundant but matches original)
    field_0x98 = 0;
    field_0x9c = 0;

    // Zero all Weapon fields (0x88-0xa7 = 32 bytes = 8 dwords)
    int* pFields = &m_posX;
    for (int i = 8; i != 0; i--) {
        *pFields = 0;
        pFields++;
    }

    // Initialize Weapon positions
    field_0x94 = 0;
    field_0xa4 = 0;
    m_posX = 0x64;
    field_0xa0 = 0;
    m_posY = 0xdc;
    m_posZ = 0xc7;

    // Zero RockThrower fields using pointer (matches LEA EAX,[ESI+0xa8] pattern)
    int* pItemCount = &m_itemCount;
    pItemCount[0] = 0;
    pItemCount[1] = 0;
    pItemCount[2] = 0;
    pItemCount[3] = 0;

    // Update positions
    m_posY = 0xa0;
    m_posX = 0xa0;
    m_posZ = 0xa6;

    // Set item count
    pItemCount[0] = 3;

    // Initialize from combat engine
    Parser::ProcessFile(this, (Parser*)g_CombatEngine, (char*)0);
}

RockThrower::~RockThrower() {}

int RockThrower::LBLParse(char* line) {
    return 0;
}
