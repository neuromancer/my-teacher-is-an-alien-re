#include "EngineC.h"
#include "stubs.h"
#include "Sprite.h"
#include "Memory.h"
#include "InputManager.h"
#include "RockThrower.h"
#include "Target.h"
#include "SoundList.h"
#include "ScoreDisplay.h"
#include "EngineSubsystems.h"
#include "VBuffer.h"
#include "SC_Combat.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

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

/* Function start: 0x40BDC0 */
void EngineC::RenderBackground() {
    void* pvVar1;
    int* piVar2;
    int iVar3;
    int bVar4;
    unsigned int uVar6;
    int iVar7;
    int iVar8;

    if (g_ScoreDisplay_0046ae6c->hitCount != *(int*)((char*)this + 0xfc)) {
        iVar7 = *(int*)((char*)this + 0xfc) + 1;
        iVar8 = *(int*)((char*)this + 0x100);
        *(int*)((char*)this + 0xfc) = iVar7;
        if (iVar8 == 0) {
            bVar4 = 0;
        } else {
            bVar4 = 1;
            if (iVar7 >= iVar8) {
            } else {
                bVar4 = 0;
            }
        }
        if (bVar4) {
            **(int**)((char*)g_ActiveCombat_00468a1c + 0xa8) = 1;
        } else {
            iVar8 = (iVar7 * 6) / iVar8;
            if (iVar8 >= 0) {
                iVar7 = 5;
                if (iVar8 <= 5) {
                    iVar7 = iVar8;
                }
            } else {
                iVar7 = 0;
            }
            ((Sprite*)*(void**)((char*)this + 0x104))->ResetAnimation(iVar7, 0);
        }
    }
    if (*(int*)((char*)this + 0xf4) != g_ScoreDisplay_0046ae6c->missCount) {
        *(int*)((char*)this + 0xf4) = g_ScoreDisplay_0046ae6c->missCount;
    }
    if (*(int*)((char*)g_CombatEngine_0046ae78 + 0xe4) < 0x68b) goto LAB_0040bff1;
    pvVar1 = *(void**)((char*)this + 0x118);
    ((Sprite*)pvVar1)->Do(*(int*)((char*)pvVar1 + 0xac), *(int*)((char*)pvVar1 + 0xb0), 1.0);
    if (*(int*)((char*)g_CombatWeapon_0046ae60 + 0xa8) != 0) {
        piVar2 = (int*)*(void**)((char*)g_InputManager_0046aa08 + 0x1a0);
        if (piVar2 == 0) {
LAB_0040beb1:
            iVar8 = 0;
        } else {
            if (0x77 < piVar2[1]) goto LAB_0040bf9e;
            if (piVar2 == 0) goto LAB_0040beb1;
            iVar8 = piVar2[1];
        }
        if (piVar2 == 0) {
            iVar7 = 0;
        } else {
            iVar7 = *piVar2;
        }
        uVar6 = ((VBuffer*)*(void**)(*(int*)((char*)*(void**)((char*)this + 0x118) + 0xf0) + 0x18))->
            CheckHit(iVar7, iVar8);
        if (uVar6 == 0xfa) {
            iVar8 = *(int*)((char*)this + 0x10c);
            iVar7 = *(int*)((char*)this + 0x108) + 1;
            *(int*)((char*)this + 0x108) = iVar7;
            if (iVar8 == 0) {
                bVar4 = 0;
            } else {
                bVar4 = 1;
                if (iVar7 >= iVar8) {
                } else {
                    bVar4 = 0;
                }
            }
            if (bVar4) {
                *(int*)(*(int*)((char*)g_ActiveCombat_00468a1c + 0xa8) + 4) = 1;
            } else {
                iVar8 = (iVar7 * 6) / iVar8;
                if (iVar8 >= 0) {
                    iVar7 = 5;
                    if (iVar8 <= 5) {
                        iVar7 = iVar8;
                    }
                } else {
                    iVar7 = 0;
                }
                ((Sprite*)*(void**)((char*)this + 0x110))->ResetAnimation(iVar7, 0);
            }
            iVar8 = 0;
            piVar2 = (int*)*(void**)((char*)g_InputManager_0046aa08 + 0x1a0);
            if (piVar2 != 0) {
                iVar8 = piVar2[1];
            }
            iVar7 = 0;
            if (piVar2 != 0) {
                iVar7 = *piVar2;
            }
            iVar3 = *(int*)((char*)this + 0x114);
            *(int*)(iVar3 + 0xac) = iVar7 - 0x48;
            *(int*)(iVar3 + 0xb0) = iVar8 - 0x41;
            ((Sprite*)*(void**)((char*)this + 0x114))->ResetAnimation(0, 0);
            ((SoundList*)*(void**)((char*)g_ActiveCombat_00468a1c + 0x110))->Play(0);
        }
    }

LAB_0040bf9e:
    pvVar1 = *(void**)((char*)this + 0x114);
    if (((Sprite*)pvVar1)->Do(*(int*)((char*)pvVar1 + 0xac), *(int*)((char*)pvVar1 + 0xb0), 1.0) != 0) {
        ((Sprite*)*(void**)((char*)this + 0x114))->ResetAnimation(-1, 0);
    }
    pvVar1 = *(void**)((char*)this + 0x110);
    ((Sprite*)pvVar1)->Do(*(int*)((char*)pvVar1 + 0xac), *(int*)((char*)pvVar1 + 0xb0), 1.0);

LAB_0040bff1:
    pvVar1 = *(void**)((char*)this + 0x104);
    ((Sprite*)pvVar1)->Do(*(int*)((char*)pvVar1 + 0xac), *(int*)((char*)pvVar1 + 0xb0), 1.0);
    if (*(int*)((char*)g_CombatWeapon_0046ae60 + 0xa8) != 0) {
        iVar8 = 0;
        if (*(int**)((char*)g_InputManager_0046aa08 + 0x1a0) != 0) {
            iVar8 = **(int**)((char*)g_InputManager_0046aa08 + 0x1a0);
        }
        g_BgSprite_0046ae50->ResetAnimation(
            iVar8 / (*(int*)((char*)g_WeaponParser_0046ae4c + 0x98) / 5) + 5, 0);
    }
    if (g_BgSprite_0046ae50->Do(*(int*)((char*)g_BgSprite_0046ae50 + 0xac),
                                 *(int*)((char*)g_BgSprite_0046ae50 + 0xb0), 1.0) != 0) {
        iVar8 = 0;
        if (*(int**)((char*)g_InputManager_0046aa08 + 0x1a0) != 0) {
            iVar8 = **(int**)((char*)g_InputManager_0046aa08 + 0x1a0);
        }
        g_BgSprite_0046ae50->ResetAnimation(
            iVar8 / (*(int*)((char*)g_WeaponParser_0046ae4c + 0x98) / 5), 0);
    }
}

/* Function start: 0x40C0D0 */
void EngineC::ProcessFrame()
{
    Target* target = (Target*)g_TargetList_0046ae58->ProcessTargets();

    g_CombatWeapon_0046ae60->DrawCrosshairs();

    int buttonDown;
    if (g_InputManager_0046aa08->pMouse != 0) {
        buttonDown = g_InputManager_0046aa08->pMouse->buttons & 1;
    } else {
        buttonDown = 0;
    }

    mouseReleased |= (buttonDown == 0) ? 1 : 0;
    g_CombatWeapon_0046ae60->m_clicked = 0;

    if (g_BgSprite_0046ae50->handle <= 4) {
        if (g_InputManager_0046aa08->pMouse != 0) {
            buttonDown = g_InputManager_0046aa08->pMouse->buttons & 1;
        } else {
            buttonDown = 0;
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

/* Function start: 0x40C1B0 */
int EngineC::HandleAction(int* param) {
    TODO("EngineC::HandleAction");
    return 0;
}

/* Function start: 0x40C200 */
void EngineC::OnProcessEnd() {
    TODO("EngineC::OnProcessEnd");
}

/* Function start: 0x40C270 */
int EngineC::LBLParse(char* line) {
    char token[32];
    int value;

    value = 0;
    sscanf(line, "%s", token);

    if (strcmp(token, "HEALTH_SPRITE") == 0) {
        sprite104 = new Sprite((char*)0);
        Parser::ProcessFile(sprite104, this, (char*)0);
    } else if (strcmp(token, "ENGINE_SPRITE") == 0) {
        sprite110 = new Sprite((char*)0);
        Parser::ProcessFile(sprite110, this, (char*)0);
    } else if (strcmp(token, "ENG_HIT_SPRITE") == 0) {
        sprite114 = new Sprite((char*)0);
        Parser::ProcessFile(sprite114, this, (char*)0);
    } else if (strcmp(token, "MASK_SPRITE") == 0) {
        sprite118 = new Sprite((char*)0);
        Parser::ProcessFile(sprite118, this, (char*)0);
    } else if (strcmp(token, "MAX_NUM_HITS") == 0) {
        sscanf(line, " %s %d", token, &value);
        field_0xFC = 0;
        field_0x100 = value;
    } else if (strcmp(token, "NUM_ENGINE_SHOTS") == 0) {
        sscanf(line, " %s %d", token, &value);
        field_0x108 = 0;
        field_0x10C = value;
    } else if (strcmp(token, "END_DERIVED_ENGINE_INFO") == 0) {
        return 1;
    } else {
        SC_CombatBase::LBLParse(line);
    }
    return 0;
}
