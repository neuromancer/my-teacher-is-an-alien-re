#include "globals.h"
#include "VBuffer.h"

extern "C" {
    int CreateTableFromBuffer(int, int, int);
    int GetVideoBufferData(unsigned int);
    void ScaleBuffer(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int);
    void FUN_004234d5(unsigned int);
}

/* Function start: 0x41B0A0 */
void __stdcall DrawScaledSprite(int x, int y, void* data, double scale)
{
    VBuffer* buf = (VBuffer*)data;
    int scaledWidth = (int)(buf->width * scale);
    int scaledHeight = (int)(buf->height * scale);

    unsigned int tableHandle = CreateTableFromBuffer((int)g_Buffer_00436964, scaledWidth, scaledHeight);

    void* destData = (void*)GetVideoBufferData(tableHandle);

    ScaleBuffer(buf->data, destData, buf->width, buf->height, scaledWidth, scaledHeight);

    FUN_004234d5(tableHandle);
}
