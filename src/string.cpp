#include <stdio.h>
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

/*
Function: ShowError
Address: 0x419110

Disassembled:
LEA EAX,[ESP + 0x8]
MOV ECX,dword ptr [ESP + 0x4]
SUB ESP,0x100
LEA EDX,[ESP]
PUSH EAX
PUSH ECX
PUSH EDX
CALL 0x00424cb0
ADD ESP,0xc
PUSH 0x1
CALL 0x00422d98
LEA ECX,[ESP + 0x4]
ADD ESP,0x4
PUSH 0x10
PUSH 0x4366fc
PUSH ECX
CALL 0x004224d0
PUSH EAX
CALL dword ptr [0x0044044c]
CALL 0x0041a550
PUSH -0x1
CALL 0x00426030
*/

/*
Function: exitWithError_
Address: 0x426030

Disassembled:
MOV EAX,dword ptr [ESP + 0x4]
PUSH 0x0
PUSH 0x0
PUSH EAX
CALL 0x00426070
*/
void exitWithError_(unsigned int param_1)
{
    exitWithErrorInternal(param_1, 0, 0);
}

/*
Function: exitWithErrorInternal
Address: 0x426070

Disassembled:
MOV dword ptr [0x0043be34],0x1
CMP dword ptr [ESP + 0x8],0x0
PUSH EBX
PUSH ESI
MOV EBX,dword ptr [ESP + 0x14]
MOV byte ptr [0x0043be30],BL
JNZ 0x5C
CMP dword ptr [0x0043f104],0x0
JZ 0x4A
MOV ESI,dword ptr [0x0043f100]
SUB ESI,0x4
CMP ESI,dword ptr [0x0043f104]
JC 0x004260ba
MOV EAX,dword ptr [ESI]
TEST EAX,EAX
JZ 0x3F
CALL EAX
SUB ESI,0x4
CMP ESI,dword ptr [0x0043f104]
JNC 0x004260a7
PUSH 0x435038
PUSH 0x435030
CALL 0x004260f0
ADD ESP,0x8
PUSH 0x435040
PUSH 0x43503c
CALL 0x004260f0
ADD ESP,0x8
TEST EBX,EBX
JNZ 0x7D
MOV EAX,dword ptr [ESP + 0xc]
PUSH EAX
CALL dword ptr [0x00440384]
POP ESI
POP EBX
RET
*/
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

/*
Function: ExecuteFunctionArray
Address: 0x4260F0

Disassembled:
PUSH ESI
PUSH EDI
MOV ESI,dword ptr [ESP + 0x10]
MOV EDI,dword ptr [ESP + 0xc]
CMP ESI,EDI
JBE 0x1D
MOV EAX,dword ptr [EDI]
TEST EAX,EAX
JZ 0x16
CALL EAX
ADD EDI,0x4
CMP ESI,EDI
JA 0xE
POP EDI
POP ESI
RET
*/
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
