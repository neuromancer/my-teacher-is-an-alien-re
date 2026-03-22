#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdio.h>

extern "C" {
    char* strstr_custom(const char* haystack, const char* needle);
    char* FindAfterSubstring(char* s1, char* s2); // 0x426570 - strstr + skip past match
    void ExecuteFunctionArray(void** param_1, void** param_2);
    void ShowError(const char* format, ...);
    void exitWithError_(unsigned int param_1);
    void exitWithErrorInternal(unsigned int param_1, int param_2, int param_3);
    void ExtractQuotedString(char *param_1,char *param_2,int param_3);
    void ShowMessage(char *param_1, ...);
    int ShowMessageYesNo(char *param_1, ...);
    char* FormatStringVA(char* format, ...);
    char* MakeAudioName(char* baseName);
    void ClearMessageLog();
    void WriteToMessageLog(const char *msg,...);
    void AddToStringTable(char *param_1);
    void WriteToLog(const char *param_1, ...);
    void SetErrorCode(unsigned int);
    FILE* OpenFileAndFindKey(char* archive_path, char* filename, const char* mode, unsigned int* out_size);
    FILE* fsopen(const char* filename, const char* mode);
    char* internal_ReadLine(char* buffer, int size, FILE* stream);
}

#endif // STRING_H
