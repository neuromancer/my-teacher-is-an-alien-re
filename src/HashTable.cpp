#include "HashTable.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>


/* Function start: 0x442230 */
HashTable::~HashTable() {
    if (buckets != 0 && numBuckets != 0) {
        unsigned int i = 0;
        do {
            HashNode* node = buckets[i];
            while (node != 0) {
                char** field = (char**)&node->reserved;
                int j = 0;
                do {
                    if (*field != 0) {
                        FreeMemory(*field);
                        *field = 0;
                    }
                    field++;
                    int old = j;
                    j--;
                    if (old == 0) break;
                } while (1);
                int k = 0;
                do {
                    int old = k;
                    k--;
                    if (old == 0) break;
                } while (1);
                node = node->next;
            }
            i++;
        } while ((unsigned int)numBuckets > i);
    }

    FreeMemory(buckets);
    buckets = 0;
    count = 0;
    freeList = 0;

    void* pool = nodePool;
    while (pool != 0) {
        void* next = *(void**)pool;
        FreeMemory(pool);
        pool = next;
    }
    nodePool = 0;
}

/* Function start: 0x4437D0 */
void HashTable::AllocateBuckets(int newNumBuckets, int allocateNow) {
    if (buckets != 0) {
        FreeMemory(buckets);
        buckets = 0;
    }
    if (allocateNow != 0) {
        HashNode** newBuckets = (HashNode**)AllocateMemory(newNumBuckets * 4);
        buckets = newBuckets;
        memset(newBuckets, 0, newNumBuckets * 4);
    }
    numBuckets = newNumBuckets;
}

/* Function start: 0x44C5E0 */
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
