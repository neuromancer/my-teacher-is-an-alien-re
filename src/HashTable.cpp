#include "HashTable.h"
#include "Memory.h"
#include <string.h>


HashTable::~HashTable() {
}

/* Function start: 0x420eb0 */
void HashTable::AllocateBuckets(int numBuckets, int allocateNow) {
    if (buckets != 0) {
        delete buckets;
        buckets = 0;
    }
    if (allocateNow != 0) {
        int* newBuckets = (int*)new char[numBuckets * 4];
        buckets = newBuckets;
        memset(newBuckets, 0, numBuckets * 4);
    }
    numBuckets = numBuckets;
}

/* Function start: 0x420f10 */
HashNode* HashTable::AllocateNode() {
    if (freeList == 0) {
        int* block = (int*)new char[capacity * 24 + 4];
        *block = (int)nodePool;
        nodePool = block;
        int count = capacity;
        HashNode* node = (HashNode*)((char*)block + capacity * 24 - 20);
        count = count - 1;
        while (count >= 0) {
            node->next = freeList;
            freeList = node;
            node = (HashNode*)((char*)node - 24);
            count = count - 1;
        }
    }
    HashNode* result = freeList;
    freeList = result->next;
    count = count + 1;
    result->key = 0;
    int i = 0;
    do {
        int prev = i;
        i = i - 1;
        if (prev == 0) break;
    } while (1);
    result->filePosLow = 0;
    result->filePosHigh = 0;
    i = 0;
    do {
        int prev = i;
        i = i - 1;
        if (prev == 0) break;
    } while (1);
    return result;
}
