#ifndef FLAGARRAY_H
#define FLAGARRAY_H

#include <stdio.h>

class FlagArray {
public:
    FILE* fp; // 0x0
    char filename[50]; // 0x4
    char pad[0x38 - 4 - 50];
    int field_0x38;
    int field_0x3c;
    int max_states; // 0x40
    int field_0x44;
    char pad2[204 - 0x44 - 4];

    FlagArray(char* filename, int max_states);
    ~FlagArray();
    void SafeClose();
    void Open();
    void Close();
    void Seek(int index);
    unsigned int GetFlag(int index, unsigned int mask);
    void SetFlag(int index, unsigned int mask);
    void ClearAllFlags();
};

#endif // FLAGARRAY_H
