#include "AnimatedAsset.h"
#include "globals.h"
#include "Animation.h"
#include "VBuffer.h"
#include "Memory.h"
#include "Array.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>



extern "C" {
    // CallBlitter2/3 are VBuffer members: use VBuffer::CallBlitter2/CallBlitter3
    // ...existing code...
    void FUN_00405770(void*);
    // FUN_00424b00 is Array_Iterate in Array.h
    void FUN_004249d0(void*, int, int, void*);
    void FUN_0041ae0c(void);
}



/* Function start: 0x420F80 */
AnimatedAsset* AnimatedAsset::Init()
{
    __try {
        this->text_x = 0;
        this->text_y = 0;
        this->charAdvance = 0;
        this->reserved_34 = 0;
        memset(this, 0, 56);
        this->color = 2;
        this->glyphValue = 1;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return this;
}

/* Function start: 0x421260 */
void AnimatedAsset::BuildGlyphTable()
{
    int glyphCount = this->glyphCount;
    int *table = (int*)0x0;
    int *alloc = (int*)AllocateMemory(glyphCount * 0x10 + 4);

    __try {
        if (alloc != (int*)0x0) {
            table = alloc + 1;
            *alloc = glyphCount;
            Array_Iterate(table, 0x10, glyphCount, (void(*)(void*))0x4213e0, (void(*)(void*))0x401680);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }

    this->glyphTable = table;

    int count = this->glyphCount * 4;
    int *p = table;
    while (count != 0) {
        *p = 0;
        p++;
        count--;
    }

    VBuffer* buf = this->buffer;
    int baseColMinus1 = buf->height - 1;
    int baseIndex = (int)buf->GetData();
    buf = this->buffer;

    this->glyphHeight = 1;

    if (buf->height > 1) {
        char glyphVal = this->glyphValue;
        do {
            int idx = (baseColMinus1 - this->glyphHeight) * buf->width + baseIndex;
            if (*(char*)(idx) == glyphVal) break;
            this->glyphHeight = this->glyphHeight + 1;
        } while (this->glyphHeight < buf->height);
    }

    int col = 0;
    int *entryPtr = this->glyphTable;
    int offset = 0;

    if (buf->height > 0) {
        do {
            int x = 0;
            int startX = 0;
            int width = this->buffer->width;
            int chBase = (baseColMinus1 - offset) * width + baseIndex;

            if (width > 0) {
                int tableOffset = col << 4;
                char glyphVal = this->glyphValue;
                int chIdx = chBase;

                do {
                    if (*(char*)(x + chIdx) == glyphVal) {
                        entryPtr[0] = startX;
                        entryPtr[2] = x - 1;
                        entryPtr[1] = offset;
                        entryPtr[3] = this->glyphHeight + offset - 1;
                        if (this->glyphCount - col == 1) goto BUILT;
                        startX = x + 1;
                        col++;
                        tableOffset += 0x10;
                        entryPtr = (int*)((char*)this->glyphTable + tableOffset);
                    }
                    x++;
                } while (x < this->buffer->width);
            }

            offset = offset + this->glyphHeight + 1;
        } while (offset < this->buffer->height);
    }
BUILT:
    this->buffer->Lock();
}

extern "C" {
    void FUN_00421671(void);
}

/* Function start: 0x4215A0 */
int AnimatedAsset::ComputeTextMetrics(void* text)
{
    int total = 0;
    char* param_1 = (char*)text;

    if (param_1 == (char*)0x0) {
        total = 0;
    }
    else {
        char cVar1 = *param_1;
        while (cVar1 != '\0') {
            int iVar2 = (int)*param_1;
            if (iVar2 == 0x20) {
                total = total + this->spaceWidth;
            }
            else if (iVar2 == 9) {
                total = total + this->tabWidth;
            }
            else {
                int* piVar3 = (int*)((char*)this->glyphTable + iVar2 * 0x10);
                int local[4];
                __try {
                    local[0] = piVar3[0];
                    local[1] = piVar3[1];
                    local[2] = piVar3[2];
                    local[3] = piVar3[3];
                } __finally {
                    FUN_00421671();
                }
                total = (total - local[0]) + local[2];
            }
            param_1 = param_1 + 1;
            total = total + this->charAdvance;
            cVar1 = *param_1;
        }
    }
    return total;
}


/* Function start: 0x421680 */
void AnimatedAsset::PrepareText(char* text)
{
    int w;
    int vert;

    // Initialize base positions from helpers
    this->text_x = this->FUN_004239de();
    this->text_y = this->FUN_004239d8();

    int mode = this->FUN_004239d0();

    if (mode == 0) {
        w = this->ComputeTextMetrics(text);
        w = w / 2;
        this->text_x -= w;
    }
    else if (mode == 1) {
        w = this->ComputeTextMetrics(text);
        this->text_x -= w;
    }
    
    // vertical alignment for all modes
    vert = this->FUN_004239c8();
    if (vert == 0) {
        this->text_y += this->glyphHeight / 2;
        return;
    }
    if (vert == 1) {
        this->text_y += this->glyphHeight;
    }
}

/* Function start: 0x4213F0 */
int AnimatedAsset::IsCharSupported(int ch)
{
    if (ch != 0x20 && ch != 9) {
        int delta = ch - this->firstChar;
        if (delta < 0 || this->glyphCount - 1 < delta) {
            return 0;
        }
        return 1;
    }
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
    iVar2 = sscanf(param_1, "%s %d %d", local_9c, &this->firstChar, &local_18);
    if (iVar2 <= 2) {
      local_18 = 0x7e;
    }
    if (iVar2 <= 1) {
      this->firstChar = 0x21;
    }
    pVVar3 = this->buffer;
    this->glyphCount = (local_18 - this->firstChar) + 1;
    if (pVVar3 != (VBuffer *)0x0) {
      pVVar3->~VBuffer();
      FreeMemory(pVVar3);
      this->buffer = 0;
    }
    local_1c = (Animation *)AllocateMemory(sizeof(Animation));
    this_00 = (Animation *)0x0;
    if (local_1c != (Animation *)0x0) {
      this_00 = local_1c->Init();
    }
    this_00->Open(local_9c,0xfe000,0xffffffff);
    local_14 = (VBuffer *)AllocateMemory(sizeof(VBuffer));
    pVVar3 = (VBuffer *)0x0;
    if (local_14 != (VBuffer *)0x0) {
      pVVar3 = local_14->CreateAndClean(this_00->smk->Width, this_00->smk->Height);
    }
    this->buffer = pVVar3;
    this_00->ToBufferVB(pVVar3);
    this_00->DoFrame();
    if (this_00 != (Animation *)0x0) {
        this_00->Delete(1);
    }
    this->BuildGlyphTable();
    iVar2 = this->IsCharSupported(0x41);
    if (iVar2 != 0) {
      iVar2 = this->ComputeTextMetrics(&DAT_00435ef4);
      iVar2 = (iVar2 * 2) / 3;
      this->spaceWidth = iVar2;
      this->tabWidth = iVar2 << 2;
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
        int* glyphPtr = this->glyphTable;
        if (glyphPtr != (int *)0x0) {
            int* countPtr = (int*)((char*)glyphPtr - 4);
            FUN_004249d0(glyphPtr, 0x10, *countPtr, (void*)0x401680);
            FreeMemory(countPtr);
            this->glyphTable = 0;
        }
    }
    this->FUN_004210a8();
    this->FUN_004210bd();

}

/* Function start: 0x4210A8 */
void AnimatedAsset::FUN_004210a8()
{
    /* tail-jump wrapper in binary: MOV ECX,[EBP-0x10]; ADD ECX,0x30; JMP 0x00405770 */
    FUN_00405770((void*)&this->charAdvance);
}

/* Function start: 0x4210BD */
void AnimatedAsset::FUN_004210bd()
{
    /* tail-jump wrapper in binary: MOV ECX,[EBP-0x10]; ADD ECX,0x8; JMP 0x00405770 */
    FUN_00405770((void*)&this->text_x);
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
            iVar4 = this->spaceWidth;
        }
        else if (param_3 == 9) {
            iVar4 = this->tabWidth;
        }
        else {
            piVar5 = (int*)((param_3 - this->firstChar) * 0x10 + this->glyphTable);
            iVar1 = piVar5[3];
            iVar4 = piVar5[2];
            iVar2 = piVar5[1];
            iVar3 = *piVar5;
            if (this->useBuffer == 0) {
                if (this->useAttr == 0) {
                    g_WorkBuffer_00436974->CallBlitter2(iVar3, iVar4, iVar2, iVar1, param_1, param_2, this->buffer);
                }
                else {
                    g_WorkBuffer_00436974->CallBlitter3(iVar3, iVar4, iVar2, iVar1, param_1, param_2, this->buffer, this->color, this->attr);
                }
            }
            else if (this->useAttr == 0) {
                this->buffer->ClipAndBlit((int)g_WorkBuffer_00436974, iVar3, iVar4, iVar2, iVar1, param_1, param_2);
            }
            else {
                this->buffer->BlitTransparent(iVar3, iVar4, iVar2, iVar1, param_1, param_2, this->color, this->attr);
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
        this->useBuffer = 0;
        this->attr = (char)param_2;
        this->useAttr = (param_2 != -1);
        PrepareText(text);
        if (*text != '\0') {
            int* piVar1 = &this->text_x;
            do {
                char cVar2 = *text;
                text = text + 1;
                int iVar3 = this->DrawChar(*piVar1, this->text_y, (int)cVar2);
                *piVar1 = *piVar1 + this->charAdvance + iVar3;
            } while (*text != '\0');
        }
    }
}
