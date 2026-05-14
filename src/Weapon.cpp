#include "RockThrower.h"
#include "globals.h"
#include "InputManager.h"
#include "Projectile.h"


Weapon::~Weapon() {}

/* Function start: 0x427880 */
void Weapon::UpdateProjectiles() {
    RockThrower* rt = (RockThrower*)this;
    InputState* pMouse;
    int mouseBtn;

    if (g_InputManager_0046aa08->mouseValid == 0) {
        goto updateAll;
    }

    DrawCrosshairs();

    pMouse = g_InputManager_0046aa08->pMouse;
    if (pMouse != 0) {
        mouseBtn = pMouse->buttons & 1;
    } else {
        mouseBtn = 0;
    }

    rt->m_holdState = (mouseBtn < 1) | rt->m_holdState;

    if (!(rt->m_hitCount & 1)) {
        pMouse = g_InputManager_0046aa08->pMouse;
        if (pMouse != 0) {
            mouseBtn = pMouse->buttons & 1;
        } else {
            mouseBtn = 0;
        }
        if (mouseBtn != 0 && rt->m_holdState != 0) {
            m_clicked = 1;
        } else {
            m_clicked = 0;
        }
    } else {
        m_clicked = 0;
    }

    if (m_clicked == 0) {
        goto updateAll;
    }

    rt->m_holdState = 0;
    OnHit();

    if (rt->m_itemCount > 0) {
        int i = 0;
        Projectile** base = rt->m_items;
        Projectile** arr = base;
        do {
            if ((*arr)->active == 0) {
                base[i]->Launch();
                goto updateAll;
            }
            arr++;
            i++;
        } while (i < rt->m_itemCount);
    }

updateAll:
    g_ProjectileHits_0047325c = 0;
    rt->m_hitCountFull = 0;
    {
        int i = 0;
        while (i < rt->m_itemCount) {
            rt->m_items[i]->Update();
            i++;
        }
    }
    rt->m_hitCountFull = g_ProjectileHits_0047325c;
}
