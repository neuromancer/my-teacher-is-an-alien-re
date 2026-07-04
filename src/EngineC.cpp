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
#include "string.h"


/* Function start: 0x40BBF0 */
EngineC::EngineC()
{
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

    if (g_ScoreDisplay_0046ae6c->hitCount != hitCounter.x) {
        hitCounter.x++;
        int atLimit = (hitCounter.y != 0 && hitCounter.x >= hitCounter.y);
        if (atLimit) {
            g_ActiveCombat_00468a1c->statusPtr[0] = 1;
        } else {
            // Original bug at 0x40BDC0: zero hitCounter.y falls through to this divide.
            value = (hitCounter.x * 6) / hitCounter.y;
            if (value >= 0) {
                frame = 5;
                if (value <= 5) {
                    frame = value;
                }
            } else {
                frame = 0;
            }
            sprite104->ResetAnimation(frame, 0);
        }
    }

    if (missCounter.x != g_ScoreDisplay_0046ae6c->missCount) {
        missCounter.x = g_ScoreDisplay_0046ae6c->missCount;
    }

    if (g_CombatEngine_0046ae78->currentFrame >= 0x68b) {
        sprite118->Do(sprite118->loc.x, sprite118->loc.y, 1.0);

        if (g_CombatWeapon_0046ae60->m_clicked != 0) {
            mouse = g_InputManager_0046aa08->pMouse;
            if (mouse != 0 && mouse->y >= 0x78) {
                goto draw_engine_sprites;
            }
            mouseY = (mouse != 0) ? mouse->y : 0;
            mouseX = (mouse != 0) ? mouse->x : 0;

            if (sprite118->animation_data->targetBuffer->CheckHit(mouseX, mouseY) == 0xfa) {
                shotCounter.x++;
                int atLimit2 = (shotCounter.y != 0 && shotCounter.x >= shotCounter.y);
                if (atLimit2) {
                    g_ActiveCombat_00468a1c->statusPtr[1] = 1;
                } else {
                    // Original bug at 0x40BDC0: zero shotCounter.y falls through to this divide.
                    value = (shotCounter.x * 6) / shotCounter.y;
                    if (value >= 0) {
                        frame = 5;
                        if (value <= 5) {
                            frame = value;
                        }
                    } else {
                        frame = 0;
                    }
                    sprite110->ResetAnimation(frame, 0);
                }

                mouse = g_InputManager_0046aa08->pMouse;
                mouseY = (mouse == 0) ? 0 : mouse->y;
                mouseX = (mouse == 0) ? 0 : mouse->x;
                {
                    Sprite* s = sprite114;
                    s->loc.x = mouseX - 0x48;
                    s->loc.y = mouseY - 0x41;
                }
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

    {
        Sprite* bg = g_BgSprite_0046ae50;
        if (bg->Do(bg->loc.x, bg->loc.y, 1.0) != 0) {
            mouse = g_InputManager_0046aa08->pMouse;
            mouseX = (mouse == 0) ? 0 : mouse->x;
            g_BgSprite_0046ae50->ResetAnimation(
                mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
        }
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
        // Original bug at 0x40C200: DAT_0046aa08 is dereferenced after only the earlier Refresh guard.
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
        hitCounter.x = 0;
        hitCounter.y = value;
    } else if (strcmp(token, "NUM_ENGINE_SHOTS") == 0) {
        sscanf(line, " %s %d", token, &value);
        shotCounter.x = 0;
        shotCounter.y = value;
    } else if (strcmp(token, "END_DERIVED_ENGINE_INFO") == 0) {
        return 1;
    } else {
        SC_CombatBase::LBLParse(line);
    }
    return 0;
}
