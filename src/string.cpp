#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <share.h>
#include <ctype.h>
#include "string.h"
#include "Memory.h"
#include <mbstring.h>

/* Function start: 0x425E50 */
// Wrapper for _fsopen with _SH_DENYNO (0x40) share mode
FILE* fsopen(const char* filename, const char* mode)
{
    return _fsopen(filename, mode, _SH_DENYNO);
}

/* Function start: 0x425FD0 */
/* This is strncpy from MSVC CRT - the assembly uses LODSB/STOSB/LOOP string instructions */
/* which are characteristic of the statically linked CRT implementation */


// This is a highly literal C++ translation of the original assembly for strstr_custom (strstr).
// It uses 'goto' to precisely replicate the unconventional control flow of the 1997 compiler's
// output, which is necessary to produce a matching assembly file.

#include <windows.h>

extern "C" {
extern void SetCursorVisible(int visible);
extern void* GetGameWindowHandle();
}

extern void ShutdownGameSystems();
extern void exitWithErrorInternal(unsigned int param_1, int param_2, int param_3);
extern int ExecuteFunctionArray(void* param_1, void* param_2);
extern int DAT_0043be34;
extern char DAT_0043be30;
extern void* DAT_0043f104;
extern void* DAT_0043f100;
extern int DAT_00435030;
extern int DAT_00435038;
extern int DAT_0043503c;
extern int DAT_00435040;


char* strstr_custom(const char* haystack, const char* needle) {
    const char* haystack_base = haystack;
    const char* needle_base = needle;

    if (*needle_base == '\0') {
        return (char*)haystack;
    }

loop_start:
    if (*haystack == '\0') {
        return NULL;
    }

    if (*haystack == *needle) {
        // A character matches. Advance both pointers.
        haystack++;
        needle++;
        if (*needle == '\0') {
            // We've reached the end of the needle, so we have a full match.
            return (char*)haystack_base;
        }
        // This is the key part: jump back to the top to re-evaluate the loop conditions
        // for the next character, rather than using a nested loop.
        goto loop_start;
    } else {
        // The characters do not match.
        // Advance the base haystack pointer and reset the other pointers.
        haystack_base++;
        haystack = haystack_base;
        needle = needle_base;
        goto loop_start;
    }
}

/* Function start 0x426030 */
void exitWithError_(unsigned int param_1)
{
    exitWithErrorInternal(param_1, 0, 0);
}

/* 0x426070 */
void exitWithErrorInternal(unsigned int param_1, int param_2, int param_3)
{
    DAT_0043be34 = 1;
    DAT_0043be30 = (char)param_3;
    if (param_2 == 0) {
        if ((DAT_0043f104 != 0) && ((unsigned int)DAT_0043f100 - 4 >= (unsigned int)DAT_0043f104)) {
            void** puVar1 = (void**)((char*)DAT_0043f100 - 4);
            do {
                if (*puVar1 != 0) {
                    ((void (*)(void)) *puVar1)();
                }
                puVar1 = puVar1 - 1;
            } while ((unsigned int)puVar1 >= (unsigned int)DAT_0043f104);
        }
        ExecuteFunctionArray(&DAT_00435030, &DAT_00435038);
    }
    ExecuteFunctionArray(&DAT_0043503c, &DAT_00435040);
    if (param_3 == 0) {
        ExitProcess(param_1);
    }
}

/* Function start: 0x419080 */
void ExtractQuotedString(char *param_1,char *param_2,int param_3)
{
    char *pcVar1;
    char *pcVar2;
    int iVar3;

    if ((param_1 == (char *)0x0) || (param_2 == (char *)0x0)) {
        ShowError("missing string");
    }
    pcVar1 = strchr(param_1,'\"');
    if (pcVar1 == (char *)0x0) {
        *param_2 = '\0';
        return;
    }
    pcVar1 = pcVar1 + 1;
    pcVar2 = strchr(pcVar1,'\"');
    if (pcVar2 == (char *)0x0) {
        ShowError("open quote found in '%s'",param_1);
    }
    iVar3 = (int)pcVar2 - (int)pcVar1;
    if (param_3 < iVar3 + 1) {
        ShowError("dest string too small");
    }
    memcpy(param_2,pcVar1,iVar3);
    param_2[iVar3] = '\0';
    return;
}

/* Function start: 0x419110 */
void ShowError(const char* format, ...)
{
    char buffer[256];
    vsprintf(buffer, format, (char*)(&format + 1));
    SetCursorVisible(1);
    char* lpText = buffer;
    HWND hWnd = (HWND)GetGameWindowHandle();
    MessageBoxA(hWnd, lpText, "Error", 0x10);
    ShutdownGameSystems();
    exitWithError_(-1);
}

/* Function start: 0x419170 */
void ShowMessage(char *param_1, ...)
{
    char buffer[256];
    vsprintf(buffer, param_1, (char*)(&param_1 + 1));
    SetCursorVisible(1);
    // extern decl moved to global scope
    HWND hWnd = (HWND)GetGameWindowHandle();
    MessageBoxA(hWnd, buffer, "Message", 0);
    SetCursorVisible(0);
}

/* Function start: 0x426110 */
void DeleteFile_Wrapper(const char* filename)
{
    if (DeleteFileA(filename) == 0) {
        int error = GetLastError();
        if (error != 0) {
            SetErrorCode(error);
        }
    }
}

/* Function start: 0x4191C0 */
void ClearMessageLog()
{
    DeleteFile_Wrapper("message.log");
}

/* Function start: 0x4191D0 */
void WriteToMessageLog(const char *msg,...)
{
    FILE *_File;
    va_list argptr;

    _File = _fsopen("message.log", "a+", _SH_DENYNO);
    if (_File != NULL) {
        va_start(argptr, msg);
        vfprintf(_File, msg, argptr);
        va_end(argptr);
        fprintf(_File, "\n");
        fclose(_File);
    }
}

/* Function start: 0x419220 */
void AddToStringTable(char *param_1)
{
    char local_20[32];
    int iVar2;
    unsigned int uVar3;
    char *pcVar5;
    char *pcVar6;

    local_20[0] = '\0';
    iVar2 = sscanf(param_1, " %s ", local_20);
    if (iVar2 == 1) {
        uVar3 = strlen(local_20);
        if (uVar3 > 0) {
            pcVar5 = local_20;
            pcVar6 = &((char*)0x43d158)[*(int*)0x4366b4 * 0x20];
            memcpy(pcVar6, pcVar5, uVar3);
            (*(int*)0x4366b4)++;
        }
    }
}

/* Function start: 0x4192A0 */
void WriteToMessageLogIfEnabled(wchar_t *param_1, ...)
{
    if ((*(char*)0x43d5a8 & 1) != 0) {
        FILE *_File;
        va_list argptr;

        _File = _fsopen("message.log", "a+", _SH_DENYNO);
        if (_File != NULL) {
            va_start(argptr, param_1);
            vfwprintf(_File, param_1, argptr);
            va_end(argptr);
            fwprintf(_File, L"\n");
            fclose(_File);
        }
    }
}

extern "C" {
FILE *fsopen(const char* filename, const char* mode);
void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext);
int FileStat(const unsigned char* filename, int* stat_buf);
char* FormatFilePath(char* path);
void* AllocateMemory_Wrapper(int size);
}

/* Function start: 0x419420 */
FILE* OpenFileAndFindKey(char* archive_path, char* filename, const char* mode, unsigned int* out_size)
{
    FILE* fp;
    char* entry_buf;
    int found;
    long offset;
    char fname[12];
    char ext[8];
    char key[16];
    int stat_buf[5];

    if (out_size != NULL) {
        *out_size = 0;
    }

    if (archive_path == NULL) {
        fp = fsopen(filename, mode);
        if (fp != NULL && out_size != NULL) {
            FileStat((const unsigned char*)filename, stat_buf);
            *out_size = stat_buf[5];
        }
        return fp;
    }

    stat_buf[0] = 0;
    offset = 0;
    ParsePath(filename, NULL, NULL, fname, ext);
    sprintf(key, "%s%s", fname, ext);

    fp = fsopen(archive_path, mode);
    if (fp == NULL) {
        char* formatted = FormatFilePath(archive_path);
        fp = fsopen(formatted, mode);
        if (fp == NULL) {
            goto not_found;
        }
    }

    found = 1;
    entry_buf = (char*)AllocateMemory_Wrapper(0x18);

    do {
        while (found) {
            fread(entry_buf, 0x18, 1, fp);
            if (*entry_buf != (char)0xff) {
                break;
            }
            if (*(int*)(entry_buf + 0xc) == 0) {
                found = 0;
            } else {
                fseek(fp, *(long*)(entry_buf + 0xc), 0);
            }
        }
        if (!found) break;
    } while (_strnicmp(entry_buf, key, 0xc) != 0);

    if (found) {
        offset = *(long*)(entry_buf + 0xc);
        stat_buf[0] = 1;
        if (out_size != NULL) {
            *out_size = *(unsigned int*)(entry_buf + 0x10);
        }
    }

    FreeFromGlobalHeap(entry_buf);

not_found:
    if (stat_buf[0] != 0) {
        fseek(fp, offset, 0);
        return fp;
    }

    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
    return fp;
}

/* Function start: 0x419770 */
void internal_ReadLine_placeholder(void)
{
    return;
}

/* Function start: 0x419780 */
char* internal_ReadLine(char* buffer, int size, FILE* stream)
{
    char local_buf[128];
    int result;
    char* line;

    do {
        line = fgets(buffer, size, stream);
        if (line == NULL) {
            return NULL;
        }
        internal_ReadLine_placeholder();
        result = sscanf(buffer, " %s ", local_buf);
    } while (result < 1 || local_buf[0] == ';' || local_buf[0] == '\r');

    return buffer;
}

/* Function start: 0x419800 */
int ParseCommandLineArgs(char *param_1, char **param_2, int param_3)
{
    int iVar1;
    
    iVar1 = 0;
    if (param_3 > 0) {
        if (*param_1 != '\0') {
            do {
                do {
                    if (_isctype(*param_1, 0x8) == 0) {
                        break;
                    }
                    param_1 = param_1 + 1;
                } while (*param_1 != '\0');
                *param_2 = param_1;
                if (*param_1 != '\0') {
                    do {
                        if (_isctype(*param_1, 0x8) != 0) {
                            break;
                        }
                        param_1 = param_1 + 1;
                    } while (*param_1 != '\0');
                    if (*param_1 != '\0') {
                        *param_1 = '\0';
                        param_1 = param_1 + 1;
                    }
                }
                param_2 = param_2 + 1;
                iVar1 = iVar1 + 1;
            } while (iVar1 < param_3);
        }
    }
    return iVar1;
}

/* Function start: 0x4260F0 */
int ExecuteFunctionArray(void* param_1, void* param_2)
{
    void** p1 = (void**)param_1;
    void** p2 = (void**)param_2;
    if (p1 < p2) {
        do {
            if (*p1 != 0) {
                ((void (*)(void)) *p1)();
            }
            p1 = p1 + 1;
        } while (p1 < p2);
    }
    return 0;
}
