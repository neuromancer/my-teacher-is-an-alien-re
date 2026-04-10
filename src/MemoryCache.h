#ifndef MEMORYCACHE_H
#define MEMORYCACHE_H

// MemoryCache struct (24 bytes = 0x18) used by InitMemoryCache/CleanupMemoryCache
class MemoryCache {
public:
    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    int field_14;

    MemoryCache(int param) {
        field_8 = 0;
        field_c = 0;
        field_4 = 0;
        field_0 = 0;
        field_10 = 0;
        field_14 = param;
    }
};

#endif // MEMORYCACHE_H
