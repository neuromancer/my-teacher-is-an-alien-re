#include "globals.h"
#include <string.h>

/* 
 * BuildScaleTable - creates a bitmask table for Bresenham-style scaling
 * 
 * Original function at 0x4236D7 uses non-standard register calling:
 * ECX = count (max of src/dest), EDX = src size, EBX = dest size, EDI = output buffer
 * We inline this functionality since we can't match the register calling convention.
 */
static void BuildScaleTable(unsigned char* output, unsigned int count, unsigned int srcSize, unsigned int destSize)
{
    unsigned char bitMask;
    unsigned char currentByte;
    unsigned int smaller;
    unsigned int larger;
    unsigned int accumLarger;
    unsigned int accumSmaller;
    
    currentByte = 0x40;  /* AL starts as 0x40 */
    bitMask = 0x80;      /* AH starts as 0x80 */
    
    count--;
    if (count == 0) {
        *output = currentByte;
        return;
    }
    
    /* The original swaps if destSize > srcSize */
    if (srcSize < destSize) {
        smaller = srcSize;
        larger = destSize;
    } else {
        smaller = destSize;
        larger = srcSize;
    }
    
    accumSmaller = smaller;
    accumLarger = larger;
    
    do {
        accumSmaller = accumSmaller + smaller;
        if (accumSmaller > accumLarger) {
            accumLarger = accumLarger + larger;
            currentByte = currentByte | bitMask;
        }
        
        if (bitMask & 1) {
            *output = currentByte;
            output++;
            currentByte = 0;
        }
        bitMask = (bitMask >> 1) | ((bitMask & 1) << 7);
        count--;
    } while (count != 0);
    
    *output = currentByte;
}

/* Function start: 0x4234F9 */
extern "C" void __cdecl ScaleBuffer(void* srcData, void* destData, unsigned int srcWidth, unsigned int srcHeight, unsigned int destWidth, unsigned int destHeight)
{
    unsigned char xTable[130];
    unsigned char yTable[130];
    unsigned int maxWidth;
    unsigned int maxHeight;
    unsigned char scaleFlags;
    unsigned char* src;
    unsigned char* dest;
    unsigned char yBitMask;
    unsigned char xBitMask;
    unsigned char yByte;
    unsigned char xByte;
    unsigned char* yTablePtr;
    unsigned char* xTablePtr;
    unsigned int xCount;
    unsigned int yCount;
    
    scaleFlags = 0;
    
    if ((int)srcWidth <= (int)destWidth) {
        maxWidth = destWidth;
        scaleFlags = 1;
    } else {
        maxWidth = srcWidth;
    }
    
    if ((int)srcHeight <= (int)destHeight) {
        maxHeight = destHeight;
        scaleFlags = scaleFlags | 2;
    } else {
        maxHeight = srcHeight;
    }
    
    BuildScaleTable(xTable, maxWidth, srcWidth, destWidth);
    BuildScaleTable(yTable, maxHeight, srcHeight, destHeight);
    
    src = (unsigned char*)srcData;
    dest = (unsigned char*)destData;
    yTablePtr = yTable;
    yByte = *yTablePtr;
    yBitMask = 0x80;
    
    if (scaleFlags == 0) {
        /* Case 0: srcWidth > destWidth AND srcHeight > destHeight */
        /* Skip rows & copy to dest with row duplication from previous */
        yCount = maxHeight;
        do {
            if ((yBitMask & yByte) == 0) {
                /* Skip source row - copy previous dest row */
                memcpy(dest, dest - maxWidth, maxWidth);
                dest += maxWidth;
            } else {
                xTablePtr = xTable;
                xBitMask = 0x80;
                xByte = *xTablePtr;
                xCount = maxWidth;
                do {
                    if ((xBitMask & xByte) == 0) {
                        src--;
                    }
                    *dest = *src;
                    dest++;
                    src++;
                    
                    if (xBitMask & 1) {
                        xTablePtr++;
                        xByte = *xTablePtr;
                    }
                    xBitMask = (xBitMask >> 1) | ((xBitMask & 1) << 7);
                    xCount--;
                } while (xCount != 0);
            }
            
            if (yBitMask & 1) {
                yTablePtr++;
                yByte = *yTablePtr;
            }
            yBitMask = (yBitMask >> 1) | ((yBitMask & 1) << 7);
            yCount--;
        } while (yCount != 0);
    }
    else if (scaleFlags == 1) {
        /* Case 1: srcWidth <= destWidth AND srcHeight > destHeight */
        yCount = maxHeight;
        do {
            if ((yBitMask & yByte) == 0) {
                src = src + srcWidth;
            } else {
                xTablePtr = xTable;
                xBitMask = 0x80;
                xByte = *xTablePtr;
                xCount = maxWidth;
                do {
                    if ((xBitMask & xByte) == 0) {
                        src--;
                    }
                    *dest = *src;
                    dest++;
                    src++;
                    
                    if (xBitMask & 1) {
                        xTablePtr++;
                        xByte = *xTablePtr;
                    }
                    xBitMask = (xBitMask >> 1) | ((xBitMask & 1) << 7);
                    xCount--;
                } while (xCount != 0);
            }
            
            if (yBitMask & 1) {
                yTablePtr++;
                yByte = *yTablePtr;
            }
            yBitMask = (yBitMask >> 1) | ((yBitMask & 1) << 7);
            yCount--;
        } while (yCount != 0);
    }
    else if (scaleFlags == 2) {
        /* Case 2: srcWidth > destWidth AND srcHeight <= destHeight */
        yCount = maxHeight;
        do {
            if ((yBitMask & yByte) == 0) {
                /* Copy previous row */
                memcpy(dest, dest - maxWidth, maxWidth);
                dest += maxWidth;
            } else {
                xTablePtr = xTable;
                xBitMask = 0x80;
                xByte = *xTablePtr;
                xCount = maxWidth;
                do {
                    if ((xBitMask & xByte) == 0) {
                        src++;
                    } else {
                        *dest = *src;
                        dest++;
                    }
                    
                    if (xBitMask & 1) {
                        xTablePtr++;
                        xByte = *xTablePtr;
                    }
                    xBitMask = (xBitMask >> 1) | ((xBitMask & 1) << 7);
                    xCount--;
                } while (xCount != 0);
            }
            
            if (yBitMask & 1) {
                yTablePtr++;
                yByte = *yTablePtr;
            }
            yBitMask = (yBitMask >> 1) | ((yBitMask & 1) << 7);
            yCount--;
        } while (yCount != 0);
    }
    else {
        /* Case 3: srcWidth <= destWidth AND srcHeight <= destHeight (grow both) */
        yCount = maxHeight;
        do {
            if ((yBitMask & yByte) == 0) {
                /* Copy previous row */
                memcpy(dest, dest - maxWidth, maxWidth);
                dest += maxWidth;
            } else {
                xTablePtr = xTable;
                xBitMask = 0x80;
                xByte = *xTablePtr;
                xCount = maxWidth;
                do {
                    unsigned char* pixelSrc = src;
                    if ((xBitMask & xByte) == 0) {
                        pixelSrc = src - 1;
                    }
                    *dest = *pixelSrc;
                    dest++;
                    src++;
                    
                    if (xBitMask & 1) {
                        xTablePtr++;
                        xByte = *xTablePtr;
                    }
                    xBitMask = (xBitMask >> 1) | ((xBitMask & 1) << 7);
                    xCount--;
                } while (xCount != 0);
            }
            
            if (yBitMask & 1) {
                yTablePtr++;
                yByte = *yTablePtr;
            }
            yBitMask = (yBitMask >> 1) | ((yBitMask & 1) << 7);
            yCount--;
        } while (yCount != 0);
    }
}
