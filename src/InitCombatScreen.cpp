#include "SC_CombatBase.h"
#include "Viewport.h"
#include "Palette.h"
#include "Sprite.h"
#include "Animation.h"
#include "Sample.h"
#include "globals.h"
#include "VBuffer.h"

extern void BlankScreen();

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    SC_CombatBase* engine = (SC_CombatBase*)self;

    engine->combatFlags = 0;

    g_Viewport_0046ae54->SetDimensions(
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x98),
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x9c));

    Sprite* navSpr = *(Sprite**)((char*)g_Navigator_0046ae70 + 0xa0);
    Animation* anim = navSpr->animation_data;
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
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x90),
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x94));

    *(int*)((int)g_ScoreDisplay_0046ae6c + 4) = 100;
    engine->spriteFrameCount = 1;
    engine->frameCount = 1;
    BlankScreen();

    {
        unsigned int palStart = *(unsigned int*)((char*)g_WeaponParser_0046ae4c + 0xa0);
        int palEnd = *(int*)((char*)g_WeaponParser_0046ae4c + 0xa4);
        g_Palette_0046ae64->SetPalette(palStart, palEnd - palStart + 1);
    }

    if (engine->backgroundSound != 0) {
        engine->backgroundSound->Play(100, 0);
    }
}
