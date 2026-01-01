#include "AnimatedAsset.h"
#include "globals.h"
#include "Animation.h"
#include "VBuffer.h"
#include "Memory.h"
#include "Array.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "Cleanup.h"

/* Function start: 0x420F80 */
AnimatedAsset::AnimatedAsset()
{
    char glyphVal = 1;
    int* text_xy = &this->text_pos.x;
    int* charAdv = &this->char_adv.advance;
    text_xy[0] = 0;
    text_xy[1] = 0;
    charAdv[0] = 0;
    charAdv[1] = 0;
    int count = 14;
    int* p = (int*)this;
    while (count != 0) {
        *p = 0;
        p++;
        count--;
    }
    this->color = 2;
    this->glyphValue = glyphVal;
}

/* Function start: 0x421260 */
void AnimatedAsset::BuildGlyphTable()
{
    int glyphCount = this->glyphCount;
    GlyphRect* table = new GlyphRect[glyphCount];

    int count = this->glyphCount << 4 >> 2;
    this->glyphTable = table;
    int* p = (int*)table;
    while (count != 0) {
        *p = 0;
        p++;
        count--;
    }

    int baseColMinus1 = this->buffer->height - 1;
    int baseIndex = (int)this->buffer->GetData();
    VBuffer* buf = this->buffer;
    
    this->glyphHeight = 1;

    if (buf->height > 1) {
        char glyphVal = this->glyphValue;
        int glyphH = this->glyphHeight;
        do {
            if (*(char*)((baseColMinus1 - glyphH) * buf->width + baseIndex) == glyphVal) break;
            glyphH++;
            this->glyphHeight = glyphH;
        } while (buf->height > glyphH);
    }

    int col = 0;
    GlyphRect* entryPtr = this->glyphTable;
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

                do {
                    if (*(char*)(x + chBase) == glyphVal) {
                        entryPtr->left = startX;
                        entryPtr->right = x - 1;
                        entryPtr->top = offset;
                        entryPtr->bottom = this->glyphHeight + offset - 1;
                        if (this->glyphCount - col == 1) goto BUILT;
                        startX = x + 1;
                        col++;
                        tableOffset += 0x10;
                        entryPtr = (GlyphRect*)((char*)this->glyphTable + tableOffset);
                    }
                    x++;
                } while (this->buffer->width > x);
            }

            offset = offset + this->glyphHeight + 1;
        } while (this->buffer->height > offset);
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
                GlyphRect* piVar3 = (GlyphRect*)((char*)this->glyphTable + iVar2 * 0x10);
                int local_left;
                int local_top;
                int local_right;
                int local_bottom;
                __try {
                    local_left = piVar3->left;
                    local_top = piVar3->top;
                    local_right = piVar3->right;
                    local_bottom = piVar3->bottom;
                } __finally {
                    FUN_00421671();
                }
                total = (total - local_left) + local_right;
            }
            param_1 = param_1 + 1;
            total = total + this->char_adv.advance;
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
    this->text_pos.x = this->GetGlobalTextX();
    this->text_pos.y = this->GetGlobalTextY();

    int mode = this->GetGlobalTextAlign();

    if (mode == 0) {
        w = this->ComputeTextMetrics(text);
        w = w / 2;
        this->text_pos.x -= w;
    }
    else if (mode == 1) {
        w = this->ComputeTextMetrics(text);
        this->text_pos.x -= w;
    }
    
    // vertical alignment for all modes
    vert = this->GetGlobalVertAlign();
    if (vert == 0) {
        this->text_pos.y += this->glyphHeight / 2;
        return;
    }
    if (vert == 1) {
        this->text_pos.y += this->glyphHeight;
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
  Animation *anim;
  char local_9c [128];
  int local_18;

  if (param_1 != (char *)0x0) {
    iVar2 = sscanf(param_1, "%s %d %d", local_9c, &this->firstChar, &local_18);
    if (iVar2 <= 2) {
      local_18 = 0x7e;
    }
    if (iVar2 <= 1) {
      this->firstChar = 0x21;
    }
    this->glyphCount = (local_18 - this->firstChar) + 1;
    delete buffer;
    buffer = 0;

    anim = new Animation();
    anim->Open(local_9c,0xfe000,0xffffffff);
    
    this->buffer = new VBuffer(anim->smk->Width, anim->smk->Height);
    anim->ToBufferVB(this->buffer);
    anim->DoFrame();
    if (anim != (Animation *)0x0) {
        anim->Delete(1);
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
    //__try {
        if (this->buffer != 0) {
            delete this->buffer;
            this->buffer = 0;
        }

        if (this->glyphTable != 0) {
            delete[] this->glyphTable;
            this->glyphTable = 0;
        }
    //} __finally {
    //}
}


/* Function start: 0x4239DE */
int AnimatedAsset::GetGlobalTextX()
{
    /* MOV EAX,[0x004374c2] ; RET */
    return DAT_004374c2;
}

/* Function start: 0x4239D8 */
int AnimatedAsset::GetGlobalTextY()
{
    /* MOV EAX,[0x004374ce] ; RET */
    return DAT_004374ce;
}

/* Function start: 0x4239D0 */
int AnimatedAsset::GetGlobalTextAlign()
{
    /* MOVSX EAX,byte ptr [0x004374c0] ; RET */
    return (int)(signed char)DAT_004374c0;
}

/* Function start: 0x4239C8 */
int AnimatedAsset::GetGlobalVertAlign()
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
    GlyphRect* piVar5;

    __try {
        if (param_3 == 0x20) {
            iVar4 = this->spaceWidth;
        }
        else if (param_3 == 9) {
            iVar4 = this->tabWidth;
        }
        else {
            piVar5 = (GlyphRect*)((param_3 - this->firstChar) * 0x10 + (int)this->glyphTable);
            iVar1 = piVar5->bottom;
            iVar4 = piVar5->right;
            iVar2 = piVar5->top;
            iVar3 = piVar5->left;
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
            int* piVar1 = &this->text_pos.x;
            do {
                char cVar2 = *text;
                text = text + 1;
                int iVar3 = this->DrawChar(*piVar1, this->text_pos.y, (int)cVar2);
                *piVar1 = *piVar1 + this->char_adv.advance + iVar3;
            } while (*text != '\0');
        }
    }
}
