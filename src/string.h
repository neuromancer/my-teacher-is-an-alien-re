#ifndef STRING_H
#define STRING_H

#include <stddef.h>

extern "C" {
    char* strncpy_custom(char* dest, const char* src, size_t n);
    char* strstr_custom(const char* haystack, const char* needle);
    void ExecuteFunctionArray(void** param_1, void** param_2);
    void ShowError(const char* format, ...);
    void exitWithError_(unsigned int param_1);
    void exitWithErrorInternal(unsigned int param_1, int param_2, int param_3);
    int GetWindowHandle_();
    void ExtractQuotedString(char *param_1,char *param_2,int param_3);
    void ShowMessage(char *param_1, ...);
    void ClearMessageLog();
    void WriteToMessageLog(wchar_t *msg,...);
    void AddToStringTable(char *param_1);
    void WriteToMessageLogIfEnabled(wchar_t *param_1, ...);
}

#endif // STRING_H
