#include "HashTable.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>


/* Function start: 0x442230 */
HashTable::~HashTable() {
    if (buckets != 0 && numBuckets != 0) {
        unsigned int i = 0;
        int offset = 0;
        do {
            HashNode* node = *(HashNode**)((char*)buckets + offset);
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
            offset += 4;
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
        int* newBuckets = (int*)AllocateMemory(newNumBuckets * 4);
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

extern "C" void ShowMessage(char*, ...);

/* Function start: 0x44C480 */
void HashTable::CopyStrings(void* dest, int maxLen) {
    int overflow = 0;
    int* iter = (int*)((*(int*)(*(int*)((int)this + 8) + 8) == 0) - 1);
    do {
        if (iter == 0) {
            if (overflow) {
                ShowMessage("One or more strings in %s is too long");
            }
            return;
        }
        int* table = *(int**)((int)this + 8);
        if (iter == (int*)-1) {
            unsigned int idx = 0;
            if (table[1] != 0) {
                int* buckets = (int*)table[0];
                do {
                    iter = (int*)buckets[idx];
                    if (iter != 0) break;
                    idx++;
                } while (idx < (unsigned int)table[1]);
            }
        }
        int* nextIter = (int*)iter[0];
        if (nextIter == 0) {
            unsigned int idx = iter[1] + 1;
            if (idx < (unsigned int)table[1]) {
                int* buckets = (int*)(idx * 4 + table[0]);
                do {
                    nextIter = (int*)*buckets;
                    if (nextIter != 0) break;
                    buckets++;
                    idx++;
                } while (idx < (unsigned int)table[1]);
            }
        }
        char* str = (char*)iter[3];
        iter = nextIter;
        if (str != 0) {
            char buf[256];
            sprintf(buf, "%s\n", str);
            int len = strlen(buf);
            if (maxLen < len) {
                overflow = 1;
            } else {
                strcat((char*)dest, buf);
                maxLen -= len;
            }
        }
    } while (1);
}
