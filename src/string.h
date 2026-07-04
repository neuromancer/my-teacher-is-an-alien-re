#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdio.h>


    char* FindAfterSubstring(char* s1, char* s2); // 0x426570 - strstr + skip past match
    void ShowError(const char* format, ...);
    void ExtractQuotedString(char *param_1,char *param_2,int param_3);
    void ShowMessage(char *param_1, ...);
    int ShowMessageYesNo(char *param_1, ...);
    char* FormatStringVA(char* format, ...);
    char* MakeAudioName(char* baseName);
    void ClearMessageLog();
    void WriteToMessageLog(const char *msg,...);
    void AddToStringTable(char *param_1);
    void WriteToLog(const char *param_1, ...);
    FILE* fsopen(const char* filename, const char* mode);
    char* internal_ReadLine(char* buffer, int size, FILE* stream);
    int GetFileSize(char* path);

char* GetSoundFilename(int handle);


#endif // STRING_H
