#include "globals.h"
#include <string.h>



#pragma optimize("y", off)

/* Function start: 0x452CA3 */
int __cdecl BlitBufferOpaque(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle)
{
    unsigned int srcIdx = (srcHandle & 0x1f);
    int srcHeight = g_VBufHeights_0046ded4[srcIdx] - 1;
    int flipY2 = srcHeight - srcY1;
    srcY1 = srcHeight - srcY2;
    srcY2 = flipY2;
    unsigned int srcWidth = g_VBufWidths_0046df54[srcIdx];
    char* srcPtr = (char*)g_VBufDataPtrs_0046de54[srcIdx];
    if (srcPtr != 0) {
        unsigned int destIdx = (destHandle & 0x1f);
        unsigned int destWidth = g_VBufWidths_0046df54[destIdx];
        int destHeight = g_VBufHeights_0046ded4[destIdx] - 1;
        char* destPtr = (char*)g_VBufDataPtrs_0046de54[destIdx];
        if (destPtr != 0) {
            srcPtr += (unsigned int)srcY1 * srcWidth + srcX1;
            destPtr += (unsigned int)(destHeight - destY) * destWidth + destX;
            int copyHeight = (srcY2 + 1) - srcY1;
            int copyWidth = (srcX2 + 1) - srcX1;
            int srcSkip = srcWidth - copyWidth;
            int destSkip = destWidth - copyWidth;
            do {
                memcpy(destPtr, srcPtr, copyWidth);
                srcPtr += copyWidth + srcSkip;
                destPtr += copyWidth + destSkip;
                copyHeight--;
            } while (copyHeight != 0);
        }
    }
    return 0;
}

/* Function start: 0x452EBD */
int __cdecl BlitBufferTransparent(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle)
{
    unsigned char localTable[256];
    memcpy(localTable, g_PaletteData_0046d208, 256);
    
    int srcIdx = (srcHandle & 0x1f);
    int destIdx = (destHandle & 0x1f);
    
    int srcData = g_VBufDataPtrs_0046de54[srcIdx];
    if (srcData == 0) {
        return 0;
    }
    
    int srcWidth = g_VBufWidths_0046df54[srcIdx];
    int srcHeight = g_VBufHeights_0046ded4[srcIdx];
    
    int destData = g_VBufDataPtrs_0046de54[destIdx];
    if (destData == 0) {
        return 0;
    }
    
    int destWidth = g_VBufWidths_0046df54[destIdx];
    int destHeight = g_VBufHeights_0046ded4[destIdx];
    
    int flipY1 = (srcHeight - 1) - srcY2;
    int flipY2 = (srcHeight - 1) - srcY1;
    
    unsigned char* srcPtr = (unsigned char*)(srcData + flipY1 * srcWidth + srcX1);
    
    int destFlipY = (destHeight - 1) - destY;
    unsigned char* destPtr = (unsigned char*)(destData + destFlipY * destWidth + destX);
    
    int copyWidth = (srcX2 + 1) - srcX1;
    int copyHeight = (flipY2 + 1) - flipY1;
    
    int srcSkip = srcWidth - copyWidth;
    int destSkip = destWidth - copyWidth;
    
    for (int row = 0; row < copyHeight; row++) {
        for (int col = 0; col < copyWidth; col++) {
            unsigned char pixel = *srcPtr++;
            if (localTable[pixel] == 0) {
                *destPtr = pixel;
            }
            destPtr++;
        }
        srcPtr += srcSkip;
        destPtr += destSkip;
    }
    
    return 0;
}


