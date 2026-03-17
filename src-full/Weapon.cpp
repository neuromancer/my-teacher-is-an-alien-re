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
