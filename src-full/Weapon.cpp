#include "RockThrower.h"
#include "Sample.h"
#include "globals.h"
#include "Animation.h"
#include "EngineSubsystems.h"
#include "CursorState.h"
#include <string.h>

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

class CombatResult {
public:
    int state; // 0x00

    void PlayResult();   // 0x415D40
};

/* Function start: 0x415D40 */ /* DEMO ONLY - no full game match */
void CombatResult::PlayResult() {
    switch (CombatResult::state) {
    case 1:
        {
            Animation anim;
            anim.Play("rat1\\lose.smk", 0x20);
        }
        break;
    case 2:
        {
            Animation anim;
            anim.Play("rat1\\win.smk", 0x20);
        }
        break;
    }
}

Weapon::~Weapon() {}

// Weapon::OnHit (0x415E00) and Weapon::DrawExplosion (0x415F10)
// are DEMO ONLY — full game versions are in CombatDisplay.cpp

#include "InputManager.h"
extern InputManager* g_InputManager_0046aa08;
extern int g_ProjectileHits_0043d150;
#include "Projectile.h"
extern void __fastcall FUN_00427390(void*);  // Projectile::Update (full game)

// Access Weapon fields at +0xB0..+0xC0 by offset (not declared in class to avoid layout shift)
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

    if (*(int*)((char*)g_InputManager_0046aa08 + 0x1B4) == 0) {
        goto updateAll;
    }

    DrawCrosshairs();

    pMouse = g_InputManager_0046aa08->pMouse;
    if (pMouse != 0) {
        mouseBtn = pMouse->buttons & 1;
    } else {
        mouseBtn = 0;
    }

    W_HOLD = (mouseBtn == 1) ? 1 : W_HOLD;

    if (!(W_FLAGS & 1)) {
        if (pMouse != 0) {
            mouseBtn = pMouse->buttons & 1;
        } else {
            mouseBtn = 0;
        }
        if (mouseBtn != 0 && W_HOLD != 0) {
            fire = 1;
        } else {
            fire = 0;
        }
    } else {
        fire = 0;
    }
    m_clicked = fire;

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
    g_ProjectileHits_0043d150 = 0;
    W_HITS = 0;
    {
        int i = 0;
        while (i < W_PROJ_COUNT) {
            FUN_00427390((void*)W_PROJ_ARRAY[i]);
            i++;
        }
    }
    W_HITS = g_ProjectileHits_0043d150;
}
