#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>

extern "C" {
    void GetCurrentDir(char* buffer, int size);
    char* GetDriveDir(unsigned int drive, char* buffer, size_t size);
    int __validdrive(unsigned int drive); // 0x42DF20
}

#endif // FILESYSTEM_H
