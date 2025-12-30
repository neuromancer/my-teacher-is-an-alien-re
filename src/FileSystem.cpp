#include "FileSystem.h"
#include <windows.h>
#include "globals.h"
#include "Memory.h"
#include <string.h>

extern "C" {

/* Function start: 0x42DF20 */
int __cdecl __validdrive(unsigned int drive)
{
    unsigned int type;
    char path[4];

    if (drive == 0) {
        return 1;
    }
    path[1] = ':';
    path[2] = '\\';
    path[3] = 0;
    path[0] = (char)drive + '@';
    type = GetDriveTypeA(path);
    if (type != 0 && type != 1) {
        return 1;
    }
    return 0;
}

/* Function start: 0x42DDF0 */
char * __cdecl FUN_0042ddf0(unsigned int drive, char *buffer, size_t size)
{
    DWORD len;
    size_t uVar4;
    char local_path[260];
    char drive_str[4];
    LPSTR file_part;

    if (drive == 0) {
        len = GetCurrentDirectoryA(260, local_path);
    } else {
        if (__validdrive(drive) == 0) {
             DAT_0043bdf4 = 0xf;
             DAT_0043bdf0 = 0xd;
             return 0;
        }
        drive_str[0] = (char)drive + '@';
        drive_str[1] = ':';
        drive_str[2] = '.';
        drive_str[3] = 0;
        len = GetFullPathNameA(drive_str, 260, local_path, &file_part);
    }
    
    if (len == 0 || (uVar4 = len + 1, 260 < uVar4)) {
        return 0;
    }

    if (buffer == 0) {
        if (uVar4 <= size) {
            uVar4 = size;
        }
        buffer = (char*)FUN_00428440(uVar4);
        if (buffer == 0) {
            DAT_0043bdf0 = 0xc;
            return 0;
        }
    } else if (size < uVar4) {
        DAT_0043bdf0 = 0x22;
        return 0;
    }

    strcpy(buffer, local_path);
    
    return buffer;
}

/* Function start: 0x42DDD0 */
void __cdecl FUN_0042ddd0(char *buffer, int size)
{
    FUN_0042ddf0(0, buffer, size);
}

}
