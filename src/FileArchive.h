#ifndef FILEARCHIVE_H
#define FILEARCHIVE_H

#include <stdio.h>
#include <string.h>

// FileArchive - save/load wrapper passed as `void* param` to Serialize() methods.
// Size: 0x48 bytes
struct FileArchive {
    int mode;           // 0x00 - 0=read, 1=write
    char filename[64];  // 0x04-0x43
    FILE* fp;           // 0x44

    FileArchive() {
        memset(this, 0, 0x48);
    }
    FileArchive(char* path) {
        memset(this, 0, 0x48);
        strcpy(filename, path);
    }
    ~FileArchive() {
        if (fp != 0) {
            fclose(fp);
            fp = 0;
        }
    }
};

#endif // FILEARCHIVE_H
