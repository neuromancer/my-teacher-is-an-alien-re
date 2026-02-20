#include "HashTable.h"
#include "Memory.h"
#include <string.h>


HashTable::~HashTable() {
}

/* Function start: 0x420eb0 */
void HashTable::AllocateBuckets(int newNumBuckets, int allocateNow) {
    if (buckets != 0) {
        FreeMemory(buckets);
        buckets = 0;
    }
    if (allocateNow != 0) {
        int* newBuckets = (int*)AllocateMemory(newNumBuckets * 4);
        buckets = newBuckets;
        memset(newBuckets, 0, newNumBuckets * 4);
    }
    numBuckets = newNumBuckets;
}

/* Function start: 0x420f10 */
HashNode* HashTable::AllocateNode() {
    if (freeList == 0) {
        int* block = (int*)AllocateMemory(capacity * 24 + 4);
        *block = (int)nodePool;
        nodePool = block;
        int cnt = capacity;
        HashNode* node = (HashNode*)((char*)block + cnt * 24 - 20);
        cnt--;
        if (cnt >= 0) {
            do {
                cnt--;
                node->next = freeList;
                freeList = node;
                node = (HashNode*)((char*)node - 24);
            } while (cnt >= 0);
        }
    }
    HashNode* result = freeList;
    int n = 0;
    freeList = result->next;
    count++;
    result->key = n;
    do {
        int prev = n;
        n--;
        if (prev == 0) break;
    } while (1);
    result->filePosLow = n;
    result->filePosHigh = n;
    n = 0;
    do {
        int prev = n;
        n--;
        if (prev == 0) break;
    } while (1);
    return result;
}
