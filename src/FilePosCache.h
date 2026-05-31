#ifndef FILPOSCACHE_H
#define FILPOSCACHE_H

#include <string.h>

// FilePosCache — file position cache for Parser::FindKey optimization
// Pool stored at g_SoundPool_00469134 (SoundPool*)
// Entries are 0x50 bytes each

struct FilePosEntry {
    int accessCount;    // +0x00
    char filename[32];  // +0x04
    char key[36];       // +0x24
    int posLo;          // +0x48
    int posHi;          // +0x4C

    FilePosEntry(char* fname, char* keyName, int posLo, int posHi) {
        strcpy(filename, fname);
        strcpy(key, keyName);
        FilePosEntry::posLo = posLo;
        FilePosEntry::posHi = posHi;
        accessCount = 0;
    }
};

class FilePosCache {
public:
    __int64 Lookup(char* filename, char* key);   // 0x412130
    void Store(char* filename, char* key, int posLo, int posHi);  // 0x412210
};

// Lookup and Store are SoundTracker methods (0x412130, 0x412210)

#endif // FILPOSCACHE_H
