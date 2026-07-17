#include "WeaponDisplay.h"
#include "SC_CombatBase.h"
#include "Sample.h"
#include "Graphics.h"
#include "PaletteUtils.h"
#include "VideoTable.h"
#include "globals.h"
#include "MouseControl.h"
#include "Target.h"


/* Function start: 0x427DC0 */
void WeaponDisplay::DrawCrosshairs()
{
    if (g_Mouse_0046aa18 != 0) {
        g_Mouse_0046aa18->DrawCursor();
        return;
    }
    SetFillColor(0xfa);
    SetDrawPosition(m_crosshair.x, m_crosshair.y);
    DrawCircle(6);
    if (g_TargetList_0046ae58 != 0 && g_TargetList_0046ae58->field_1B4 != 0) {
        SetDrawPosition(m_crosshair.x, m_crosshair.y);
        DrawCircle(4);
        DrawCircle(8);
    }
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
