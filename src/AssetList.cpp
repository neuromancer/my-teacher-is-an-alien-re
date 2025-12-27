#include "AssetList.h"

extern "C" {
    void __fastcall FUN_00404230_impl(void*);
}

/* Function start: 0x404230 */
void AssetList::Cleanup() {
    FUN_00404230_impl(this);
}
