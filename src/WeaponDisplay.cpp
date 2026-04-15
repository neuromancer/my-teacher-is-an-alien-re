#include "WeaponDisplay.h"
#include "SC_CombatBase.h"
#include "Sample.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

/* Function start: 0x427DC0 */ /* No assembly extracted */
void WeaponDisplay::DrawCrosshairs()
{
}

/* Function start: 0x427E50 */
void WeaponDisplay::OnHit()
{
    Weapon::OnHit();
    SetFillColor(0xfd);

    m_posY = 0xa0;
    m_posX = 0xa0;
    m_posZ = 0xc7;

    SetDrawPosition(m_posX - 4, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posX - 3, m_posZ);

    SetDrawPosition(m_posX - 2, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posX - 1, m_posZ);

    SetDrawPosition(m_posX + 4, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posX + 3, m_posZ);

    SetDrawPosition(m_posX + 2, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posX + 1, m_posZ);

    SetDrawPosition(m_posY - 4, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posY - 3, m_posZ);

    SetDrawPosition(m_posY - 2, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posY - 1, m_posZ);

    SetDrawPosition(m_posY + 4, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posY + 3, m_posZ);

    SetDrawPosition(m_posY + 2, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posY + 1, m_posZ);

    SetDrawPosition(m_posX, m_posZ);
    DrawLine(m_crosshair.x, m_crosshair.y);
    DrawLine(m_posY, m_posZ);
}

WeaponDisplay::~WeaponDisplay()
{
}
