#include "Animation.h"
#include "Smacker.h"
#include <string.h>
#include "string.h"

extern "C" void OpenAndConvertToBuffer(Animation* anim, char* filename);
extern "C" void Play(Animation* anim, unsigned int flags);

/*
Function: Play
Address: 0x41FA80

Disassembled:
MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41fad9
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
PUSH ESI
MOV ESI,ECX
MOV dword ptr [EBP + -0x4],0x0
CALL 0x0041cd50
MOV ECX,dword ptr [EBP + 0x8]
OR dword ptr [EAX + 0x8c],ECX
MOV ECX,EAX
CALL 0x00420020
MOV ECX,EAX
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x0041fb70
MOV EAX,dword ptr [EBP + -0xc]
POP ESI
MOV dword ptr FS:[0x0],ECX
MOV ESP,EBP
POP EBP
RET 0x4
*/
void Animation::Play(char* filename, unsigned int flags)
{
    Sprite::Sprite(filename);
    this->flags |= flags;
    this->MainLoop();
}

/*
Function: Open
Address: 0x41FE50

Disassembled:
MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x420014
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x20
PUSH EBX
PUSH ESI
PUSH EDI
MOV dword ptr [EBP + -0x10],ECX
MOV EAX,dword ptr [EBP + 0x8]
MOV dword ptr [EBP + -0x4],0x0
PUSH EAX
LEA EAX,[EBP + -0x14]
PUSH EAX
CALL 0x00424c40
ADD ESP,0x8
MOV EAX,dword ptr [EBP + -0x14]
TEST EAX,EAX
JZ 0x4E
PUSH EAX
CALL 0x00424940
ADD ESP,0x4
MOV ECX,dword ptr [EBP + 0x8]
PUSH ECX
CALL 0x00424a00
ADD ESP,0x4
MOV dword ptr [EBP + -0x14],EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV dword ptr [ECX + 0xb4],EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0xb4]
PUSH EAX
CALL 0x00424980
ADD ESP,0x4
TEST EAX,EAX
JNZ 0x8F
LEA EAX,[EBP + -0x14]
PUSH EAX
PUSH 0x437174
CALL 0x00419110
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0xb4]
PUSH EAX
CALL dword ptr [0x00440504]
ADD ESP,0x4
MOV ECX,dword ptr [EBP + -0x10]
MOV dword ptr [ECX + 0xc],EAX
TEST EAX,EAX
JNZ 0xC1
LEA EAX,[EBP + -0x14]
PUSH EAX
PUSH 0x4371a0
CALL 0x00419110
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0xc]
MOV EDX,dword ptr [EBP + 0xc]
PUSH EDX
PUSH EAX
CALL dword ptr [0x0044051c]
ADD ESP,0x8
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0xc]
MOV dword ptr [EBP + -0x4],0x1
MOV dword ptr [ECX + 0xd0],EAX
MOV ECX,dword ptr [EBP + -0x10]
CALL 0x0041d500
MOV ECX,dword ptr [EBP + -0x10]
CALL 0x0041fd80
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0x8]
CALL 0x0041d1c0
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
POP ESI
POP EBX
MOV dword ptr FS:[0x0],EAX
MOV ESP,EBP
POP EBP
RET 0xc
*/
void Animation::Open(char* filename, int param_2, unsigned int param_3)
{
    this->filename = strdup(filename);
    if (this->filename == 0) {
        ShowError("Error allocating memory for filename");
    }
    this->smack_struct = SmackOpen(this->filename, param_3, 0);
    if (this->smack_struct == 0) {
        ShowError("Error opening smack file");
    }
    this->animation_data = (Animation*)this->smack_struct;
    this->CheckRanges1();
    this->VBInit();
    this->ToBuffer();
}
