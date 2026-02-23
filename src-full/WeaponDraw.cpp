#include "RockThrower.h"
#include "Target.h"
#include "globals.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

/* Function start: 0x415E20 */ /* DEMO ONLY - no full game match */
void Weapon::DrawCrosshairs() {
    SetFillColor(0xfa);
    SetDrawPosition(Weapon::m_crosshairX, Weapon::m_crosshairY);
    DrawCircle(6);

    SetFillColor(0xfb);
    SetDrawPosition(0x77, 1);
    DrawLine(0x71, 1);
    DrawLine(0x71, 9);
    DrawLine(0x77, 9);

    SetDrawPosition(0xc8, 1);
    DrawLine(0xce, 1);
    DrawLine(0xce, 9);
    DrawLine(0xc8, 9);

    if (g_TargetList_00435f0c->field_0x1ac != 0) {
        SetDrawPosition(Weapon::m_crosshairX, Weapon::m_crosshairY);
        DrawCircle(4);
        DrawCircle(8);
    }
}
