#include "globals.h"
#include <string.h>

extern "C" {

/* Function start: 0x4231CE */
int __cdecl BlitBufferOpaque(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle)
{
    int srcIdx = (srcHandle & 0x1f);
    int destIdx = (destHandle & 0x1f);
    
    int srcData = DAT_0043826c[srcIdx];
    if (srcData == 0) {
        return 0;
    }
    
    int srcWidth = DAT_0043836c[srcIdx];
    int srcHeight = DAT_004382ec[srcIdx];
    
    int destData = DAT_0043826c[destIdx];
    if (destData == 0) {
        return 0;
    }
    
    int destWidth = DAT_0043836c[destIdx];
    int destHeight = DAT_004382ec[destIdx];
    
    int flipY1 = (srcHeight - 1) - srcY2;
    int flipY2 = (srcHeight - 1) - srcY1;
    
    char* srcPtr = (char*)(srcData + flipY1 * srcWidth + srcX1);
    
    int destFlipY = (destHeight - 1) - destY;
    char* destPtr = (char*)(destData + destFlipY * destWidth + destX);
    
    int copyWidth = (srcX2 + 1) - srcX1;
    int copyHeight = (flipY2 + 1) - flipY1;
    
    int srcSkip = srcWidth - copyWidth;
    int destSkip = destWidth - copyWidth;
    
    for (int row = 0; row < copyHeight; row++) {
        memcpy(destPtr, srcPtr, copyWidth);
        srcPtr += copyWidth + srcSkip;
        destPtr += copyWidth + destSkip;
    }
    
    return 0;
}

/* Function start: 0x4233E8 */
int __cdecl BlitBufferTransparent(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle)
{
    unsigned char localTable[256];
    memcpy(localTable, DAT_00437620, 256);
    
    int srcIdx = (srcHandle & 0x1f);
    int destIdx = (destHandle & 0x1f);
    
    int srcData = DAT_0043826c[srcIdx];
    if (srcData == 0) {
        return 0;
    }
    
    int srcWidth = DAT_0043836c[srcIdx];
    int srcHeight = DAT_004382ec[srcIdx];
    
    int destData = DAT_0043826c[destIdx];
    if (destData == 0) {
        return 0;
    }
    
    int destWidth = DAT_0043836c[destIdx];
    int destHeight = DAT_004382ec[destIdx];
    
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

}
