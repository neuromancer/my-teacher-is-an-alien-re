#include "EngineC.h"
#include "Sprite.h"
#include "Memory.h"
#include "InputManager.h"
#include "RockThrower.h"
#include "Target.h"
#include <string.h>

extern "C" InputManager* g_InputManager_0046aa08;
/* Function start: 0x40BBF0 */
EngineC::EngineC()
{
    int* p;
    p = &field_0xF4;
    p[0] = 0;
    p[1] = 0;
    p = &field_0xFC;
    p[0] = 0;
    p[1] = 0;
    p = &field_0x108;
    p[0] = 0;
    p[1] = 0;
    memset(&mouseReleased, 0, 0xC * 4);
}

/* Function start: 0x40BCC0 */
EngineC::~EngineC()
{
    if (sprite118 != 0) {
        delete (Sprite*)sprite118;
        sprite118 = 0;
    }
    if (sprite104 != 0) {
        delete (Sprite*)sprite104;
        sprite104 = 0;
    }
    if (sprite110 != 0) {
        delete (Sprite*)sprite110;
        sprite110 = 0;
    }
    if (sprite114 != 0) {
        delete (Sprite*)sprite114;
        sprite114 = 0;
    }
}

/* Function start: 0x40C0D0 */
void EngineC::method10()
{
    Target* target = (Target*)g_TargetList_0046ae58->ProcessTargets();

    g_CombatWeapon_0046ae60->DrawCrosshairs();

    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    int buttonDown;
    if (pMouse == 0) {
        buttonDown = 0;
    } else {
        buttonDown = pMouse->buttons & 1;
    }

    mouseReleased |= (buttonDown == 0) ? 1 : 0;
    g_CombatWeapon_0046ae60->m_clicked = 0;

    if (g_BgSprite_0046ae50->handle <= 4) {
        if (pMouse == 0) {
            buttonDown = 0;
        } else {
            buttonDown = pMouse->buttons & 1;
        }
        if (buttonDown != 0 && mouseReleased != 0) {
            g_CombatWeapon_0046ae60->m_clicked = 1;
            mouseReleased = 0;
        }
    }

    if (g_CombatWeapon_0046ae60->m_clicked != 0) {
        g_CombatWeapon_0046ae60->OnHit();
        if (target != 0) {
            target->UpdateProgress(1);
        }
    }

    RenderBackground();
}
