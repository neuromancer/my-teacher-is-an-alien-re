#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <share.h>
#include "string.h"

// Based on the assembly, this function is a custom implementation of strncpy.
// It copies up to 'n' characters from src to dest, padding with nulls if src is shorter.
char* strncpy_custom(char* dest, const char* src, size_t n) {
    char* original_dest = dest;

    if (n == 0) {
        return original_dest;
    }

    // Copy characters from src to dest
    while (n > 0 && *src != '\0') {
        *dest++ = *src++;
        n--;
    }

    // Pad the rest of the buffer with nulls
    while (n > 0) {
        *dest++ = '\0';
        n--;
    }

    return original_dest;
}

// This is a highly literal C++ translation of the original assembly for strstr_custom (strstr).
// It uses 'goto' to precisely replicate the unconventional control flow of the 1997 compiler's
// output, which is necessary to produce a matching assembly file.

#include <windows.h>

extern void SetCursorVisible(int visible);
extern int GetWindowHandle_();
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
    HWND hWnd = (HWND)GetWindowHandle_();
    MessageBoxA(hWnd, buffer, "Error", 0x10);
    ShutdownGameSystems();
    exitWithError_(-1);
}

/* Function start: 0x419170 */
void ShowMessage(char *param_1, ...)
{
    char buffer[256];
    vsprintf(buffer, param_1, (char*)(&param_1 + 1));
    SetCursorVisible(1);
    HWND hWnd = (HWND)((int (*)())0x4224d0)();
    MessageBoxA(hWnd, buffer, "Message", 0);
    SetCursorVisible(0);
}

/* Function start: 0x426110 */
void DeleteFile_Wrapper(const char* filename)
{
    if (DeleteFileA(filename) == 0) {
        int error = GetLastError();
        if (error != 0) {
            FUN_0042b300(error);
        }
    }
}

/* Function start: 0x4191C0 */
void ClearMessageLog()
{
    DeleteFile_Wrapper("message.log");
}

/* Function start: 0x4191D0 */
void WriteToMessageLog(wchar_t *msg,...)
{
    FILE *_File;
    va_list argptr;

    _File = _fsopen("message.log", "a+", _SH_DENYNO);
    if (_File != NULL) {
        va_start(argptr, msg);
        vfwprintf(_File, msg, argptr);
        va_end(argptr);
        fwprintf(_File, L"\n");
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
    iVar2 = sscanf(param_1, "%s", local_20);
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

/* Function start: 0x4260F0 */
void ExecuteFunctionArray(void** param_1, void** param_2)
{
    if (param_1 < param_2) {
        do {
            if (*param_1 != 0) {
                ((void (*)(void)) *param_1)();
            }
            param_1 = param_1 + 1;
        } while (param_1 < param_2);
    }
}
