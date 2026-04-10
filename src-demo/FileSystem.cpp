#include "FileSystem.h"
#include <windows.h>
#include "globals.h"
#include "Memory.h"
#include <string.h>
#include <mbstring.h>
#include <direct.h>

extern "C" {
    int DateTimeToTimestamp(int year, int month, int day, int hour, int minute, int second);
}

extern char g_WildcardChars[];
extern char g_PathSeparator[];

extern "C" {
    char* __cdecl GetFullPath(char *dst, const char *src, unsigned int maxlen);
    void __cdecl SetErrorCode(unsigned int errorCode);
}

extern "C" {

/* Function start: 0x426470 */
int __cdecl ___dtoxmode(unsigned int attr, const unsigned char* filename)
{
    const unsigned char* p = filename;
    unsigned short mode;

    if (filename[1] == ':') {
        p = filename + 2;
    }

    if ((*p == '\\' || *p == '/') && p[1] == 0) {
        mode = 0x4040;
    } else if ((attr & 0x10) != 0 || *p == 0) {
        mode = 0x4040;
    } else {
        mode = 0x8000;
    }

    if ((attr & 1) == 0) {
        mode = mode | 0x80;
    }
    mode = mode | 0x100;

    return mode | ((mode & 0x1c0) >> 3) | ((mode & 0x1c0) >> 6);
}

/* Function start: 0x426550 */
int __cdecl FileStat(const unsigned char* filename, int* stat_buf)
{
    SYSTEMTIME sysTime;
    FILETIME localTime;
    WIN32_FIND_DATAA findData;
    char rootPath[260];
    int drive;
    int timestamp;
    HANDLE hFind;
    char* fullPath;

    if (_mbspbrk(filename, (const unsigned char*)"*?") != 0) {
        DAT_0043bdf0 = 2;
        g_ErrorCode_0043bdf4 = 2;
        return -1;
    }

    if (filename[1] == ':') {
        if (filename[0] != 0 && filename[2] == 0) {
            DAT_0043bdf0 = 2;
            g_ErrorCode_0043bdf4 = 2;
            return -1;
        }
        drive = _mbctolower((int)(char)filename[0]) - 0x60;
    } else {
        drive = _getdrive();
    }

    hFind = FindFirstFileA((LPCSTR)filename, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        if (_mbspbrk(filename, (const unsigned char*)"\\") != 0) {
            fullPath = GetFullPath(rootPath, (const char*)filename, 0x104);
            if (fullPath != 0) {
                if (strlen(fullPath) == 3) {
                    if (GetDriveTypeA(fullPath) > 1) {
                        findData.nFileSizeHigh = 0;
                        findData.nFileSizeLow = 0;
                        findData.cFileName[0] = 0;
                        findData.dwFileAttributes = 0x10;
                        timestamp = DateTimeToTimestamp(1980, 1, 1, 0, 0, 0);
                        stat_buf[7] = timestamp;
                        stat_buf[6] = timestamp;
                        stat_buf[8] = timestamp;
                        goto fill_stat;
                    }
                }
            }
        }
        DAT_0043bdf0 = 2;
        g_ErrorCode_0043bdf4 = 2;
        return -1;
    }

    if (FileTimeToLocalFileTime(&findData.ftLastWriteTime, &localTime) == 0 ||
        FileTimeToSystemTime(&localTime, &sysTime) == 0) {
        SetErrorCode(GetLastError());
        FindClose(hFind);
        return -1;
    }

    timestamp = DateTimeToTimestamp(sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                                sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    stat_buf[7] = timestamp;

    if (findData.ftLastAccessTime.dwLowDateTime != 0 || findData.ftLastAccessTime.dwHighDateTime != 0) {
        if (FileTimeToLocalFileTime(&findData.ftLastAccessTime, &localTime) == 0 ||
            FileTimeToSystemTime(&localTime, &sysTime) == 0) {
            SetErrorCode(GetLastError());
            FindClose(hFind);
            return -1;
        }
        timestamp = DateTimeToTimestamp(sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                                        sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    }
    stat_buf[6] = timestamp;

    if (findData.ftCreationTime.dwLowDateTime == 0 && findData.ftCreationTime.dwHighDateTime == 0) {
        timestamp = stat_buf[7];
    } else {
        if (FileTimeToLocalFileTime(&findData.ftCreationTime, &localTime) == 0 ||
            FileTimeToSystemTime(&localTime, &sysTime) == 0) {
            SetErrorCode(GetLastError());
            FindClose(hFind);
            return -1;
        }
        timestamp = DateTimeToTimestamp(sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                                    sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    }
    stat_buf[8] = timestamp;
    FindClose(hFind);

fill_stat:
    *(short*)((char*)stat_buf + 6) = (short)___dtoxmode(findData.dwFileAttributes, filename);
    *(short*)(stat_buf + 2) = 1;
    stat_buf[5] = findData.nFileSizeLow;
    stat_buf[0] = drive - 1;
    stat_buf[4] = drive - 1;
    *(short*)(stat_buf + 1) = 0;
    *(short*)(stat_buf + 3) = 0;
    *(short*)((char*)stat_buf + 10) = 0;
    return 0;
}

/* Function start: 0x42DDD0 */
void __cdecl GetCurrentDir(char *buffer, int size)
{
    GetDriveDir(0, buffer, size);
}

/* Function start: 0x42DDF0 */
char * __cdecl GetDriveDir(unsigned int drive, char *buffer, size_t size)
{
    DWORD len;
    size_t uVar4;
    char drive_str[4];
    LPSTR file_part;
    char local_path[260];

    if (drive != 0) {
        if (__validdrive(drive) == 0) {
             g_ErrorCode_0043bdf4 = 0xf;
             DAT_0043bdf0 = 0xd;
             return 0;
        }
        drive_str[0] = (char)drive + '@';
        drive_str[1] = ':';
        drive_str[2] = '.';
        drive_str[3] = 0;
        len = GetFullPathNameA(drive_str, 260, local_path, &file_part);
    } else {
        len = GetCurrentDirectoryA(260, local_path);
    }
    
    if (len == 0 || (uVar4 = len + 1, 260 < uVar4)) {
        return 0;
    }

    if (buffer == 0) {
        if ((int)uVar4 <= (int)size) {
            uVar4 = size;
        }
        buffer = (char*)CrtMalloc(uVar4);
        if (buffer == 0) {
            DAT_0043bdf0 = 0xc;
            return 0;
        }
    } else if ((int)size < (int)uVar4) {
        DAT_0043bdf0 = 0x22;
        return 0;
    }

    strcpy(buffer, local_path);
    
    return buffer;
}

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

// Path formatting stub - returns path unchanged
char* FormatFilePath(char* path)
{
    return path;
}

}
