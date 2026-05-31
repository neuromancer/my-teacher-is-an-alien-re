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
#include "SpriteAction.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

extern "C" void ShowError(const char* format, ...);

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
    int value;
    int frame;
    int mouseX;
    int mouseY;
    InputState* mouse;

    if (g_ScoreDisplay_0046ae6c->hitCount != field_0xFC) {
        field_0xFC++;
        if (field_0x100 != 0 && field_0xFC >= field_0x100) {
            g_ActiveCombat_00468a1c->statusPtr[0] = 1;
        } else {
            value = (field_0xFC * 6) / field_0x100;
            if (value < 0) {
                frame = 0;
            } else {
                frame = 5;
                if (value <= 5) {
                    frame = value;
                }
            }
            sprite104->ResetAnimation(frame, 0);
        }
    }

    if (field_0xF4 != g_ScoreDisplay_0046ae6c->missCount) {
        field_0xF4 = g_ScoreDisplay_0046ae6c->missCount;
    }

    if (g_CombatEngine_0046ae78->currentFrame >= 0x68b) {
        sprite118->Do(sprite118->loc.x, sprite118->loc.y, 1.0);

        if (g_CombatWeapon_0046ae60->m_clicked != 0) {
            mouse = g_InputManager_0046aa08->pMouse;
            if (mouse == 0) {
                mouseY = 0;
                mouseX = 0;
            } else {
                if (mouse->y > 0x77) {
                    goto draw_engine_sprites;
                }
                mouseY = mouse->y;
                mouseX = mouse->x;
            }

            if (sprite118->animation_data->targetBuffer->CheckHit(mouseX, mouseY) == 0xfa) {
                field_0x108++;
                if (field_0x10C != 0 && field_0x108 >= field_0x10C) {
                    g_ActiveCombat_00468a1c->statusPtr[1] = 1;
                } else {
                    value = (field_0x108 * 6) / field_0x10C;
                    if (value < 0) {
                        frame = 0;
                    } else {
                        frame = 5;
                        if (value <= 5) {
                            frame = value;
                        }
                    }
                    sprite110->ResetAnimation(frame, 0);
                }

                mouse = g_InputManager_0046aa08->pMouse;
                if (mouse == 0) {
                    mouseY = 0;
                    mouseX = 0;
                } else {
                    mouseY = mouse->y;
                    mouseX = mouse->x;
                }
                sprite114->loc.x = mouseX - 0x48;
                sprite114->loc.y = mouseY - 0x41;
                sprite114->ResetAnimation(0, 0);
                g_ActiveCombat_00468a1c->bgSound->Play(0);
            }
        }

draw_engine_sprites:
        if (sprite114->Do(sprite114->loc.x, sprite114->loc.y, 1.0) != 0) {
            sprite114->ResetAnimation(-1, 0);
        }
        sprite110->Do(sprite110->loc.x, sprite110->loc.y, 1.0);
    }

    sprite104->Do(sprite104->loc.x, sprite104->loc.y, 1.0);

    if (g_CombatWeapon_0046ae60->m_clicked != 0) {
        mouse = g_InputManager_0046aa08->pMouse;
        mouseX = (mouse == 0) ? 0 : mouse->x;
        g_BgSprite_0046ae50->ResetAnimation(
            mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5) + 5, 0);
    }

    if (g_BgSprite_0046ae50->Do(g_BgSprite_0046ae50->loc.x, g_BgSprite_0046ae50->loc.y, 1.0) != 0) {
        mouse = g_InputManager_0046aa08->pMouse;
        mouseX = (mouse == 0) ? 0 : mouse->x;
        g_BgSprite_0046ae50->ResetAnimation(
            mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
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
    int result = 0;
    SpriteAction* action = (SpriteAction*)param;

    if (action->instruction != 0x37) {
        ShowError("EngineGauntlet::OnLogicMessage - unsupported instruction %d",
                  action->instruction);
    }

    if (action->extra1 == 1) {
        g_ActiveCombat_00468a1c->statusPtr[0] = 1;
        result = 1;
    }

    return result;
}

/* Function start: 0x40C200 */
void EngineC::OnProcessEnd() {
    int mouseX;
    InputState* mouse;

    if (g_InputManager_0046aa08 != 0) {
        g_InputManager_0046aa08->Refresh(1);
    }

    if (g_BgSprite_0046ae50 != 0) {
        mouse = g_InputManager_0046aa08->pMouse;
        mouseX = (mouse == 0) ? 0 : mouse->x;
        g_BgSprite_0046ae50->ResetAnimation(
            mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
    }

    mouseReleased = 1;
}

/* Function start: 0x40C270 */
int EngineC::LBLParse(char* line) {
    char token[32];
    int value;

    value = 0;
    sscanf(line, "%s", token);

    if (strcmp(token, "HEALTH_SPRITE") == 0) {
        sprite104 = new Sprite(0);
        Parser::ProcessFile(sprite104, this, 0);
    } else if (strcmp(token, "ENGINE_SPRITE") == 0) {
        sprite110 = new Sprite(0);
        Parser::ProcessFile(sprite110, this, 0);
    } else if (strcmp(token, "ENG_HIT_SPRITE") == 0) {
        sprite114 = new Sprite(0);
        Parser::ProcessFile(sprite114, this, 0);
    } else if (strcmp(token, "MASK_SPRITE") == 0) {
        sprite118 = new Sprite(0);
        Parser::ProcessFile(sprite118, this, 0);
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
