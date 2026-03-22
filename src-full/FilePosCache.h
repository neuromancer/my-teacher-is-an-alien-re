#ifndef FILPOSCACHE_H
#define FILPOSCACHE_H

// FilePosCache — file position cache for Parser::FindKey optimization
// Pool stored at DAT_00469134 (SoundPool*)
// Entries are 0x50 bytes each

struct FilePosEntry {
    int accessCount;    // +0x00
    char filename[32];  // +0x04
    char key[36];       // +0x24
    int posLo;          // +0x48
    int posHi;          // +0x4C
};

class FilePosCache {
public:
    __int64 Lookup(char* filename, char* key);   // 0x412130
    void Store(char* filename, char* key, int posLo, int posHi);  // 0x412210
};

extern FilePosCache* g_FilePosCache_46928c;  // DAT_0046928c

#endif // FILPOSCACHE_H
