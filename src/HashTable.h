#ifndef HASHTABLE_H
#define HASHTABLE_H

struct HashNode {
    HashNode* next;      // 0x00
    int bucketIndex;     // 0x04
    unsigned int key;    // 0x08
    int reserved;        // 0x0C
    int filePosLow;      // 0x10
    int filePosHigh;     // 0x14
};

class HashTable {
public:
    int* buckets;        // 0x00
    int numBuckets;      // 0x04
    int count;           // 0x08
    HashNode* freeList;  // 0x0C
    void* nodePool;      // 0x10
    int capacity;        // 0x14

    HashTable(unsigned int cap) {
        buckets = 0;
        numBuckets = 0x11;
        count = 0;
        freeList = 0;
        nodePool = 0;
        capacity = cap;
    }
    ~HashTable();
    void AllocateBuckets(int numBuckets, int allocateNow);
    HashNode* AllocateNode();
};

#endif
