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
extern "C" void OgdenTrace(const char* format, ...);

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    SC_CombatBase* engine = (SC_CombatBase*)self;

    OgdenTrace("[OGDEN] InitCombatScreen begin engine=%08lx viewport=%08lx weapon=%08lx navigator=%08lx score=%08lx palette=%08lx bgSound=%08lx",
        (unsigned long)engine,
        (unsigned long)g_Viewport_0046ae54,
        (unsigned long)g_WeaponParser_0046ae4c,
        (unsigned long)g_Navigator_0046ae70,
        (unsigned long)g_ScoreDisplay_0046ae6c,
        (unsigned long)g_Palette_0046ae64,
        engine == 0 ? 0 : (unsigned long)engine->backgroundSound);
    engine->combatFlags = 0;

    OgdenTrace("[OGDEN] InitCombatScreen set viewport dimensions %d,%d",
        g_WeaponParser_0046ae4c->dimensions.x,
        g_WeaponParser_0046ae4c->dimensions.y);
    g_Viewport_0046ae54->SetDimensions(
        g_WeaponParser_0046ae4c->dimensions.x,
        g_WeaponParser_0046ae4c->dimensions.y);

    Sprite* navSpr = g_Navigator_0046ae70->sprite;
    Animation* anim = navSpr->animation_data;
    OgdenTrace("[OGDEN] InitCombatScreen navigator sprite=%08lx anim=%08lx",
        (unsigned long)navSpr,
        (unsigned long)anim);
    int fh = 0;
    if (anim != 0) {
        fh = anim->targetBuffer->height;
    }
    int fw = 0;
    if (anim != 0) {
        fw = anim->targetBuffer->width;
    }
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
        OgdenTrace("[OGDEN] InitCombatScreen set palette %lu,%d", palStart, palEnd);
        g_Palette_0046ae64->SetPalette(palStart, palEnd - palStart + 1);
    }

    if (engine->backgroundSound != 0) {
        OgdenTrace("[OGDEN] InitCombatScreen play background sound=%08lx", (unsigned long)engine->backgroundSound);
        engine->backgroundSound->Play(100, 0);
    }
    OgdenTrace("[OGDEN] InitCombatScreen end");
}
