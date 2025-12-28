#include "StringTable.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

// Helper externs if needed, or implement locally
extern "C" char* __cdecl FUN_00419780(char* buffer, int length, FILE* file);
// Note: FUN_00419780 is likely internal_ReadLine or similar.

/* Function start: 0x4209e0 */
void* StringTable::Init(char* filename, int loadNow) {
    this->fp = 0;
    this->filename = 0;
    this->internalData = 0;
    
    // strlen to compute allocation size  
    int len = strlen(filename) + 1;
    
    this->filename = (char*)AllocateMemory(len);
    
    // Copy using strcpy - compiler should inline as REP MOVS pattern
    strcpy(this->filename, filename);
    
    if (loadNow != 0) {
        Load();
    }
    return this;
}

/* Function start: 0x420b20 */
void StringTable::Unload() {
    if (this->fp) {
        fclose(this->fp);
        this->fp = 0;
    }
}

/* Function start: 0x420a50 */
void StringTable::Cleanup() {
    Unload();
    
    if (this->filename != 0) {
        FreeMemory(this->filename);
        this->filename = 0;
    }
    
    int* hashTable = (int*)this->internalData;
    if (hashTable != 0) {
        // Check if buckets array exists and has entries
        if (*(int*)hashTable != 0 && hashTable[1] != 0) {
            int numBuckets = hashTable[1];
            int* buckets = (int*)*(int*)hashTable;
            
            // Iterate through each bucket
            do {
                // Walk the chain in this bucket
                int* node = (int*)*buckets;
                while (node != 0) {
                    // These loops appear to be counting/checking something
                    // but effectively do nothing (original has two empty counting loops)
                    int i = 0;
                    do {
                        int prev = i;
                        i = i - 1;
                        if (prev == 0) break;
                    } while (1);
                    
                    i = 0;
                    do {
                        int prev = i;
                        i = i - 1;
                        if (prev == 0) break;
                    } while (1);
                    
                    node = (int*)*node;
                }
                buckets = buckets + 1;
                numBuckets = numBuckets - 1;
            } while (numBuckets != 0);
        }
        
        // Free the buckets array
        FreeMemory((void*)*(int*)hashTable);
        *(int*)hashTable = 0;
        hashTable[2] = 0;
        hashTable[3] = 0;
        
        // Free the string nodes linked list at offset 0x10
        int* stringNode = (int*)hashTable[4];
        while (stringNode != 0) {
            int* next = (int*)*stringNode;
            FreeMemory(stringNode);
            stringNode = next;
        }
        hashTable[4] = 0;
        
        // Free the hash table itself
        FreeMemory(hashTable);
        this->internalData = 0;
    }
}

/* Function start: 0x420b00 */
FILE* StringTable::Open() {
    Unload();
    this->fp = fopen(this->filename, "r");
    return this->fp;
}

/* Function start: 0x420b40 */
void StringTable::Load() {
    char buffer[300];
    fpos_t filePos;
    fpos_t filePos2;
    unsigned int lineCount;
    unsigned int stringId;
    
    __try {
        int openResult = (int)Open();
        if (openResult != 0) {
            filePos = 0;
            filePos2 = 0;
            lineCount = 0;
            
            // First pass: count lines
            char* result = FUN_00419780(buffer, 0xff, this->fp);
            while (result != 0) {
                lineCount = lineCount + 1;
                result = FUN_00419780(buffer, 0xff, this->fp);
            }
            
            if (lineCount != 0) {
                // Clean up existing hash table (same logic as in Cleanup)
                int* hashTable = (int*)this->internalData;
                if (hashTable != 0) {
                    if (*(int*)hashTable != 0 && hashTable[1] != 0) {
                        int numBuckets = hashTable[1];
                        int* buckets = (int*)*(int*)hashTable;
                        
                        do {
                            int* node = (int*)*buckets;
                            while (node != 0) {
                                int i = 0;
                                do {
                                    int prev = i;
                                    i = i - 1;
                                    if (prev == 0) break;
                                } while (1);
                                
                                i = 0;
                                do {
                                    int prev = i;
                                    i = i - 1;
                                    if (prev == 0) break;
                                } while (1);
                                
                                node = (int*)*node;
                            }
                            buckets = buckets + 1;
                            numBuckets = numBuckets - 1;
                        } while (numBuckets != 0);
                    }
                    
                    FreeMemory((void*)*(int*)hashTable);
                    *(int*)hashTable = 0;
                    hashTable[2] = 0;
                    hashTable[3] = 0;
                    
                    int* stringNode = (int*)hashTable[4];
                    while (stringNode != 0) {
                        int* next = (int*)*stringNode;
                        FreeMemory(stringNode);
                        stringNode = next;
                    }
                    hashTable[4] = 0;
                    
                    FreeMemory(hashTable);
                    this->internalData = 0;
                }
                
                // Allocate new hash table
                int* newHashTable = (int*)AllocateMemory(0x18);
                int* finalTable;
                if (newHashTable == 0) {
                    finalTable = 0;
                } else {
                    newHashTable[0] = 0;
                    newHashTable[1] = 0x11;  // 17 buckets
                    newHashTable[2] = 0;
                    newHashTable[3] = 0;
                    newHashTable[4] = 0;
                    newHashTable[5] = lineCount;
                    finalTable = newHashTable;
                }
                this->internalData = finalTable;
                
                // Reset to beginning of file
                fsetpos(this->fp, &filePos);
                
                // Second pass: parse lines
                while (1) {
                    fgetpos(this->fp, &filePos);
                    result = FUN_00419780(buffer, 0xff, this->fp);
                    if (result == 0) break;
                    
                    int scanResult = sscanf(buffer, "%u", &stringId);
                    unsigned int id = stringId;
                    if (scanResult == 1) {
                        int* ht = (int*)this->internalData;
                        int bucketsPtr = *ht;
                        unsigned int bucketIdx = (stringId >> 4) % (unsigned int)ht[1];
                        
                        int* foundNode = 0;
                        if (bucketsPtr != 0) {
                            int* node = *(int**)(bucketsPtr + bucketIdx * 4);
                            while (node != 0) {
                                if ((unsigned int)node[2] == stringId) {
                                    foundNode = node;
                                    break;
                                }
                                node = (int*)*node;
                            }
                        }
                        
                        if (foundNode == 0) {
                            if (bucketsPtr == 0) {
                                // FUN_00420eb0 - allocate buckets
                                void FUN_00420eb0(int*, int, int);
                                FUN_00420eb0(ht, ht[1], 1);
                            }
                            // FUN_00420f10 - allocate new node
                            int* FUN_00420f10(int*);
                            foundNode = FUN_00420f10(ht);
                            foundNode[1] = bucketIdx;
                            foundNode[2] = id;
                            *foundNode = *(int*)(*ht + bucketIdx * 4);
                            *(int*)(*ht + bucketIdx * 4) = (int)foundNode;
                        }
                        foundNode[4] = (int)filePos;
                        foundNode[5] = (int)filePos2;
                    }
                }
            }
            Unload();
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}
