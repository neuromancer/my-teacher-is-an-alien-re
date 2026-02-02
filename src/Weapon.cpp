#include "RockThrower.h"
#include "Sample.h"
#include "globals.h"
#include "Animation.h"
#include "EngineSubsystems.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl FUN_00422aaf(int param_1);
extern "C" int __cdecl FUN_00422ac3(int param_1, int param_2);

class CombatResult {
public:
    int state; // 0x00

    void PlayResult();   // 0x415D40
};

/* Function start: 0x415D40 */
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

/* Function start: 0x415E00 */
void Weapon::OnHit() {
    g_ScoreManager[6]++;
    if (Weapon::field_0xa4 != 0) {
        ((Sample*)Weapon::field_0xa4)->Play(100, 1);
    }
}

/* Function start: 0x415F10 */
void Weapon::DrawExplosion() {
    Weapon::OnHit();
    SetFillColor(0xfd);

    SetDrawPosition(Weapon::m_posX - 4, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posX - 3, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posX - 2, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posX - 1, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posX + 4, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posX + 3, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posX + 2, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posX + 1, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posY - 4, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posY - 3, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posY - 2, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posY - 1, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posY + 4, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posY + 3, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posY + 2, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posY + 1, Weapon::m_posZ);

    SetDrawPosition(Weapon::m_posX, Weapon::m_posZ);
    FUN_00422ac3(Weapon::field_0x98, Weapon::field_0x9c);
    FUN_00422ac3(Weapon::m_posY, Weapon::m_posZ);
}
