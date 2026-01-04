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
    memset(this, 0, 14 * sizeof(int));
    color = 2;
    glyphValue = glyphVal;
}

/* Function start: 0x421260 */
void AnimatedAsset::BuildGlyphTable()
{
    GlyphRect* table = new GlyphRect[glyphCount];

    int count = glyphCount << 4 >> 2;
    glyphTable = table;
    int* p = (int*)table;
    while (count != 0) {
        *p = 0;
        p++;
        count--;
    }

    int baseColMinus1 = buffer->height - 1;
    int baseIndex = (int)buffer->GetData();
    VBuffer* buf = buffer;
    
    glyphHeight = 1;

    if (buf->height > 1) {
        char glyphVal = glyphValue;
        int glyphH = glyphHeight;
        do {
            if (*(char*)((baseColMinus1 - glyphH) * buf->width + baseIndex) == glyphVal) break;
            glyphH++;
            glyphHeight = glyphH;
        } while (buf->height > glyphH);
    }

    int col = 0;
    GlyphRect* entryPtr = glyphTable;
    int offset = 0;

    if (buf->height > 0) {
        do {
            int x = 0;
            int startX = 0;
            int width = buffer->width;
            int chBase = (baseColMinus1 - offset) * width + baseIndex;

            if (width > 0) {
                int tableOffset = col << 4;
                char glyphVal = glyphValue;

                do {
                    if (*(char*)(x + chBase) == glyphVal) {
                        entryPtr->left = startX;
                        entryPtr->right = x - 1;
                        entryPtr->top = offset;
                        entryPtr->bottom = glyphHeight + offset - 1;
                        if (glyphCount - col == 1) goto BUILT;
                        startX = x + 1;
                        col++;
                        tableOffset += 0x10;
                        entryPtr = (GlyphRect*)((char*)glyphTable + tableOffset);
                    }
                    x++;
                } while (buffer->width > x);
            }

            offset = offset + glyphHeight + 1;
        } while (buffer->height > offset);
    }
BUILT:
    buffer->Lock();
}

/* Function start: 0x4215A0 */
int AnimatedAsset::ComputeTextMetrics(char* text)
{
    int total = 0;

    if (text == (char*)0x0) {
        return 0;
    }
    
    while (*text != '\0') {
        int iVar2 = (int)*text;
        if (iVar2 == 0x20) {
            total = total + spaceWidth;
        }
        else if (iVar2 == 9) {
            total = total + tabWidth;
        }
        else {
            GlyphRect* piVar3 = (GlyphRect*)((char*)glyphTable + iVar2 * 0x10);
            GlyphRect local = *piVar3;
            total = (total - local.left) + local.right;
        }
        text = text + 1;
        total = total + char_adv.advance;
    }
    return total;
}


/* Function start: 0x421680 */
void AnimatedAsset::PrepareText(char* text)
{
    int w;

    text_pos.x = GetGlobalTextX();
    text_pos.y = GetGlobalTextY();

    switch (GetGlobalTextAlign()) {
    case 0:
        w = ComputeTextMetrics(text);
        w = w / 2;
        text_pos.x -= w;
        break;
    case 1:
        w = ComputeTextMetrics(text);
        text_pos.x -= w;
        break;
    }
    
    switch (GetGlobalVertAlign()) {
    case 0:
        text_pos.y += glyphHeight / 2;
        return;
    case 1:
        text_pos.y += glyphHeight;
        return;
    }
}

/* Function start: 0x4213F0 */
int AnimatedAsset::IsCharSupported(int ch)
{
    if (ch != 0x20 && ch != 9) {
        int delta = ch - firstChar;
        if (delta < 0 || glyphCount - 1 < delta) {
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
    iVar2 = sscanf(param_1, "%s %d %d", local_9c, &firstChar, &local_18);
    if (iVar2 <= 2) {
      local_18 = 0x7e;
    }
    if (iVar2 <= 1) {
      firstChar = 0x21;
    }
    glyphCount = (local_18 - firstChar) + 1;
    delete buffer;
    buffer = 0;

    anim = new Animation();
    anim->Open(local_9c,0xfe000,0xffffffff);
    
    buffer = new VBuffer(anim->smk->Width, anim->smk->Height);
    anim->ToBufferVB(buffer);
    anim->DoFrame();
    if (anim != (Animation *)0x0) {
        anim->Delete(1);
    }
    BuildGlyphTable();
    iVar2 = IsCharSupported(0x41);
    if (iVar2 != 0) {
      iVar2 = ComputeTextMetrics(&DAT_00435ef4);
      iVar2 = (iVar2 * 2) / 3;
      spaceWidth = iVar2;
      tabWidth = iVar2 << 2;
    }
  }
}

/* Function start: 0x421010 */
AnimatedAsset::~AnimatedAsset()
{
    if (buffer) {
        delete buffer;
        buffer = 0;
    }

    if (glyphTable) {
        delete[] glyphTable;
        glyphTable = 0;
    }
}


/* Function start: 0x4239DE */
int GetGlobalTextX()
{
    return DAT_004374c2;
}

/* Function start: 0x4239D8 */
int GetGlobalTextY()
{
    return DAT_004374ce;
}

/* Function start: 0x4239D0 */
int GetGlobalTextAlign()
{
    return (int)(signed char)DAT_004374c0;
}

/* Function start: 0x4239C8 */
int GetGlobalVertAlign()
{
    return (int)(signed char)DAT_004374c1;
}

/* Function start: 0x421420 */
int AnimatedAsset::DrawChar(int param_1, int param_2, int param_3)
{
    int iVar4;

    //__try {
        if (param_3 == 0x20) {
            iVar4 = spaceWidth;
        }
        else if (param_3 == 9) {
            iVar4 = tabWidth;
        }
        else {
            GlyphRect local;
            int* p = &local.left;
            int index = param_3 - firstChar;
            p[0] = 0;
            index = index << 4;
            p[1] = 0;
            index = index + (int)glyphTable;
            p[2] = 0;
            p[3] = 0;
            GlyphRect* piVar5 = (GlyphRect*)index;
            int iVar1 = piVar5->bottom;
            int iVar2 = piVar5->right;
            int iVar3 = piVar5->top;
            int iVar0 = piVar5->left;
            local.left = iVar0;
            local.top = iVar3;
            local.right = iVar2;
            local.bottom = iVar1;
            if (useBuffer == 0) {
                if (useAttr != 0) {
                    g_WorkBuffer_00436974->CallBlitter3(iVar0, iVar2, iVar3, iVar1, param_1, param_2, buffer, color, attr);
                }
                else {
                    g_WorkBuffer_00436974->CallBlitter2(iVar0, iVar2, iVar3, iVar1, param_1, param_2, buffer);
                }
            }
            else {
                if (useAttr != 0) {
                    buffer->BlitTransparent(iVar0, iVar2, iVar3, iVar1, param_1, param_2, color, attr);
                }
                else {
                    buffer->ClipAndBlit((int)g_WorkBuffer_00436974, iVar0, iVar2, iVar3, iVar1, param_1, param_2);
                }
            }
            iVar4 = local.right - local.left;
        }
    return iVar4;
}

/* Function start: 0x421700 */
void AnimatedAsset::RenderText(char* text, int param_2)
{
    if (text != (char *)0x0) {
        useBuffer = 0;
        attr = (char)param_2;
        useAttr = (param_2 != -1);
        PrepareText(text);
        if (*text != '\0') {
            int* piVar1 = &text_pos.x;
            do {
                char cVar2 = *text;
                text = text + 1;
                int iVar3 = DrawChar(*piVar1, text_pos.y, (int)cVar2);
                *piVar1 = *piVar1 + char_adv.advance + iVar3;
            } while (*text != '\0');
        }
    }
}
