#include "FileSystem.h"
#include <windows.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern "C" int FileExists(const char*);
extern "C" {

/* Function start: 0x432E60 */
void __cdecl GetCurrentDir(char *buffer, int size)
{
    GetCurrentDirectoryA(size, buffer);
}

/* Function start: 0x4260F0 */
char* FormatFilePath(char* pattern, ...)
{
    char local_104[260];
    vsprintf(local_104, pattern, (char*)(&pattern + 1));

    int cddata = (int)DAT_0046aa1c;
    if (FileExists(local_104) == 0) {
        sprintf((char*)(cddata + 0x195), "%s%s", (char*)(cddata + 0x190), local_104);
    } else {
        strcpy((char*)(cddata + 0x195), local_104);
    }
    return (char*)(cddata + 0x195);
}

}
