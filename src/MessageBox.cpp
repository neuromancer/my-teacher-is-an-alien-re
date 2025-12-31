#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

extern "C" {
void* GetGameWindowHandle();
int SetCursorVisible(unsigned int);
}

/* Function start: 0x419170 */
void ShowMessage(const char *param_1, ...)
{
    char local_100[256];
    va_list args;
    
    va_start(args, param_1);
    vsprintf(local_100, param_1, args);
    va_end(args);
    
    SetCursorVisible(1);
    MessageBoxA((HWND)GetGameWindowHandle(), local_100, "Message", 0);
    SetCursorVisible(0);
}
