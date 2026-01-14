#ifndef PALETTEBUFFER_H
#define PALETTEBUFFER_H

// PaletteBuffer is essentially the same as Palette (same memory layout)
// Size: 8 bytes
struct PaletteBuffer {
    int size;    // 0x00
    void* data;  // 0x04
};

#endif // PALETTEBUFFER_H
