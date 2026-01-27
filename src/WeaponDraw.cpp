#include "RockThrower.h"
#include "globals.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl FUN_00422aaf(int param_1);
extern "C" int __cdecl FUN_00422ac3(int param_1, int param_2);

extern Parser* DAT_00435f0c;

/* Function start: 0x415E20 */
void Weapon::DrawCrosshairs() {
    SetFillColor(0xfa);
    SetDrawPosition(Weapon::field_0x98, Weapon::field_0x9c);
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

    if (*(int*)((char*)DAT_00435f0c + 0x1ac) != 0) {
        SetDrawPosition(Weapon::field_0x98, Weapon::field_0x9c);
        FUN_00422aaf(4);
        FUN_00422aaf(8);
    }
}
