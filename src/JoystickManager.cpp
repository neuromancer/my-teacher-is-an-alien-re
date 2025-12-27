#include <windows.h>
#include "JoystickManager.h"

extern "C" void FUN_00421890(void*, int);

/* Function start: 0x4217C0 */
JoystickManager* JoystickManager::Init(int param_1) {
    __try {
        this->field_1a4 = 0;
        this->field_1a8 = 0;
        this->field_1ac = 0;
        this->field_1b0 = 0;
        FUN_00421890(this, param_1);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return this;
}
