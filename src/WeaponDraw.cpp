#include "RockThrower.h"
#include "Target.h"
#include "globals.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl FUN_00422aaf(int param_1);
extern "C" int __cdecl FUN_00422ac3(int param_1, int param_2);

/* Function start: 0x415E20 */
void Weapon::DrawCrosshairs() {
    SetFillColor(0xfa);
    SetDrawPosition(Weapon::m_crosshairX, Weapon::m_crosshairY);
    FUN_00422aaf(6);

    SetFillColor(0xfb);
    SetDrawPosition(0x77, 1);
    FUN_00422ac3(0x71, 1);
    FUN_00422ac3(0x71, 9);
    FUN_00422ac3(0x77, 9);

    SetDrawPosition(0xc8, 1);
    FUN_00422ac3(0xce, 1);
    FUN_00422ac3(0xce, 9);
    FUN_00422ac3(0xc8, 9);

    if (((TargetList*)DAT_00435f0c)->field_0x1ac != 0) {
        SetDrawPosition(Weapon::m_crosshairX, Weapon::m_crosshairY);
        FUN_00422aaf(4);
        FUN_00422aaf(8);
    }
}
