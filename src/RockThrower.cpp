#include "RockThrower.h"
#include "globals.h"
#include "Engine.h"
#include "Projectile.h"
#include "InputManager.h"
#include <string.h>

extern int g_ProjectileHits_0043d150;
extern Engine* g_CombatEngine_00435eb0;

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);

/* Function start: 0x4165D0 */
RockThrower::RockThrower() {
    int* pItemCount = &m_itemCount;

    pItemCount[0] = 0;
    pItemCount[1] = 0;
    pItemCount[2] = 0;
    pItemCount[3] = 0;

    RockThrower::m_posY = 0xa0;
    RockThrower::m_posX = 0xa0;
    RockThrower::m_posZ = 0xa6;

    pItemCount[0] = 3;

    Parser::ProcessFile(this, g_CombatEngine_00435eb0, 0);
}

RockThrower::~RockThrower() {}

/* Function start: 0x416960 */
void RockThrower::DrawCrosshairs() {
    SetFillColor(0xfa);
    SetDrawPosition(RockThrower::m_crosshairX, RockThrower::m_crosshairY);
    DrawCircle(7);
}

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
            m_items = new Projectile*[m_itemCount];
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
    InputState* pMouse;
    int buttonState;
    int prevButtons;
    Projectile** projectiles;
    int i;

    if (g_InputManager_00436968->mouseValid == 0) {
        goto update_all;
    }

    DrawCrosshairs();

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

    OnHit();

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
    g_ProjectileHits_0043d150 = 0;
    RockThrower::field_0xb0 = 0;

    if (RockThrower::m_itemCount > 0) {
        do {
            RockThrower::m_items[i]->Update();
            i++;
        } while (i < RockThrower::m_itemCount);
    }

    RockThrower::field_0xb0 = g_ProjectileHits_0043d150;
}
