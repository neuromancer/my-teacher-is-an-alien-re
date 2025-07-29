#include "T_Hotspot.h"
#include "string.h"

/*************************************************************************************************/
/*                                                                                               */
/* Function: T_Hotspot::Do                                                                       */
/* Address: 0x409400                                                                             */
/*                                                                                               */
/*************************************************************************************************/
/*
Function: T_Hotspot_Do
Address: 0x409400

MOV ECX,dword ptr [ECX + 0x88]
TEST ECX,ECX
JZ 0x25
PUSH 0x3ff00000
MOV EAX,dword ptr [ECX + 0xa8]
PUSH 0x0
MOV EDX,dword ptr [ECX + 0xa4]
PUSH EAX
PUSH EDX
CALL 0x0041d300
RET
PUSH 0x435860
CALL 0x00419110
*/

void T_Hotspot::Do() {
    if (sprite) {
        sprite->Do(sprite->field_0xa4, sprite->field_0xa8, 0, 1.0f);
        return;
    }
    ShowError("Missing sprite in Hotspot Do\n");
}
