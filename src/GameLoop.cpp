#include "GameLoop.h"

extern "C" {
    void* __fastcall FUN_00417200_impl(void*);
    void __fastcall FUN_00417690_impl(void*);
    void __fastcall FUN_00417320_impl(void*);
}

/* Function start: 0x417200 */
GameLoop* GameLoop::Init() {
    FUN_00417200_impl(this);
    return this;
}

/* Function start: 0x417690 */
void GameLoop::Run() {
    FUN_00417690_impl(this);
}

/* Function start: 0x417320 */
void GameLoop::Cleanup() {
    FUN_00417320_impl(this);
}
