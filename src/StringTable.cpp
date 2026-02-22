#include "StringTable.h"
#include "Memory.h"
#include "string.h"
#include <stdio.h>
#include <windows.h>

/* Function start: 0x4209e0 */
StringTable::StringTable(char* f, int loadNow) {
    fp = 0;
    filename = 0;
    hashTable = 0;

    // strlen to compute allocation size
    int len = strlen(f) + 1;

    filename = new char[len];

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
        delete filename;
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
        delete ht->buckets;
        ht->buckets = 0;
        ht->count = 0;
        ht->freeList = 0;

        // Free the node pool blocks
        int* poolBlock = (int*)ht->nodePool;
        while (poolBlock != 0) {
            int* next = (int*)*poolBlock;
            delete poolBlock;
            poolBlock = next;
        }
        ht->nodePool = 0;

        // Free the hash table itself
        delete ht;
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

    int openResult = (int)Open();
    if (openResult != 0) {
        filePos = 0;
        filePos2 = 0;
        lineCount = 0;

        // First pass: count lines
        char* result = internal_ReadLine(buffer, 0xff, fp);
        while (result != 0) {
            lineCount = lineCount + 1;
            result = internal_ReadLine(buffer, 0xff, fp);
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

                delete ht->buckets;
                ht->buckets = 0;
                ht->count = 0;
                ht->freeList = 0;

                int* poolBlock = (int*)ht->nodePool;
                while (poolBlock != 0) {
                    int* next = (int*)*poolBlock;
                    delete poolBlock;
                    poolBlock = next;
                }
                ht->nodePool = 0;

                delete (void*)ht;
                hashTable = 0;
            }

            // Allocate new hash table
            hashTable = new HashTable(lineCount);

            // Reset to beginning of file
            fsetpos(fp, &filePos);

            // Second pass: parse lines
            do {
                fgetpos(fp, &filePos);
                result = internal_ReadLine(buffer, 0xff, fp);
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
            } while (1);
        }
        Unload();
    }
}

/* Function start: 0x420d90 */
int StringTable::GetString(unsigned int id, char* outBuffer)
{
    char buffer[300];
    unsigned int parsedId;
    fpos_t filePos;
    int found;
    HashNode* node;
    char* startQuote;
    char* endQuote;
    int len;

    found = 0;
    filePos = 0;

    if (StringTable::hashTable != 0) {
        unsigned int bucketIdx = (id >> 4) % (unsigned int)StringTable::hashTable->numBuckets;
        if (StringTable::hashTable->buckets == 0) {
            goto no_node;
        }
        node = *(HashNode**)(StringTable::hashTable->buckets + bucketIdx);
        goto hash_test;

    hash_next:
        node = node->next;
    hash_test:
        if (node == 0) {
            goto no_node;
        }
        if (node->key == id) {
            goto hash_found;
        }
        goto hash_next;

    no_node:
        node = 0;

    hash_found:
        if (node != 0) {
            filePos = *(fpos_t*)&node->filePosLow;
        }
    }

    if (Open() != 0) {
        fsetpos(StringTable::fp, &filePos);
read_loop:
        {
            char* result = internal_ReadLine(buffer, 0xff, StringTable::fp);
            if (result == 0) {
                goto done;
            }
        }
        sscanf(buffer, "%u", &parsedId);
        if (parsedId != id) goto read_loop;

        startQuote = strchr(buffer, '\"');
        startQuote = startQuote + 1;
        endQuote = strrchr(buffer, '\"');
        len = endQuote - startQuote;
        if (startQuote == 0) goto read_loop;
        if (endQuote == 0) goto read_loop;
        if (len <= 0) goto read_loop;

        found = 1;
        strncpy(outBuffer, startQuote, len);
done:
        Unload();
    }
    return found;
}
