#include "StringTable.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

// Helper externs if needed, or implement locally
extern "C" char* __cdecl FUN_00419780(char* buffer, int length, FILE* file) {
    return fgets(buffer, length, file);
}

/* Function start: 0x4209e0 */
StringTable::StringTable(char* f, int loadNow) {
    fp = 0;
    filename = 0;
    hashTable = 0;

    // strlen to compute allocation size
    int len = strlen(f) + 1;

    filename = (char*)AllocateMemory(len);

    // Copy using strcpy - compiler should inline as REP MOVS pattern
    strcpy(filename, f);

    if (loadNow != 0) {
        Load();
    }
}

/* Function start: 0x420a50 */
StringTable::~StringTable() {
    Unload();

    if (filename != 0) {
        FreeMemory(filename);
        filename = 0;
    }

    HashTable* ht = hashTable;
    if (ht != 0) {
        // Check if buckets array exists and has entries
        if (ht->buckets != 0 && ht->numBuckets != 0) {
            int numBuckets = ht->numBuckets;
            int* buckets = ht->buckets;

            // Iterate through each bucket
            do {
                // Walk the chain in this bucket
                HashNode* node = (HashNode*)*buckets;
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

                    node = node->next;
                }
                buckets = buckets + 1;
                numBuckets = numBuckets - 1;
            } while (numBuckets != 0);
        }

        // Free the buckets array
        FreeMemory(ht->buckets);
        ht->buckets = 0;
        ht->count = 0;
        ht->freeList = 0;

        // Free the node pool blocks
        int* poolBlock = (int*)ht->nodePool;
        while (poolBlock != 0) {
            int* next = (int*)*poolBlock;
            FreeMemory(poolBlock);
            poolBlock = next;
        }
        ht->nodePool = 0;

        // Free the hash table itself
        FreeMemory(ht);
        hashTable = 0;
    }
}

/* Function start: 0x420b00 */
FILE* StringTable::Open() {
    Unload();
    fp = fopen(filename, "r");
    return fp;
}

/* Function start: 0x420b20 */
void StringTable::Unload() {
    if (fp) {
        fclose(fp);
        fp = 0;
    }
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
            char* result = FUN_00419780(buffer, 0xff, fp);
            while (result != 0) {
                lineCount = lineCount + 1;
                result = FUN_00419780(buffer, 0xff, fp);
            }

            if (lineCount != 0) {
                // Clean up existing hash table (same logic as in Cleanup)
                HashTable* ht = hashTable;
                if (ht != 0) {
                    if (ht->buckets != 0 && ht->numBuckets != 0) {
                        int numBuckets = ht->numBuckets;
                        int* buckets = ht->buckets;

                        do {
                            HashNode* node = (HashNode*)*buckets;
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

                                node = node->next;
                            }
                            buckets = buckets + 1;
                            numBuckets = numBuckets - 1;
                        } while (numBuckets != 0);
                    }

                    FreeMemory(ht->buckets);
                    ht->buckets = 0;
                    ht->count = 0;
                    ht->freeList = 0;

                    int* poolBlock = (int*)ht->nodePool;
                    while (poolBlock != 0) {
                        int* next = (int*)*poolBlock;
                        FreeMemory(poolBlock);
                        poolBlock = next;
                    }
                    ht->nodePool = 0;

                    FreeMemory(ht);
                    hashTable = 0;
                }

                // Allocate new hash table
                HashTable* newHashTable = (HashTable*)AllocateMemory(0x18);
                HashTable* finalTable;
                if (newHashTable == 0) {
                    finalTable = 0;
                } else {
                    newHashTable->buckets = 0;
                    newHashTable->numBuckets = 0x11;  // 17 buckets
                    newHashTable->count = 0;
                    newHashTable->freeList = 0;
                    newHashTable->nodePool = 0;
                    newHashTable->capacity = lineCount;
                    finalTable = newHashTable;
                }
                hashTable = finalTable;

                // Reset to beginning of file
                fsetpos(fp, &filePos);

                // Second pass: parse lines
                while (1) {
                    fgetpos(fp, &filePos);
                    result = FUN_00419780(buffer, 0xff, fp);
                    if (result == 0) break;

                    int scanResult = sscanf(buffer, "%u", &stringId);
                    unsigned int id = stringId;
                    if (scanResult == 1) {
                        HashTable* ht = hashTable;
                        int* bucketsPtr = ht->buckets;
                        unsigned int bucketIdx = (stringId >> 4) % (unsigned int)ht->numBuckets;

                        HashNode* foundNode = 0;
                        if (bucketsPtr != 0) {
                            HashNode* node = *(HashNode**)(bucketsPtr + bucketIdx);
                            while (node != 0) {
                                if (node->key == stringId) {
                                    foundNode = node;
                                    break;
                                }
                                node = node->next;
                            }
                        }

                        if (foundNode == 0) {
                            if (bucketsPtr == 0) {
                                ht->AllocateBuckets(ht->numBuckets, 1);
                            }
                            foundNode = ht->AllocateNode();
                            foundNode->bucketIndex = bucketIdx;
                            foundNode->key = id;
                            foundNode->next = *(HashNode**)(ht->buckets + bucketIdx);
                            *(HashNode**)(ht->buckets + bucketIdx) = foundNode;
                        }
                        foundNode->filePosLow = (int)filePos;
                        foundNode->filePosHigh = (int)filePos2;
                    }
                }
            }
            Unload();
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}
