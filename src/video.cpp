#include <windows.h>
#include <string.h>

extern "C" {
    extern int DAT_00437f54;
    extern int DAT_00437f62;
    extern void *DAT_00437f66;
    extern short DAT_00437f6a;
    extern int DAT_00437491;
    extern char DAT_00437495;
    extern char DAT_00437490;
    extern char DAT_00437520[];
}

/*
Function: ClearScreen
Address: 0x423099

PUSH EDI
PUSH ES
CLD
CMP byte ptr [0x00437f54],0x0
JGE 0xE
JMP 0x3B
MOV ES,word ptr [0x00437f6a]
MOV EDI,dword ptr [0x00437f66]
XOR EDX,EDX
MOV ECX,dword ptr [0x00437f62]
MOV EAX,[0x00437491]
SHR ECX,0x1
PUSHF
SHR ECX,0x1
STOSD.REP ES:EDI
ADC ECX,ECX
STOSW.REP ES:EDI
POPF
ADC ECX,ECX
STOSB.REP ES:EDI
XOR EAX,EAX
POP ES
POP EDI
RET
*/
int ClearScreen(void)
{
    if (DAT_00437f54 >= 0) {
        // This function clears the screen by writing a fill value to the video memory.
        // The original assembly code is highly optimized and uses 16-bit segment registers (es)
        // which are not directly accessible in 32-bit C++ without inline assembly.
        // The MSVC 4.2 compiler is not able to generate the exact same code from a high-level C++ source.
        // The following code is a best-effort implementation that tries to mimic the original behavior.

        void* video_buffer = DAT_00437f66;
        unsigned int buffer_size = DAT_00437f62;
        unsigned int fill_color = DAT_00437491;

        memset(video_buffer, fill_color, buffer_size);
    }
    return 0;
}

/*
Function: SetGraphicsMode
Address: 0x422A01

PUSH EBP
MOV EBP,ESP
PUSH EBX
LEA EBX,[0x437520]
MOV EAX,dword ptr [EBP + 0x8]
MOV [0x00437495],AL
XLAT EBX
MOV [0x00437490],AL
MOV AH,AL
MOVZX EAX,AX
MOV EDX,EAX
SHL EDX,0x10
OR EAX,EDX
MOV [0x00437491],EAX
XOR EAX,EAX
POP EBX
POP EBP
RET
*/
void Game_SetGraphicsMode(unsigned char param_1)
{
    DAT_00437495 = param_1;
    unsigned char value = DAT_00437520[param_1];
    DAT_00437490 = value;
    unsigned short ax = (value << 8) | value;
    DAT_00437491 = (unsigned int)ax | ((unsigned int)ax << 16);
}
