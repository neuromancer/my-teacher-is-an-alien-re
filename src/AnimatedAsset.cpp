#include "AnimatedAsset.h"
#include "Animation.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

extern Animation* Animation_Ctor(Animation*);

extern VBuffer* g_WorkBuffer;

extern "C" {
    void FUN_0041ae60(VBuffer*, int, int, int, int, int, int, void*);
    void FUN_0041aea0(VBuffer*, int, int, int, int, int, int, void*, char, char);
    void FUN_0041ad50(void*, int, int, int, int, int, int, char, char);
    // ...existing code...
    void FUN_00405770(void*);
    void FUN_00424b00(void*, int, int, void*, void*);
    int FUN_0041ac30(void*);
    void FUN_0041ac40(void);
    extern int DAT_004374c2;
    extern int DAT_004374ce;
    extern signed char DAT_004374c0;
    extern signed char DAT_004374c1;
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
        this->field_0x10 = 2;
        this->field_0x12 = 1;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

/* Function start: 0x421260 */
void AnimatedAsset::BuildGlyphTable()
{
    int glyphCount = this->field_0x20;
    int *alloc = (int*)AllocateMemory(glyphCount * 0x10 + 4);
    int *table = (int*)0x0;

    if (alloc != (int*)0x0) {
        table = alloc + 1; // skip length word
        *alloc = glyphCount;
        // initialize memory for each glyph entry (size 0x10)
        //FUN_00424b00(table, 0x10, glyphCount, (void*)0x4213e0, (void*)AnimatedAsset_CleanupGlyphEntry);
    }

    this->field_0x14 = table;

    if (table != (int*)0x0) {
        // zero all ints in the table (glyphCount * 4 ints per glyph)
        int totalInts = glyphCount * 4;
        for (int i = 0; i < totalInts; i++) {
            table[i] = 0;
        }
    }

    // Prepare indices and find start column
    void* srcPtr = *(void**)this; /* treat the object's first dword as a pointer used by the original code */
    int baseColMinus1 = *(int*)((char*)srcPtr + 0x18) - 1;
    int baseIndex = FUN_0041ac30(srcPtr);

    this->field_0x2c = 1;

    if (1 < *(int*)((char*)srcPtr + 0x18)) {
        while (this->field_0x2c < *(int*)((char*)srcPtr + 0x18)) {
            int idx = (baseColMinus1 - this->field_0x2c) * *(int*)((char*)srcPtr + 0x14) + baseIndex;
            if (*(char*)(idx) == this->field_0x12) break;
            this->field_0x2c++;
        }
    }

    int *entryPtr = (int*)this->field_0x14;
    int col = 0;
    int offset = 0;

    if (0 < *(int *)(*(int*)this + 0x18)) {
        while (offset < *(int *)(*(int*)this + 0x18)) {
            int x = 0;
            int startX = 0;
            int width = *(int *)(*(int*)this + 0x14);
            if (0 < width) {
                int tableOffset = col << 4;
                do {
                    int chIdx = x + (baseColMinus1 - offset) * width + baseIndex;
                    if (*(char*)(chIdx) == this->field_0x12) {
                        entryPtr[0] = startX;
                        entryPtr[2] = x - 1;
                        entryPtr[1] = offset;
                        entryPtr[3] = this->field_0x2c + offset - 1;
                        if (this->field_0x20 - col == 1) goto BUILT;
                        startX = x + 1;
                        col = col + 1;
                        tableOffset = tableOffset + 0x10;
                        entryPtr = (int*)((char*)this->field_0x14 + tableOffset);
                    }
                    x++;
                } while (x < width);
            }
            offset = offset + this->field_0x2c + 1;
        }
    }
BUILT:
    FUN_0041ac40();
}

/* Function start: 0x4215A0 */
int AnimatedAsset::ComputeTextMetrics(void* text)
{
    if (text == (void*)0x0) {
        return 0;
    }

    char *pch = (char*)text;
    int total = 0;

    while (*pch != '\0') {
        unsigned char c = *pch;
        if (c == 0x20) {
            total += this->field_0x24;
        } else if (c == 9) {
            total += this->field_0x28;
        } else {
            int idx = c - this->field_0x4;
            if (idx >= 0 && idx < this->field_0x20 && this->field_0x14 != NULL) {
                int *entry = (int*)((char*)this->field_0x14 + idx * 0x10);
                int startX = entry[0];
                int endX = entry[2];
                int width = endX - startX;
                if (width < 0) width = 0;
                total += width;
                total += this->field_0x30; // inter-character advance
            }
        }
        pch++;
    }
    return total;
}


/* Function start: 0x421680 */
void AnimatedAsset::PrepareText(char* text)
{
    // Initialize base positions from helpers
    this->field_0x8 = this->FUN_004239de();
    this->field_0xc = this->FUN_004239d8();

    int mode = this->FUN_004239d0();

    if (mode == 0) {
        int w = this->ComputeTextMetrics(text);
        // center horizontally
        this->field_0x8 += (w / 2);
        return;
    }

    if (mode == 1) {
        int w = this->ComputeTextMetrics(text);
        // right align
        this->field_0x8 -= w;
        int vert = this->FUN_004239c8();
        if (vert == 0) {
            this->field_0xc += this->field_0x2c / 2;
        }
        else if (vert == 1) {
            this->field_0xc += this->field_0x2c;
        }
        return;
    }

    // default vertical alignment adjustments when mode not 0 or 1
    int vert = this->FUN_004239c8();
    if (vert == 0) {
        this->field_0xc += this->field_0x2c / 2;
    }
    else if (vert == 1) {
        this->field_0xc += this->field_0x2c;
    }
}

/* Function start: 0x4213F0 */
int AnimatedAsset::IsCharSupported(int ch)
{
    if (ch == 0x20) return 1;
    if (ch == 9) return 1;
    int delta = ch - this->field_0x4;
    if (delta < 0) return 0;
    if (delta > this->field_0x20 - 1) return 0;
    return 1;
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
    this->BuildGlyphTable();
    iVar2 = this->IsCharSupported(0x41);
    if (iVar2 != 0) {
      iVar2 = this->ComputeTextMetrics(&DAT_00435ef4);
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
    {
        int* glyphPtr = this->field_0x14;
        if (glyphPtr != (int *)0x0) {
            FreeMemory((int *)((char*)glyphPtr - 4));
            this->field_0x14 = 0;
        }
    }
    this->FUN_004210a8();
    this->FUN_004210bd();

}

/* Function start: 0x4210A8 */
void AnimatedAsset::FUN_004210a8()
{
    /* tail-jump wrapper in binary: MOV ECX,[EBP-0x10]; ADD ECX,0x30; JMP 0x00405770 */
    FUN_00405770((void*)((char*)this + 0x30));
}

/* Function start: 0x4210BD */
void AnimatedAsset::FUN_004210bd()
{
    /* tail-jump wrapper in binary: MOV ECX,[EBP-0x10]; ADD ECX,0x8; JMP 0x00405770 */
    FUN_00405770((void*)((char*)this + 0x8));
}

/* Function start: 0x4239DE */
int AnimatedAsset::FUN_004239de()
{
    /* MOV EAX,[0x004374c2] ; RET */
    return DAT_004374c2;
}

/* Function start: 0x4239D8 */
int AnimatedAsset::FUN_004239d8()
{
    /* MOV EAX,[0x004374ce] ; RET */
    return DAT_004374ce;
}

/* Function start: 0x4239D0 */
int AnimatedAsset::FUN_004239d0()
{
    /* MOVSX EAX,byte ptr [0x004374c0] ; RET */
    return (int)(signed char)DAT_004374c0;
}

/* Function start: 0x4239C8 */
int AnimatedAsset::FUN_004239c8()
{
    /* MOVSX EAX,byte ptr [0x004374c1] ; RET */
    return (int)(signed char)DAT_004374c1;
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
                    FUN_0041aea0(g_WorkBuffer, iVar3, iVar4, iVar2, iVar1, param_1, param_2, this, this->field_0x10, this->field_0x11);
                }
            }
            else if (this->field_0x1c == 0) {
                this->buffer->ClipAndBlit((int)g_WorkBuffer, iVar3, iVar4, iVar2, iVar1, param_1, param_2);
            }
            else {
                FUN_0041ad50(this, iVar3, iVar4, iVar2, iVar1, param_1, param_2, this->field_0x10, this->field_0x11);
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
        this->field_0x11 = (char)param_2;
        this->field_0x1c = (param_2 != -1);
        PrepareText(text);
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
