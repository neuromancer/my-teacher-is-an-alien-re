#ifndef PALETTEBUFFER_H
#define PALETTEBUFFER_H

struct PaletteBuffer {
    int size;
    void* data;
    
    void CopyEntries(int start, int count);
    void SetEntries(int start, int count);
    void Cleanup();
};

#endif // PALETTEBUFFER_H
