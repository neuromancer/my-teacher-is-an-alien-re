#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <share.h>
#include <ctype.h>
#include <windows.h>
#include "string.h"
#include "Memory.h"
#include <mbstring.h>

extern "C" {
extern void SetCursorVisible(int visible);
extern void* GetGameWindowHandle();
}

extern void ShutdownGameSystems();
extern void exitWithErrorInternal(unsigned int param_1, int param_2, int param_3);
extern int g_ExitInProgress_0043be34;
extern char g_ExitCode_0043be30;
extern void* g_AtExitTableStart_0043f104;
extern void* g_AtExitTableEnd_0043f100;
extern int DAT_00435030;
extern int DAT_00435038;
extern int DAT_0043503c;
extern int DAT_00435040;

int DeleteFile_Wrapper(const char* filename);

// Message log enabled flag (was hardcoded at 0x43d5a8)
// File-local since only used in string.cpp
static char g_messageLogEnabled = 1;

// Static buffer for FormatStringVA at address 0x43cfe8
static char g_formatBuffer[64];

/* Function start: 0x40D200 */
char* FormatStringVA(char* format, ...)
{
    vsprintf(g_formatBuffer, format, (char*)(&format + 1));
    strchr(g_formatBuffer, '?');
    return g_formatBuffer;
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
    strncpy(param_2,pcVar1,iVar3);
    param_2[iVar3] = '\0';
    return;
}

/* Function start: 0x419110 */
void ShowError(const char* format, ...)
{
    char buffer[256];
    vsprintf(buffer, format, (char*)(&format + 1));
    SetCursorVisible(1);
    MessageBoxA((HWND)GetGameWindowHandle(), buffer, "Error", 0x10);
    ShutdownGameSystems();
    exitWithError_(-1);
}

/* Function start: 0x419170 */
void ShowMessage(char *param_1, ...)
{
    char buffer[256];
    vsprintf(buffer, param_1, (char*)(&param_1 + 1));
    SetCursorVisible(1);
    MessageBoxA((HWND)GetGameWindowHandle(), buffer, "Message", 0);
    SetCursorVisible(0);
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

    _File = fsopen("message.log", "a+");
    if (_File != NULL) {
        va_start(argptr, msg);
        vfprintf(_File, msg, argptr);
        va_end(argptr);
        fprintf(_File, "\n");
        fclose(_File);
    }
}

int g_stringTableCount = 0; // 0x4366b4
char g_stringTable[16384] = {0}; // 0x43d158

/* Function start: 0x419220 */
extern "C" void AddToStringTable(char *param_1)
{
    char local_20[32];
    int iVar2;

    local_20[0] = '\0';
    iVar2 = sscanf(param_1, " %s ", local_20);
    if (iVar2 == 1) {
        if (strlen(local_20) != 0) {
            strcpy(&g_stringTable[g_stringTableCount * 0x20], local_20);
            g_stringTableCount++;
        }
    }
}

/* Function start: 0x4192A0 */
void WriteToMessageLogIfEnabled(const char *param_1, ...)
{
    if ((g_messageLogEnabled & 1) != 0) {
        FILE *_File;
        va_list argptr;

        _File = fsopen("message.log", "a+");
        if (_File != NULL) {
            va_start(argptr, param_1);
            vfprintf(_File, param_1, argptr);
            va_end(argptr);
            fprintf(_File, "\n");
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
    if (param_3 <= 0) goto done;

outer_loop:
    if (*param_1 == '\0') goto done;

    do {
        if ((__mb_cur_max > 1 ? _isctype(*param_1, _SPACE) : _pctype[*param_1] & _SPACE) == 0) {
            break;
        }
        param_1 = param_1 + 1;
    } while (*param_1 != '\0');
    *param_2 = param_1;
    if (*param_1 != '\0') {
        do {
            if ((__mb_cur_max > 1 ? _isctype(*param_1, _SPACE) : _pctype[*param_1] & _SPACE) != 0) {
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
    if (iVar1 < param_3) goto outer_loop;

done:
    return iVar1;
}

/* Function start: 0x4260F0 */
void ExecuteFunctionArray(void** param_1, void** param_2)
{
    if (param_2 <= param_1) return;
    do {
        if (*param_1 != 0) {
            ((void (*)(void)) *param_1)();
        }
        param_1 = param_1 + 1;
    } while (param_2 > param_1);
}

/* Function start: 0x425E50 */
// Wrapper for _fsopen with _SH_DENYNO (0x40) share mode
FILE* fsopen(const char* filename, const char* mode)
{
    return _fsopen(filename, mode, _SH_DENYNO);
}

// NOTE: strstr_custom address unknown; 0x425FD0 is actually strncpy in the original binary
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
    g_ExitInProgress_0043be34 = 1;
    g_ExitCode_0043be30 = (char)param_3;
    if (param_2 == 0) {
        if ((g_AtExitTableStart_0043f104 != 0) && ((unsigned int)g_AtExitTableEnd_0043f100 - 4 >= (unsigned int)g_AtExitTableStart_0043f104)) {
            void** puVar1 = (void**)((char*)g_AtExitTableEnd_0043f100 - 4);
            do {
                if (*puVar1 != 0) {
                    ((void (*)(void)) *puVar1)();
                }
                puVar1 = puVar1 - 1;
            } while ((unsigned int)puVar1 >= (unsigned int)g_AtExitTableStart_0043f104);
        }
        ExecuteFunctionArray((void**)&DAT_00435030, (void**)&DAT_00435038);
    }
    ExecuteFunctionArray((void**)&DAT_0043503c, (void**)&DAT_00435040);
    if (param_3 == 0) {
        ExitProcess(param_1);
    }
}

/* Function start: 0x426110 */
int DeleteFile_Wrapper(const char* filename)
{
    unsigned int error;

    error = DeleteFileA(filename);
    if (error != 0) goto done;
    error = GetLastError();

done:
    if (error != 0) {
        SetErrorCode(error);
        return -1;
    }
    return 0;
}
