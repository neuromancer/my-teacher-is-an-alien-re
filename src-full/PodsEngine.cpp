#include "PodsEngine.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include <string.h>

extern "C" extern void* DAT_0046bf30;  // Palette* for pods

/* Function start: 0x440860 */
PodsEngine::PodsEngine()
{
    field_0xFC = 0;
    field_0x100 = 0;
    memset(&bgSprite, 0, 10 * sizeof(int));
    field_0xF4.field_0 = 0;
    field_0xF4.field_4 = 3;
}

/* Function start: 0x440950 */
PodsEngine::~PodsEngine()
{
    if (DAT_0046bf30 != 0) {
        delete (Palette*)DAT_0046bf30;
        DAT_0046bf30 = 0;
    }
    if (bgSprite != 0) {
        bgSprite->~Sprite();
        FreeMemory(bgSprite);
        bgSprite = 0;
    }
    if (field_0x108 != 0) {
        // Object at field_0x108 has some destructor called via funclet
        // Using generic cleanup pattern
        void* obj = (void*)field_0x108;
        FreeMemory(obj);
        field_0x108 = 0;
    }
}

/* Function start: 0x440F30 */
void StartScheduleTimer() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    EnqueueSpriteAction(&action);
}
