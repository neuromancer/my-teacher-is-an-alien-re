#include "PodsEngine.h"
#include "stubs.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include "HashTable.h"
#include "globals.h"
#include <string.h>

/* Function start: 0x440860 */
PodsEngine::PodsEngine()
{
    reserved_0xFC = 0;
    reserved_0x100 = 0;
    memset(&podsBgSprite, 0, 10 * sizeof(int));
    podState.x = 0;
    podState.y = 3;
}

/* Function start: 0x440950 */
PodsEngine::~PodsEngine()
{
    if (g_PodsPalette_0046bf30 != 0) {
        delete g_PodsPalette_0046bf30;
        g_PodsPalette_0046bf30 = 0;
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

/* Function start: 0x440A50 */
void PodsEngine::ProcessFrame() {
    TODO("PodsEngine::ProcessFrame");
}

extern void __stdcall EnqueueHotspotAction(SpriteAction*);

/* Function start: 0x440F30 */
void StartScheduleTimer() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    EnqueueHotspotAction(&action);
}

/* Function start: 0x440FC0 */
int PodsEngine::HandleAction(int* param) {
    TODO("PodsEngine::HandleAction");
    return 0;
}

/* Function start: 0x441000 */
void PodsEngine::OnProcessEnd() {
    TODO("PodsEngine::OnProcessEnd");
}

/* Function start: 0x441140 */
int PodsEngine::LBLParse(char* line) {
    TODO("PodsEngine::LBLParse");
    return 0;
}
