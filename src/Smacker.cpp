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
