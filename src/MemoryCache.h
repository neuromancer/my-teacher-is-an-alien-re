#ifndef MEMORYCACHE_H
#define MEMORYCACHE_H

// FileCacheEntry — 0x2C bytes. Allocated via operator new in FileCacheRegister.
// Points to from CacheNode::entry.
struct FileCacheEntry {
    char name[0x20];        // 0x00 — cached file path (copied from argument)
    int size;               // 0x20 — reported file size in bytes
    int hitCount;           // 0x24 — incremented on each FileCacheLookup hit
    unsigned int tickTime;  // 0x28 — GetTickCount() at registration / last access
};

// CacheNode — 12-byte linked-list node used by MemoryCache. Pool-allocated
// in blocks; free nodes are chained through the `next` field.
struct CacheNode {
    CacheNode* next;        // 0x00 — next in LRU list (or free list)
    CacheNode* prev;        // 0x04 — prev in LRU list
    FileCacheEntry* entry;  // 0x08 — the cached entry this node represents
};

// MemoryCache struct (24 bytes = 0x18) used by InitMemoryCache/CleanupMemoryCache
class MemoryCache {
public:
    CacheNode* head;        // 0x00 — most-recently used
    CacheNode* tail;        // 0x04 — least-recently used
    int count;              // 0x08 — number of active entries
    CacheNode* freeList;    // 0x0C — free-node chain
    int* blockList;         // 0x10 — allocated node-block chain
    int poolCapacity;       // 0x14 — nodes per pool block

    MemoryCache(int param) {
        count = 0;
        freeList = 0;
        tail = 0;
        head = 0;
        blockList = 0;
        poolCapacity = param;
    }
};

#endif // MEMORYCACHE_H
