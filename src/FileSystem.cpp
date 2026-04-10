#include "FileSystem.h"
#include <windows.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern "C" int FileExists(const char*);
extern "C" {

/* Function start: 0x432E60 */ /* No assembly extracted */
void __cdecl GetCurrentDir(char *buffer, int size)
{
    GetCurrentDirectoryA(size, buffer);
}

// FormatFilePath (0x4260F0) = FormatAssetPath — moved to CDData.cpp

}
