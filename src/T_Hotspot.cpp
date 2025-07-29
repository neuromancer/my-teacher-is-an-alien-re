#include "T_Hotspot.h"
#include "string.h"

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

/*
Function: T_Hotspot_Set_State
Address: 0x409440

MOV ECX,dword ptr [ECX + 0x88]
TEST ECX,ECX
JZ 0x1C
MOV EAX,dword ptr [ESP + 0x4]
PUSH EAX
CALL 0x0041d190
MOV EAX,0x1
RET 0x4
PUSH 0x435880
CALL 0x00419110
*/

int T_Hotspot::Set_State(int state) {
    if (sprite) {
        sprite->SetState(state);
        return 1;
    }
    ShowError("Error in T_Hotspot::Set_State\n");
    return 0; // Should not be reached
}

/*
Function: T_Hotspot_Get_State
Address: 0x409470

MOV EAX,dword ptr [ECX + 0x88]
TEST EAX,EAX
JNZ 0x1A
PUSH 0x4358a0
CALL 0x00419110
MOV EAX,dword ptr [EAX + 0x90]
RET
*/

int T_Hotspot::Get_State() {
    if (!sprite) {
        ShowError("Error in T_Hotspot::Get_State\n");
    }
    return sprite->field_0x90;
}
