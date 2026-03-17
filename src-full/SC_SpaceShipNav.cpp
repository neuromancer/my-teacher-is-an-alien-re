#include "SC_SpaceShipNav.h"
#include "Memory.h"

extern void __fastcall FUN_00447790(void*, int, int); // ShutDown(0)

/* Function start: 0x447520 */
SC_SpaceShipNav::SC_SpaceShipNav() {
}

/* Function start: 0x4475B0 */
SC_SpaceShipNav::~SC_SpaceShipNav() {
    FUN_00447790(this, 0, 0);

    if (field_118[0] != 0) {
        int* obj = (int*)field_118[0];
        int* vtbl = (int*)*obj;
        ((void (__fastcall *)(int*, int, int))vtbl[3])(obj, 0, 1);
        field_118[0] = 0;
    }
}
