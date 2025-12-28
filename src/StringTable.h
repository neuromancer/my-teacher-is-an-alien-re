#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include <stdio.h>

class StringTable {
public:
    FILE* fp;               // 0x0
    char* filename;         // 0x4
    void* internalData;     // 0x8

    void* Init(char* filename, int loadNow);
    void Cleanup();
    
    // Internal helpers
    void Load(); 
    void Unload(); 
    int Open();
};

#endif
