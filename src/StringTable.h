#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include <stdio.h>
#include "HashTable.h"

class StringTable {
public:
    FILE* fp;               // 0x0
    char* filename;         // 0x4
    HashTable* hashTable;   // 0x8

    StringTable(char* filename, int loadNow);
    ~StringTable();
    
    // Internal helpers
    void Load(); 
    void Unload(); 
    FILE* Open();
};

#endif
