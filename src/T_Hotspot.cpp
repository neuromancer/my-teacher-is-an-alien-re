#include "T_Hotspot.h"
#include "memory.h"
#include "string.h"

extern "C" {
    int FUN_0041f800(int);
    void SC_Message_Send(int, int, int, int, int, int, int, void *, int, int);
}

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

/*
Function: T_Hotspot_Update
Address: 0x4094F0

PUSH ESI
MOV EAX,dword ptr [ECX + 0x11c]
MOV ESI,ECX
DEC EAX
CMP EAX,0x3
JA 0x16
JMP dword ptr [EAX*0x4 + 0x409608]
PUSH 0x4358c0
CALL 0x00419110
MOV ECX,dword ptr [ESI + 0x8c]
TEST ECX,ECX
JZ 0x3F
CALL 0x0041f800
TEST EAX,EAX
JZ 0x49
MOV dword ptr [ESI + 0x11c],0x2
CMP dword ptr [ESI + 0x11c],0x2
JZ 0x9A
XOR EAX,EAX
POP ESI
RET 0xc
MOV ECX,dword ptr [ESI + 0x94]
TEST ECX,ECX
JZ 0x7E
CALL 0x0041f800
TEST EAX,EAX
JZ 0x88
MOV dword ptr [ESI + 0x11c],0x1
MOV EAX,0x1
POP ESI
RET 0xc
MOV dword ptr [ESI + 0x11c],0x1
MOV EAX,0x1
POP ESI
RET 0xc
MOV EAX,0x1
POP ESI
RET 0xc
MOV ECX,dword ptr [ESI + 0x90]
TEST ECX,ECX
JZ 0xBD
CALL 0x0041f800
TEST EAX,EAX
JZ 0x10F
MOV dword ptr [ESI + 0x11c],0x3
XOR EAX,EAX
POP ESI
RET 0xc
MOV dword ptr [ESI + 0x11c],0x3
CMP dword ptr [ESI + 0x128],0x0
JZ 0x10F
PUSH 0x8
CALL 0x004249c0
MOV ECX,dword ptr [ESP + 0xc]
MOV EDX,dword ptr [ESP + 0x10]
ADD ESP,0x4
MOV dword ptr [EAX],ECX
MOV dword ptr [EAX + 0x4],EDX
PUSH 0x0
MOV ECX,dword ptr [ESP + 0x14]
PUSH 0x0
PUSH EAX
PUSH 0x0
MOV EAX,dword ptr [ESI + 0x120]
MOV EDX,dword ptr [ESI + 0x124]
PUSH EAX
PUSH 0x5
PUSH ECX
PUSH 0xb
PUSH EDX
PUSH 0x9
CALL 0x0041a150
ADD ESP,0x28
XOR EAX,EAX
POP ESI
RET 0xc
*/

int T_Hotspot::Update(int param_1, int param_2, int param_3) {
    switch (field_0x11c) {
        case 1:
            if (field_0x8c == 0) {
                field_0x11c = 2;
            } else {
                if (FUN_0041f800(field_0x8c) != 0) {
                    field_0x11c = 2;
                }
            }

            if (field_0x11c != 2) {
                return 0;
            }

        case 2:
            if (field_0x90 != 0) {
                if (FUN_0041f800(field_0x90) == 0) {
                    return 0;
                }
                field_0x11c = 3;
                return 0;
            }

            field_0x11c = 3;
            if (field_0x128 != 0) {
                int *mem = (int *)AllocateMemory_Wrapper(8);
                mem[0] = param_1;
                mem[1] = param_2;
                SC_Message_Send(9, field_0x124, 0xb, param_3, 5, field_0x120, 0, mem, 0, 0);
            }
            return 0;

        case 3:
            if (field_0x94 == 0) {
                field_0x11c = 1;
            } else if (FUN_0041f800(field_0x94) != 0) {
                field_0x11c = 1;
            }
            return 1;

        case 4:
            return 1;
    }

    ShowError("Error in Thotspot.cpp - Update\n");
    return 0;
}
