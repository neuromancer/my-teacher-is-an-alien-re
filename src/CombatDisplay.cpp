#include "CombatDisplay.h"
#include "SC_CombatBase.h"
#include "globals.h"
#include "ScoreDisplay.h"
#include "Sample.h"
#include "Target.h"
#include "Graphics.h"
#include "PaletteUtils.h"
#include "VideoTable.h"


/* Function start: 0x426E00 */
void Weapon::OnHit()
{
    if (g_ScoreDisplay_0046ae6c != 0) {
        g_ScoreDisplay_0046ae6c->shotsFired++;
    }
    if (m_sound != 0) {
        m_sound->Play(100, 1);
    }
}

/* Function start: 0x426E30 */
void CombatDisplay::DrawCrosshairs()
{
    SetFillColor(0xfa);
    SetDrawPosition(m_crosshair.x, m_crosshair.y);
    DrawCircle(6);

    if (g_TargetList_0046ae58 != 0 && g_TargetList_0046ae58->field_1B4 != 0) {
        SetDrawPosition(m_crosshair.x, m_crosshair.y);
        DrawCircle(4);
        DrawCircle(8);
    }
}

/* Function start: 0x426EB0 */
void CombatDisplay::OnHit()
{
    Weapon::OnHit();
    SetFillColor(0xfd);

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

CombatDisplay::~CombatDisplay()
{
}
