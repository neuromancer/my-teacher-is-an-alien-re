#include "ZBufferManager.h"

extern "C" {
    void* __fastcall FUN_0041b760_impl(void*);
    void __fastcall FUN_0041b8e0_impl(void*);
}

/* Function start: 0x41B760 */
ZBufferManager* ZBufferManager::Init() {
    FUN_0041b760_impl(this);
    return this;
}

/* Function start: 0x41B8E0 */
void ZBufferManager::Cleanup() {
    FUN_0041b8e0_impl(this);
}
