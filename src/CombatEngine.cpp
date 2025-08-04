#include "CombatEngine.h"
#include <stdio.h>
#include "AnimatedAsset.h"

extern void* g_TextManager;

extern "C" {
    void FUN_00422a2f(int, int);
    void FUN_004229ea(int, int);
}

/* Function start: 0x411D60 */
void CombatEngine::DisplayFrameRate()
{
    char local_80[128];

    FUN_00422a2f(0xff, 0xff);
    FUN_004229ea(0, 199);
    sprintf(local_80, "frames: a=%d l=%d", this->field_0xd4, this->field_0xdc);
    if (g_TextManager != (void*)0x0) {
        ((AnimatedAsset*)g_TextManager)->RenderText(local_80, -1);
    }
}
