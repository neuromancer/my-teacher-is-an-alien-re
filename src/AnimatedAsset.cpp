#include "AnimatedAsset.h"
#include "Animation.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

extern "C" {
    void FUN_00421260(int*);
    int FUN_004213f0(void*, int);
    int FUN_004215a0(void*, void*);
    extern void* DAT_00435ef4;
}

#include <new.h>

/* Function start: 0x420F80 */
AnimatedAsset::AnimatedAsset()
{
    __try {
        this->field_0x8 = 0;
        this->field_0xc = 0;
        this->field_0x30 = 0;
        this->field_0x34 = 0;
        memset(this, 0, 56);
        *(char*)((int)this + 0x10) = 2;
        *(char*)((int)this + 0x12) = 1;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

/* Function start: 0x4210D0 */
void AnimatedAsset::LoadAnimatedAsset(char *param_1)
{
  int iVar2;
  VBuffer *pVVar3;
  Animation *this_00;
  char local_9c [128];
  Animation *local_1c;
  int local_18;
  VBuffer *local_14;

  if (param_1 != (char *)0x0) {
    iVar2 = sscanf(param_1, "%s %d %d", local_9c, &this->field_0x4, &local_18);
    if (iVar2 < 3) {
      local_18 = 0x7e;
    }
    if (iVar2 < 2) {
      this->field_0x4 = 0x21;
    }
    pVVar3 = this->buffer;
    this->field_0x20 = (local_18 - this->field_0x4) + 1;
    if (pVVar3 != (VBuffer *)0x0) {
      pVVar3->~VBuffer();
      FreeMemory(pVVar3);
      this->buffer = 0;
    }
    local_1c = (Animation *)AllocateMemory(sizeof(Animation));
    this_00 = (Animation *)0x0;
    if (local_1c != (Animation *)0x0) {
      this_00 = new (local_1c) Animation();
    }
    this_00->Open(local_9c,0xfe000,0xffffffff);
    local_14 = (VBuffer *)AllocateMemory(sizeof(VBuffer));
    pVVar3 = (VBuffer *)0x0;
    if (local_14 != (VBuffer *)0x0) {
      pVVar3 = VirtualBufferCreateAndClean(local_14, this_00->smk->height, this_00->smk->width);
    }
    this->buffer = pVVar3;
    this_00->ToBuffer(pVVar3);
    Animation::DoFrame(this_00);
    if (this_00 != (Animation *)0x0) {
        this_00->~Animation();
        FreeMemory(this_00);
    }
    FUN_00421260((int *)this);
    iVar2 = FUN_004213f0(this,0x41);
    if (iVar2 != 0) {
      iVar2 = FUN_004215a0(this,&DAT_00435ef4);
      iVar2 = (iVar2 * 2) / 3;
      this->field_0x24 = iVar2;
      this->field_0x28 = iVar2 << 2;
    }
  }
}

/* Function start: 0x421010 */
AnimatedAsset::~AnimatedAsset()
{
    VBuffer *this_00;
    int iVar1;

    this_00 = this->buffer;
    if (this_00 != (VBuffer *)0x0) {
        this_00->~VBuffer();
        FreeMemory(this_00);
        this->buffer = 0;
    }
    iVar1 = this->field_0x20;
    if (iVar1 != 0) {
        // Array_Cleanup(iVar1,0x10,*(int *)(iVar1 + -4),(_func_void_void_ptr *)0x401680);
        FreeMemory((int *)(iVar1 + -4));
        this->field_0x20 = 0;
    }
    // FUN_004210a8();
    // FUN_004210bd();
}
