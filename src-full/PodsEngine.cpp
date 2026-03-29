#include "PodsEngine.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include "HashTable.h"
#include <string.h>

extern "C" extern void* DAT_0046bf30;  // Palette* for pods

/* Function start: 0x440860 */
PodsEngine::PodsEngine()
{
    field_0xFC = 0;
    field_0x100 = 0;
    memset(&podsBgSprite, 0, 10 * sizeof(int));
    podState.x = 0;
    podState.y = 3;
}

/* Function start: 0x440950 */
PodsEngine::~PodsEngine()
{
    if (DAT_0046bf30 != 0) {
        delete (Palette*)DAT_0046bf30;
        DAT_0046bf30 = 0;
    }
    if (podsBgSprite != 0) {
        delete podsBgSprite;
        podsBgSprite = 0;
    }
    if (hashTable != 0) {
        delete (HashTable*)hashTable;
        hashTable = 0;
    }
}

/* Function start: 0x440F30 */
void StartScheduleTimer() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    EnqueueSpriteAction(&action);
}
