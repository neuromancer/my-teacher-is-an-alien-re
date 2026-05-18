#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <share.h>
#include <sys/stat.h>
#include <ctype.h>
#include <windows.h>
#include "string.h"
#include "Memory.h"
#include "globals.h"
#include "GameState.h"
#include <mbstring.h>

extern "C" {
extern void SetCursorVisible(int visible);
extern void* GetGameWindowHandle();
}

extern void ShutdownGameSystems();

int DeleteFile_Wrapper(const char* filename);

// Message log enabled flag (was hardcoded at 0x43d5a8)
// File-local since only used in string.cpp
static char g_messageLogEnabled = 1;

// Static buffer for FormatStringVA at address 0x43cfe8
static char g_formatBuffer[64];
static char g_audioNameBuffer[260];
static char g_soundFileBuffer[260];  // 0x473D10

// FormatStringVA - address unknown (NOT 0x44E3E0, that's MakeAnimName)
char* FormatStringVA(char* format, ...)
{
    vsprintf(g_formatBuffer, format, (char*)(&format + 1));
    strchr(g_formatBuffer, '?');
    return g_formatBuffer;
}

/* Function start: 0x44E470 */
char* MakeAudioName(char* baseName)
{
    int nameLength;
    int suffixValue;
    GameState* gameState;

    nameLength = strlen(baseName);
    if (nameLength < 4) {
        ShowError("MakeAudioName - invalid base name = '%s'", baseName);
    }

    suffixValue = atoi(baseName + nameLength - 4);
    if (suffixValue == 0) {
        return baseName;
    }

    if (suffixValue > 4999) {
        gameState = g_GameState_0046aa30;
        if (g_PeriodStateIdx_0046cb90 < 0 || gameState->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
            ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
        }
        sprintf(g_audioNameBuffer, "%s%c.wav", baseName,
                g_PeriodCharTable_0046cb94[gameState->stateValues[g_PeriodStateIdx_0046cb90]]);
        return g_audioNameBuffer;
    }

    sprintf(g_audioNameBuffer, "%s.wav", baseName);
    return g_audioNameBuffer;
}

/* Function start: 0x44E530 */
extern "C" char* GetSoundFilename(int handle)
{
    if (handle >= 0x1388) {
        int idx = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        char ch = g_PeriodCharTable_0046cb94[gs->stateValues[idx]];
        sprintf(g_soundFileBuffer, "audio\\snd%4.4d%c.wav", handle, (int)ch);
        return g_soundFileBuffer;
    }

    sprintf(g_soundFileBuffer, "audio\\snd%4.4d.wav", handle);
    return g_soundFileBuffer;
}

/* Function start: 0x425BC0 */
void ExtractQuotedString(char *param_1,char *param_2,int param_3)
{
    char *pcVar1;
    char *pcVar2;
    int iVar3;

    if ((param_1 == 0) || (param_2 == 0)) {
        ShowError("missing string");
    }
    pcVar1 = strchr(param_1,'\"');
    if (pcVar1 == 0) {
        *param_2 = '\0';
        return;
    }
    pcVar1 = pcVar1 + 1;
    pcVar2 = strchr(pcVar1,'\"');
    if (pcVar2 == 0) {
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

/* Function start: 0x425C50 */
void ShowError(const char* format, ...)
{
    char buffer[256];
    vsprintf(buffer, format, (char*)(&format + 1));
    SetCursorVisible(1);
    MessageBoxA((HWND)GetGameWindowHandle(), buffer, "Error", 0x10);
    ShutdownGameSystems();
    exit(-1);
}

/* Function start: 0x425CB0 */
void ShowMessage(char *param_1, ...)
{
    char buffer[256];
    vsprintf(buffer, param_1, (char*)(&param_1 + 1));
    SetCursorVisible(1);
    MessageBoxA((HWND)GetGameWindowHandle(), buffer, "Message", 0);
    SetCursorVisible(0);
}

/* Function start: 0x425D00 */
int ShowMessageYesNo(char *param_1, ...)
{
    char buffer[256];
    vsprintf(buffer, param_1, (char*)(&param_1 + 1));
    SetCursorVisible(1);
    int result = MessageBoxA((HWND)GetGameWindowHandle(), buffer, "Message", 4);
    SetCursorVisible(0);
    return result;
}

/* Function start: 0x425D60 */
void ClearMessageLog()
{
    remove("cfg\\message.log");
}

/* Function start: 0x425D70 */
void WriteToMessageLog(const char *msg,...)
{
    FILE *_File;
    va_list argptr;

    _File = fsopen("cfg\\message.log", "a+");
    if (_File != NULL) {
        va_start(argptr, msg);
        vfprintf(_File, msg, argptr);
        va_end(argptr);
        fprintf(_File, "\n");
        fclose(_File);
    }
}


// g_StringTableCount_0046aa80 — defined in globals.cpp
// g_StringTable_00472e30 — defined in globals.cpp

/* Function start: 0x425DC0 */
extern "C" void AddToStringTable(char *param_1)
{
    char local_20[32];
    int iVar2;

    local_20[0] = '\0';
    iVar2 = sscanf(param_1, " %s ", local_20);
    if (iVar2 == 1) {
        if (strlen(local_20) != 0) {
            strcpy(&g_StringTable_00472e30[g_StringTableCount_0046aa80 * 0x20], local_20);
            g_StringTableCount_0046aa80++;
        }
    }
}

/* Function start: 0x425E40 */
extern "C" void WriteToLog(const char *param_1, ...)
{
    if (!(g_LogEnabled_00472e28 & 1)) return;

    FILE* fp = fsopen("cfg\\message.log", "a");
    if (fp == 0) return;

    vfprintf(fp, param_1, (char*)(&param_1 + 1));
    fprintf(fp, "\n");
    fclose(fp);
}

extern "C" {
FILE *fsopen(const char* filename, const char* mode);
void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext);
void* AllocateMemory_Wrapper(int size);
}



/* Function start: 0x4263E0 */
void DecryptLine(char* buffer)
{
    int i = 0;
    if (buffer[0] == '\n') {
        return;
    }
    do {
        buffer[i] ^= 0xFE;
        i++;
    } while (buffer[i] != '\n');
}

/* Function start: 0x426400 */
char* internal_ReadLine(char* buffer, int size, FILE* stream)
{
    char local_buf[128];
    int result;
    char* line;
    char* semi;

    do {
        line = fgets(buffer, size, stream);
        if (line == NULL) {
            return NULL;
        }
        DecryptLine(buffer);
        result = sscanf(buffer, " %s ", local_buf);
    } while (result < 1 || local_buf[0] == ';' || local_buf[0] == '\r');

    semi = strchr(buffer, ';');
    if (semi != NULL) {
        *semi = '\0';
    }

    return buffer;
}

/* Function start: 0x425FC0 */
int GetFileSize(char* path) {
    struct _stat statbuf;
    if (_stat(path, &statbuf) != 0) {
        return -1;
    }
    return statbuf.st_size;
}

/* Function start: 0x426490 */
void EncryptAndWrite(char* buffer, FILE* file)
{
    DecryptLine(buffer);
    fputs(buffer, file);
}

/* Function start: 0x4264B0 */
int ParseCommandLineArgs(char *param_1, char **param_2, int param_3)
{
    int iVar1;

    iVar1 = 0;
    if (param_3 > 0) {
        do {
            if (*param_1 == '\0') break;

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
        } while (iVar1 < param_3);
    }
    return iVar1;
}

/* Function start: 0x455110 */
FILE* fsopen(const char* filename, const char* mode)
{
    return _fsopen(filename, mode, _SH_DENYNO);
}

static char g_AnimNameBuf[64];  // DAT_00473cf0
static char g_CineNameBuf[64]; // DAT_00473cb0

/* Function start: 0x44E320 */
char* MakeSoundName(char* baseName) {
    int len = strlen(baseName);
    if (len < 4) {
        ShowError("MakeAnimName - invalid base name = '%s'", baseName);
    }
    int index = atoi(baseName + (len - 4));
    if (index == 0) {
        return baseName;
    }
    if (index >= 5000) {
        int gsIdx = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
            ShowError("Invalid gamestate %d", gsIdx);
        }
        sprintf(g_AnimNameBuf, "%s%c.smk", baseName,
                (int)(char)g_PeriodCharTable_0046cb94[gs->stateValues[gsIdx]]);
        return g_AnimNameBuf;
    }
    sprintf(g_AnimNameBuf, "%s.smk", baseName);
    return g_AnimNameBuf;
}

/* Function start: 0x44E3E0 */
char* MakeAnimName(int index) {
    if (index >= 5000) {
        int gsIdx = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
            ShowError("Invalid gamestate %d", gsIdx);
        }
        sprintf(g_CineNameBuf, "cine\\cin%4.4d%c.smk", index,
                (int)(char)g_PeriodCharTable_0046cb94[gs->stateValues[gsIdx]]);
        return g_CineNameBuf;
    }
    sprintf(g_CineNameBuf, "cine\\cin%4.4d.smk", index);
    return g_CineNameBuf;
}
