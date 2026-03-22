#include "EngineC.h"
#include "Sprite.h"
#include "Memory.h"
#include <string.h>

class InputManager;
extern InputManager* g_InputManager_0046aa08;
#include "Target.h"
/* Function start: 0x40BBF0 */
EngineC::EngineC()
{
    field_0xF4 = 0;
    field_0xF8 = 0;
    field_0xFC = 0;
    field_0x100 = 0;
    field_0x108 = 0;
    field_0x10C = 0;
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
    void* target = g_TargetList_0046ae58->ProcessTargets();

    int* obj60 = (int*)g_CombatWeapon_0046ae60;
    int* vtbl60 = (int*)*obj60;
    ((void (__fastcall *)(int*, int))vtbl60[5])(obj60, 0);

    int* pMouse = *(int**)((char*)g_InputManager_0046aa08 + 0x1a0);
    int buttonDown;
    if (pMouse == 0) {
        buttonDown = 0;
    } else {
        buttonDown = *(int*)((char*)pMouse + 0x8) & 1;
    }

    mouseReleased |= (buttonDown == 0) ? 1 : 0;
    ((int*)g_CombatWeapon_0046ae60)[0x2a] = 0;

    if (g_BgSprite_0046ae50->handle < 5) {
        if (pMouse == 0) {
            buttonDown = 0;
        } else {
            buttonDown = *(int*)((char*)pMouse + 0x8) & 1;
        }
        if (buttonDown != 0 && mouseReleased != 0) {
            ((int*)g_CombatWeapon_0046ae60)[0x2a] = 1;
            mouseReleased = 0;
        }
    }

    if (((int*)g_CombatWeapon_0046ae60)[0x2a] != 0) {
        int* vtbl60b = (int*)*(int*)g_CombatWeapon_0046ae60;
        ((void (__fastcall *)(int*, int))vtbl60b[4])((int*)g_CombatWeapon_0046ae60, 0);
        if (target != 0) {
            ((Target*)target)->UpdateProgress(1);
        }
    }

    RenderBackground();
}
