#include "PaletteBuffer.h"
#include "PaletteUtils.h"
#include "Memory.h"

/* Function start: 0x41EB50 */
void PaletteBuffer::CopyEntries(int start, int count) {
    CopyPaletteData(start, count, (unsigned char*)data + start * 3);
}

/* Function start: 0x41EB70 */
void PaletteBuffer::SetEntries(int start, int count) {
    SetPaletteEntries_(start, count, (unsigned char*)data + start * 3);
}

/* Function start: 0x41EA80 */
void PaletteBuffer::Cleanup() {
    if (data != 0) {
        FreeMemory(data);
        data = 0;
    }
    size = 0;
}
