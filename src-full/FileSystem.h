#ifndef FILESYSTEM_H
#define FILESYSTEM_H

extern "C" {
    void GetCurrentDir(char* buffer, int size);
    char* FormatFilePath(char* pattern, ...);
}

#endif // FILESYSTEM_H
