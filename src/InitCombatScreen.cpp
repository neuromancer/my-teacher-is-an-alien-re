#include "SC_CombatBase.h"
#include "Viewport.h"
#include "Palette.h"
#include "Sprite.h"
#include "Animation.h"
#include "Sample.h"
#include "EngineSubsystems.h"
#include "mCNavigator.h"
#include "ScoreDisplay.h"
#include "globals.h"
#include "VBuffer.h"

extern void BlankScreen();

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    SC_CombatBase* engine = (SC_CombatBase*)self;

    engine->combatFlags = 0;

    g_Viewport_0046ae54->SetDimensions(
        g_WeaponParser_0046ae4c->dimensions.x,
        g_WeaponParser_0046ae4c->dimensions.y);

    Sprite* navSpr = g_Navigator_0046ae70->sprite;
    Animation* anim = navSpr->animation_data;
    int fh = (anim != 0) ? anim->targetBuffer->height : 0;
    int fw = (anim != 0) ? anim->targetBuffer->width : 0;
    fh -= g_Viewport_0046ae54->dim.b;
    fw -= g_Viewport_0046ae54->dim.a;
    g_Viewport_0046ae54->SetDimensions2(fw, fh);

    g_Viewport_0046ae54->SetCenter();
    g_Viewport_0046ae54->SetAnchor(
        g_WeaponParser_0046ae4c->anchor.x,
        g_WeaponParser_0046ae4c->anchor.y);

    g_ScoreDisplay_0046ae6c->scoreInitial = 100;
    engine->spriteFrameCount = 1;
    engine->frameCount = 1;
    BlankScreen();

    {
        unsigned int palStart = g_WeaponParser_0046ae4c->paletteStart.x;
        int palEnd = g_WeaponParser_0046ae4c->paletteStart.y;
        g_Palette_0046ae64->SetPalette(palStart, palEnd - palStart + 1);
    }

    if (engine->backgroundSound != 0) {
        engine->backgroundSound->Play(100, 0);
    }
}
