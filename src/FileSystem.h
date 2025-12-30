#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>

extern "C" {
    void FUN_0042ddd0(char* buffer, int size);
    char* FUN_0042ddf0(unsigned int drive, char* buffer, size_t size);
    int __validdrive(unsigned int drive); // 0x42DF20
}

#endif // FILESYSTEM_H
