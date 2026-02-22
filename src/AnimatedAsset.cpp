#include "AnimatedAsset.h"
#include "globals.h"
#include "Animation.h"
#include "VBuffer.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

/* Function start: 0x420F80 */
AnimatedAsset::AnimatedAsset()
{
    ZeroMemory(this, 56);
    color = 2;
    glyphValue = 1;
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

/* Function start: 0x4210D0 */
void AnimatedAsset::LoadAnimatedAsset(char *param_1)
{
  int iVar2;
  Animation *anim;
  char local_9c [128];
  int local_18;

  if (param_1 != 0) {
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
    if (anim != 0) {
        delete anim;
    }
    BuildGlyphTable();
    iVar2 = IsCharSupported(0x41);
    if (iVar2 != 0) {
      iVar2 = ComputeTextMetrics(g_EngineTypeCombat_00435ef4);
      iVar2 = (iVar2 * 2) / 3;
      spaceWidth = iVar2;
      tabWidth = iVar2 << 2;
    }
  }
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

    glyphHeight = 1;

    if (buffer->height > 1) {
        char glyphVal = glyphValue;
        do {
            if (*(char*)((baseColMinus1 - glyphHeight) * buffer->width + baseIndex) == glyphVal) break;
            glyphHeight++;
        } while (buffer->height > glyphHeight);
    }

    int col = 0;
    GlyphRect* entryPtr = glyphTable;
    int offset = 0;

    if (buffer->height > 0) {
        do {
            int x = 0;
            int startX = 0;
            int width = buffer->width;
            int chBase = (baseColMinus1 - offset) * width + baseIndex;

            if (width > 0) {
                int tableOffset = col << 4;

                do {
                    if (*(char*)(x + chBase) == glyphValue) {
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

/* Function start: 0x421420 */
int AnimatedAsset::DrawChar(int x, int y, int ch)
{
    int width;

    if (ch == 0x20) {
        width = spaceWidth;
    }
    else if (ch == 0x9) {
        width = tabWidth;
    }
    else {
        GlyphRect local;
        GlyphRect* entry = &glyphTable[ch - firstChar];
        int b = entry->bottom;
        int r = entry->right;
        int t = entry->top;
        int l = entry->left;

        local.left = l;
        local.top = t;
        local.right = r;
        local.bottom = b;

        if (useBuffer != 0) {
            if (useAttr != 0) {
                buffer->BlitTransparent(local.left, local.right, local.top, local.bottom, x, y, color, attr);
            }
            else {
                buffer->TPaste(local.left, local.right, local.top, local.bottom, x, y);
            }
        }
        else {
            if (useAttr != 0) {
                g_WorkBuffer_00436974->CallBlitter3(local.left, local.right, local.top, local.bottom, x, y, buffer, color, attr);
            }
            else {
                g_WorkBuffer_00436974->CallBlitter2(local.left, local.right, local.top, local.bottom, x, y, buffer);
            }
        }
        width = local.right - local.left;
    }
    return width;
}

/* Function start: 0x4215A0 */
int AnimatedAsset::ComputeTextMetrics(char* text)
{
    int total = 0;

    if (text == 0) {
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
            GlyphRect* piVar3 = &glyphTable[iVar2];
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

/* Function start: 0x421700 */
void AnimatedAsset::RenderText(char* text, int color_param)
{
    if (text != 0) {
        useBuffer = 0;
        attr = (char)color_param;
        useAttr = (color_param != -1);
        PrepareText(text);
        if (*text != '\0') {
            int* p_x = &text_pos.x;
            do {
                int width = DrawChar(*p_x, p_x[1], (int)(char)*text++);
                *p_x += char_adv.advance + width;
            } while (*text != '\0');
        }
    }
}
