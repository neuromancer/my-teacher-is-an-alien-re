#include "SC_SpaceShipNav.h"
#include "SC_CombatBase.h"
#include "Memory.h"
#include "Sprite.h"
#include "mCNavigator.h"

extern int DAT_0046ae78;

/* Function start: 0x447520 */
SC_SpaceShipNav::SC_SpaceShipNav() {
}

/* Function start: 0x4475B0 */
SC_SpaceShipNav::~SC_SpaceShipNav() {
    ShutDown(0);

    if (field_118[0] != 0) {
        int* obj = (int*)field_118[0];
        int* vtbl = (int*)*obj;
        ((void (__fastcall *)(int*, int, int))vtbl[3])(obj, 0, 1);
        field_118[0] = 0;
    }
}

/* Function start: 0x447790 */
int SC_SpaceShipNav::ShutDown(SC_Message* msg) {
    if (DAT_0046ae78 != 0) {
        if (DAT_0046ae70 != 0) {
            Sprite* spr = ((mCNavigator*)DAT_0046ae70)->sprite;
            if (spr != 0) {
                spr->StopAnimationSound();
            }
        }
        ((SC_CombatBase*)DAT_0046ae78)->RenderState();
        DAT_0046ae78 = 0;
    }
    return SC_Combat::ShutDown(msg);
}
