#include "RockThrower.h"
#include "globals.h"
#include "Engine.h"
#include "Projectile.h"
#include "InputManager.h"
#include <string.h>

extern int DAT_0043d150;
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

/* Function start: 0x4169A0 */
int RockThrower::LBLParse(char* line) {
    char token[32];
    int count;

    if (sscanf(line, "%s", token) != 1) {
        return 0;
    }

    if (strcmp(token, "MAXROCKS") == 0) {
        if (sscanf(line, "%s %d", token, &count) == 2) {
            m_itemCount = count;
            m_items = (Projectile**)new char[m_itemCount * 4];
            for (int i = 0; i < m_itemCount; i++) {
                m_items[i] = new Projectile();
            }
        }
    }
    else if (strcmp(token, "SPRITE") == 0) {
        SaveFilePosition();
        if (m_itemCount > 0) {
            for (int i = 0; i < m_itemCount; i++) {
                RestoreFilePosition();
                Parser::ProcessFile(m_items[i], this, 0);
            }
        }
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("RockThrower");
    }

    return 0;
}

/* Function start: 0x416880 */
void RockThrower::UpdateProjectiles() {
    int** vtable;
    InputState* pMouse;
    int buttonState;
    int prevButtons;
    Projectile** projectiles;
    int i;

    if (g_InputManager_00436968->mouseValid == 0) {
        goto update_all;
    }

    vtable = *(int***)this;
    ((void (*)(RockThrower*))vtable[5])(this); // vtable[5] = offset 0x14

    pMouse = g_InputManager_00436968->pMouse;
    buttonState = 0;
    if (pMouse != 0) {
        buttonState = pMouse->buttons & 1;
    }

    if (buttonState == 0 && (pMouse->prevButtons & 1) != 0) {
        RockThrower::field_0xa0 = 1;
    } else {
        RockThrower::field_0xa0 = 0;
    }

    i = 0;
    if (RockThrower::field_0xa0 == 0) {
        goto update_all;
    }

    ((void (*)(RockThrower*))vtable[4])(this); // vtable[4] = offset 0x10

    if (RockThrower::m_itemCount <= 0) {
        goto update_all;
    }

    projectiles = RockThrower::m_items;
    do {
        if (projectiles[i]->active == 0) {
            RockThrower::m_items[i]->Launch();
            goto update_all;
        }
        i++;
    } while (i < RockThrower::m_itemCount);

update_all:
    i = 0;
    DAT_0043d150 = 0;
    RockThrower::field_0xb0 = 0;

    if (RockThrower::m_itemCount > 0) {
        do {
            RockThrower::m_items[i]->Update();
            i++;
        } while (i < RockThrower::m_itemCount);
    }

    RockThrower::field_0xb0 = DAT_0043d150;
}
