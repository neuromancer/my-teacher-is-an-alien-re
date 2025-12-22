#include "AnimatedAsset.h"
#include "Animation.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

extern Animation* Animation_Ctor(Animation*);

extern VBuffer* g_WorkBuffer;

extern "C" {
    void FUN_00421260(int*);
    int FUN_004213f0(void*, int);
    int FUN_004215a0(void*, void*);
    void FUN_00421680(void*, char*);
    void FUN_0041ae60(VBuffer*, int, int, int, int, int, int, void*);
    void FUN_0041aea0(VBuffer*, int, int, int, int, int, int, void*, char, char);
    void FUN_0041ad50(void*, int, int, int, int, int, int, char, char);
    extern void* DAT_00435ef4;
}

// Constructor wrapper for external callers
AnimatedAsset* AnimatedAsset_Ctor(AnimatedAsset* p) {
    p->AnimatedAsset::AnimatedAsset();
    return p;
}

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
      this_00 = Animation_Ctor(local_1c);
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

/* Function start: 0x421420 */
int AnimatedAsset::DrawChar(int param_1, int param_2, int param_3)
{
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;
    int* piVar5;

    __try {
        if (param_3 == 0x20) {
            iVar4 = this->field_0x24;
        }
        else if (param_3 == 9) {
            iVar4 = this->field_0x28;
        }
        else {
            piVar5 = (int*)((param_3 - this->field_0x4) * 0x10 + this->field_0x14);
            iVar1 = piVar5[3];
            iVar4 = piVar5[2];
            iVar2 = piVar5[1];
            iVar3 = *piVar5;
            if (this->field_0x18 == 0) {
                if (this->field_0x1c == 0) {
                    FUN_0041ae60(g_WorkBuffer, iVar3, iVar4, iVar2, iVar1, param_1, param_2, this);
                }
                else {
                    FUN_0041aea0(g_WorkBuffer, iVar3, iVar4, iVar2, iVar1, param_1, param_2, this, this->field_0x10, *(char*)((int)this + 0x11));
                }
            }
            else if (this->field_0x1c == 0) {
                this->buffer->ClipAndBlit((int)g_WorkBuffer, iVar3, iVar4, iVar2, iVar1, param_1, param_2);
            }
            else {
                FUN_0041ad50(this, iVar3, iVar4, iVar2, iVar1, param_1, param_2, this->field_0x10, *(char*)((int)this + 0x11));
            }
            iVar4 = iVar4 - iVar3;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        iVar4 = 0;
    }
    return iVar4;
}

/* Function start: 0x421700 */
void AnimatedAsset::RenderText(char* text, int param_2)
{
    if (text != (char *)0x0) {
        this->field_0x18 = 0;
        *(char*)((int)this + 0x11) = (char)param_2;
        this->field_0x1c = (param_2 != -1);
        FUN_00421680(this, text);
        if (*text != '\0') {
            int* piVar1 = &this->field_0x8;
            do {
                char cVar2 = *text;
                text = text + 1;
                int iVar3 = this->DrawChar(*piVar1, this->field_0xc, (int)cVar2);
                *piVar1 = *piVar1 + this->field_0x30 + iVar3;
            } while (*text != '\0');
        }
    }
}
