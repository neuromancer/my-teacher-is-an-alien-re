#include "CombatDisplay.h"
#include "SC_CombatBase.h"
#include "Sample.h"

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

/* Function start: 0x426E00 */
void Weapon::OnHit()
{
    if (DAT_0046ae6c != 0) {
        ((int*)DAT_0046ae6c)[6]++;
    }
    if (m_sound != 0) {
        ((Sample*)m_sound)->Play(100, 1);
    }
}

/* Function start: 0x426E30 */
void CombatDisplay::DrawCrosshairs()
{
}

/* Function start: 0x426EB0 */
void CombatDisplay::OnHit()
{
    Weapon::OnHit();
    SetFillColor(0xfd);

    SetDrawPosition(m_posX - 4, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posX - 3, m_posZ);

    SetDrawPosition(m_posX - 2, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posX - 1, m_posZ);

    SetDrawPosition(m_posX + 4, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posX + 3, m_posZ);

    SetDrawPosition(m_posX + 2, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posX + 1, m_posZ);

    SetDrawPosition(m_posY - 4, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posY - 3, m_posZ);

    SetDrawPosition(m_posY - 2, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posY - 1, m_posZ);

    SetDrawPosition(m_posY + 4, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posY + 3, m_posZ);

    SetDrawPosition(m_posY + 2, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posY + 1, m_posZ);

    SetDrawPosition(m_posX, m_posZ);
    DrawLine(m_crosshairX, m_crosshairY);
    DrawLine(m_posY, m_posZ);
}

CombatDisplay::~CombatDisplay()
{
}
