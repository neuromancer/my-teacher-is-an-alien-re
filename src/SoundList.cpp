#include "memory.h"

extern "C" {
    void ShowError(const char*);
}

class SoundList {
public:
    SoundList(int capacity);

private:
    void** sounds;
    int capacity;
    void** another_list;
    int count;
};

/*
Function: SoundList
Address: 0x41E740

PUSH ESI
PUSH EDI
MOV ESI,ECX
XOR EDI,EDI
MOV EAX,dword ptr [ESP + 0xc]
MOV dword ptr [ESI],EDI
MOV dword ptr [ESI + 0x4],EDI
MOV dword ptr [ESI + 0x8],EDI
MOV dword ptr [ESI + 0xc],EDI
MOV dword ptr [ESI + 0x4],EAX
SHL EAX,0x2
PUSH EAX
CALL 0x004249c0
ADD ESP,0x4
MOV dword ptr [ESI],EAX
MOV EAX,dword ptr [ESI + 0x4]
SHL EAX,0x2
PUSH EAX
CALL 0x004249c0
ADD ESP,0x4
MOV dword ptr [ESI + 0x8],EAX
CMP EAX,EDI
JZ 0x40
CMP dword ptr [ESI],EDI
JNZ 0x4D
PUSH 0x436f38
CALL 0x00419110
XOR DX,DX
CMP dword ptr [ESI + 0x4],0x0
JLE 0x7D
MOVSX EAX,DX
LEA ECX,[EAX*0x4 + 0x0]
INC DX
MOV EDI,dword ptr [ESI]
MOV dword ptr [EDI + ECX*0x1],0x0
MOV EAX,dword ptr [ESI + 0x8]
MOV dword ptr [EAX + ECX*0x1],0x0
MOVSX ECX,DX
CMP ECX,dword ptr [ESI + 0x4]
JL 0x56
MOV EAX,ESI
POP EDI
POP ESI
RET 0x4
*/

SoundList::SoundList(int cap)
{
    sounds = 0;
    capacity = 0;
    another_list = 0;
    count = 0;

    capacity = cap;

    sounds = (void**)AllocateMemory_Wrapper(capacity * 4);
    another_list = (void**)AllocateMemory_Wrapper(capacity * 4);

    if (another_list != 0 && sounds != 0) {
        short i = 0;
        if (0 < capacity) {
            do {
                sounds[i] = 0;
                another_list[i] = 0;
                i++;
            } while (i < capacity);
        }
        return;
    }
    ShowError("SoundList::SoundList() - Memory allocation error");
}
