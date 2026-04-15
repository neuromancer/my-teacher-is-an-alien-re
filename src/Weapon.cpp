#include "RockThrower.h"
#include "globals.h"
#include "InputManager.h"
#include "Projectile.h"


Weapon::~Weapon() {}

// Access RockThrower fields via offset (base class accesses derived class layout)
// Maps to: m_itemCount(0xB0), m_items(0xB4), m_hitCount(0xB8), m_holdState(0xBC), m_hitCountFull(0xC0)
#define W_PROJ_COUNT   (*(int*)((char*)this + 0xB0))
#define W_PROJ_ARRAY   (*(int**)((char*)this + 0xB4))
#define W_FLAGS        (*(int*)((char*)this + 0xB8))
#define W_HOLD         (*(int*)((char*)this + 0xBC))
#define W_HITS         (*(int*)((char*)this + 0xC0))

/* Function start: 0x427880 */
void Weapon::UpdateProjectiles() {
    InputState* pMouse;
    int mouseBtn;
    int fire;

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

    W_HOLD = (mouseBtn < 1) | W_HOLD;

    if (!(W_FLAGS & 1)) {
        pMouse = g_InputManager_0046aa08->pMouse;
        if (pMouse != 0) {
            mouseBtn = pMouse->buttons & 1;
        } else {
            mouseBtn = 0;
        }
        if (mouseBtn != 0 && W_HOLD != 0) {
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

    W_HOLD = 0;
    OnHit();

    {
        int i = 0;
        int* arr = W_PROJ_ARRAY;
        while (i < W_PROJ_COUNT) {
            if (*(int*)(*arr + 0x11C) == 0) {
                ((Projectile*)W_PROJ_ARRAY[i])->Launch();
                break;
            }
            arr++;
            i++;
        }
    }

updateAll:
    g_ProjectileHits_0047325c = 0;
    W_HITS = 0;
    {
        int i = 0;
        while (i < W_PROJ_COUNT) {
            ((Projectile*)W_PROJ_ARRAY[i])->Update();
            i++;
        }
    }
    W_HITS = g_ProjectileHits_0047325c;
}
