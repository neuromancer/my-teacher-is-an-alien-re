#ifndef TEACHER_ANALYSIS_PRELUDE_H
#define TEACHER_ANALYSIS_PRELUDE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include "mbctype.h"
#include "mbstring.h"

#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __fastcall
#define __fastcall
#endif

#ifndef ZeroMemory
#define ZeroMemory(ptr, len) memset((ptr), 0, (len))
#endif

#ifndef _stricmp
#define _stricmp strcasecmp
#endif
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strcmpi
#define strcmpi strcasecmp
#endif
#ifndef _strcmpi
#define _strcmpi strcasecmp
#endif
#ifndef _strnicmp
#define _strnicmp strncasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif

#ifndef filelength
#define filelength _filelength
#endif

#if defined(__clang__)
#define TEACHER_ANALYSIS_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#define TEACHER_ANALYSIS_NORETURN __attribute__((noreturn))
#else
#define TEACHER_ANALYSIS_PRINTF(fmt, args)
#define TEACHER_ANALYSIS_NORETURN
#endif

extern "C" {
void ShowError(const char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2) TEACHER_ANALYSIS_NORETURN;
int ShowMessageYesNo(char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2);
char* FormatStringVA(char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2);
void WriteToLog(const char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2);
void WriteToMessageLog(const char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2);
char* ResolveAssetPath(char* name, ...) TEACHER_ANALYSIS_PRINTF(1, 2);
}

const char* __cdecl CDData_ResolvePath(const char* format, ...) TEACHER_ANALYSIS_PRINTF(1, 2);

class Parser;
Parser* ParseFile(Parser*, const char*, char*, ...);

#endif
