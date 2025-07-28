#include "Smacker.h"

/*
Function: FUN_0041fcb0
Address: 0x41FCB0

MOV EAX,dword ptr [ECX + 0xc]
TEST EAX,EAX
JZ 0xE
PUSH EAX
CALL dword ptr [0x0044051c]
RET
*/
void Smacker::NextFrame()
{
    if (this->smk != 0) {
        SmackNextFrame(this->smk);
    }
}

extern "C" char DAT_00436970[];

/*
Function: FUN_0041fcc0
Address: 0x41FCC0

PUSH ESI
PUSH EDI
MOV EAX,dword ptr [ECX + 0xc]
MOV ESI,ECX
TEST EAX,EAX
JZ 0x4D
MOV ECX,dword ptr [0x00436970]
CMP byte ptr [ECX + 0x46],0x2
JNZ 0x24
PUSH 0x0
MOV EDI,dword ptr [0x00440538]
PUSH EAX
CALL EDI
JMP 0x2A
MOV EDI,dword ptr [0x00440538]
MOV EAX,dword ptr [ESP + 0xc]
MOV ECX,dword ptr [ESI + 0xc]
PUSH EAX
PUSH ECX
CALL dword ptr [0x00440520]
MOV ECX,dword ptr [0x00436970]
CMP byte ptr [ECX + 0x46],0x2
JNZ 0x4D
PUSH 0x1
MOV EAX,dword ptr [ESI + 0xc]
PUSH EAX
CALL EDI
POP EDI
POP ESI
RET 0x4
*/
void Smacker::Goto(int frame)
{
    if (this->smk != 0) {
        if (DAT_00436970[0x46] == 2) {
            SmackSoundOnOff(this->smk, 0);
        }
        SmackGoto(this->smk, frame);
        if (DAT_00436970[0x46] == 2) {
            SmackSoundOnOff(this->smk, 1);
        }
    }
}
