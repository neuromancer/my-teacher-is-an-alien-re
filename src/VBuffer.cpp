#include "VBuffer.h"

#include <string.h>
#include "string.h"

extern "C" unsigned int FUN_00423703(int* p1, unsigned int p2, unsigned int p3);
extern "C" void FUN_004234f9(void* p1, void* p2, unsigned int p3, unsigned int p4, unsigned int p5, unsigned int p6);
extern "C" int FUN_0041b590(void* p1, void* p2, void* p3, void* p4);
extern "C" void FUN_004233e8(int p1, int p2, int p3, int p4, int p5, int p6, unsigned int p7, void* p8);
extern "C" void FUN_004234d5(unsigned int p1);
extern "C" void FUN_0041b29a();
extern "C" void FUN_0041b2ac();
extern "C" int DAT_00436964[];
extern "C" void FUN_0041a9a0();
extern "C" void FUN_00422e1a(void* p);
extern "C" void FUN_0041a9e0(void* p);
extern "C" void SetGraphicsMode(unsigned char param_1);
extern "C" int GlobalClearScreen();
extern "C" int InvalidateVideoMode();
extern "C" int _SetVideoMode(int mode);
extern "C" int GetCurrentVideoMode();
extern "C" void* FUN_00422f00(int width, int height);
extern "C" void FUN_0041ac50(void* pThis);
extern "C" void FUN_00422e8f();
extern "C" void FUN_0041ac80(void* pThis);
extern "C" void* FUN_00422e71(void* p);
extern "C" void FUN_0041a9d0(void* p);
extern "C" void FUN_0041a9a0();

// Global variables
extern "C" int DAT_0043826c[];
extern "C" int DAT_0043841c;
extern "C" int DAT_00437fec[];
extern "C" char DAT_00437f54;
extern "C" int DAT_0043836c[];
extern "C" int DAT_00437f5e;
extern "C" int DAT_004374c6;
extern "C" int DAT_004382ec[];
extern "C" int DAT_004374d2;
extern "C" int DAT_004374ca;
extern "C" int DAT_0043816c[];
extern "C" int DAT_004374de;
extern "C" int DAT_0043806c[];
extern "C" int DAT_004374e2;
extern "C" int DAT_004381ec[];
extern "C" int DAT_004374e6;
extern "C" int DAT_004380ec[];
extern "C" int DAT_004374ea;
extern "C" short DAT_00437f6a;
extern "C" int DAT_00437f66;
extern "C" int DAT_00437f62;
extern "C" void FUN_0043020a(void* p1, int p2, int p3, int p4);


VBuffer::~VBuffer()
{
    Release();
    Free();
}

/*
Function: VBuffer
Address: 0x41AAA0

PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
MOV EDI,dword ptr [ESP + 0x10]
MOV EBX,dword ptr [ESP + 0x14]
TEST EDI,EDI
JZ 0x15
TEST EBX,EBX
JNZ 0x28
MOV EAX,EBX
IMUL EAX,EDI
PUSH EAX
PUSH 0x436b20
CALL 0x00419110
XOR ECX,ECX
LEA EAX,[EDI + -0x1]
MOV dword ptr [ESI + 0x14],EDI
MOV dword ptr [ESI + 0x18],EBX
LEA EDX,[EBX + -0x1]
MOV dword ptr [ESI + 0x28],ECX
MOV dword ptr [ESI + 0x2c],EAX
MOV dword ptr [ESI + 0x20],ECX
MOV dword ptr [ESI + 0x24],EDX
CMP dword ptr [ESI + 0x10],ECX
JZ 0x54
PUSH 0x436af8
CALL 0x00419110
MOV EAX,EDI
CDQ
XOR EAX,EDX
SUB EAX,EDX
AND EAX,0x3
XOR EAX,EDX
SUB EAX,EDX
JZ 0x75
INC EDI
MOV EAX,EDI
CDQ
XOR EAX,EDX
SUB EAX,EDX
AND EAX,0x3
XOR EAX,EDX
SUB EAX,EDX
JNZ 0x64
MOV EAX,EBX
CDQ
XOR EAX,EDX
SUB EAX,EDX
AND EAX,0x3
XOR EAX,EDX
SUB EAX,EDX
JZ 0x96
INC EBX
MOV EAX,EBX
CDQ
XOR EAX,EDX
SUB EAX,EDX
AND EAX,0x3
XOR EAX,EDX
SUB EAX,EDX
JNZ 0x85
PUSH EBX
PUSH EDI
CALL 0x00422f00
ADD ESP,0x8
MOV dword ptr [ESI + 0x1c],EAX
CMP EAX,-0x1
JNZ 0xBE
PUSH 0x436ac8
CALL 0x00419110
CMP EAX,-0x2
JNZ 0xD9
PUSH 0x436a98
CALL 0x00419110
PUSH EAX
MOV ECX,ESI
CALL 0x0041ac50
CALL 0x00422e8f
MOV ECX,ESI
CALL 0x0041ac80
MOV EAX,dword ptr [ESI + 0x1c]
PUSH EAX
CALL 0x00422e71
ADD ESP,0x4
MOV dword ptr [ESI + 0x10],EAX
MOV EAX,dword ptr [ESI + 0x1c]
PUSH EAX
CALL 0x0041a9d0
ADD ESP,0x4
MOV EAX,dword ptr [ESI + 0x10]
POP EDI
POP ESI
POP EBX
RET 0x8
*/
VBuffer::VBuffer(int width, int height)
{
    if (width == 0 || height == 0) {
        ShowError("s_Error__Invalid_buffer_size_speci_00436b20", width * height);
    }

    this->width = width;
    this->height = height;
    this->field_0x28 = 0;
    this->field_0x2c = width - 1;
    this->field_0x20 = 0;
    this->field_0x24 = height - 1;

    if (this->data != 0) {
        ShowError("s_Error__Virtual_buffer_already_al_00436af8");
    }

    while (width % 4 != 0) {
        width++;
    }
    while (height % 4 != 0) {
        height++;
    }

    this->field_0x1c = FUN_00422f00(width, height);

    if (this->field_0x1c == (void*)-1) {
        ShowError("s_VBuffer__Init___Unable_To_create_00436ac8");
    }
    if (this->field_0x1c == (void*)-2) {
        ShowError("s_VBuffer__Init___Unable_To_create_00436a98");
    }

    SetCurrentVideoMode((int)this->field_0x1c);
    FUN_00422e8f();
    InvalidateVideoMode();
    this->data = FUN_00422e71(this->field_0x1c);
    FUN_0041a9d0(this->field_0x1c);
}

VBuffer* VBuffer::VirtualBufferCreateAndClean(int width, int height)
{
    if (width == 0 || height == 0) {
        ShowError("s_VBuffer__VBuffer___bad_dimensi_00436b20", width * height);
    }

    this->width = width;
    this->height = height;
    this->field_0x28 = 0;
    this->field_0x2c = width - 1;
    this->field_0x20 = 0;
    this->field_0x24 = height - 1;

    if (this->data != 0) {
        ShowError("s_VBuffer__VBuffer___vbuffer_alre_00436af8");
    }

    if ((width % 4) != 0) {
        width++;
        if ((width % 4) != 0) {
            // This seems to be a manual alignment to 4 bytes.
        }
    }
    if ((height % 4) != 0) {
        height++;
        if ((height % 4) != 0) {
            // This seems to be a manual alignment to 4 bytes.
        }
    }

    this->field_0x1c = FUN_00422f00(width, height);

    if (this->field_0x1c == (void*)-1) {
        ShowError("s_VBuffer__VBuffer___unable_to_cr_00436ac8");
    }
    if (this->field_0x1c == (void*)-2) {
        ShowError("s_VBuffer__VBuffer___unable_to_lo_00436a98");
    }

    FUN_0041ac50(this);
    FUN_00422e8f();
    FUN_0041ac80(this);
    this->data = FUN_00422e71(this->field_0x1c);
    FUN_0041a9d0(this->field_0x1c);

    this->Initialize();
    this->Clear();
    return this;
}

int VBuffer::GetSomething()
{
    return *(int*)((char*)this->data);
}

/*
Function: SetVideoMode
Address: 0x41ABF0

PUSH ESI
MOV EAX,dword ptr [ECX + 0x1c]
MOV ESI,ECX
PUSH EAX
CALL 0x004230d9
ADD ESP,0x4
MOV dword ptr [ESI],0xffffffff
MOV dword ptr [ESI + 0x4],0x1
POP ESI
RET
*/
int VBuffer::SetVideoMode(void* param_1, void* param_2)
{
    int iVar1 = _SetVideoMode((int)field_0x1c);
    videoMode = 0xffffffff;
    field1_0x4 = 1;
    return iVar1;
}

/*
Function: InvalidateVideoMode
Address: 0x41AC80

PUSH ESI
MOV EAX,dword ptr [ECX + 0x4]
MOV ESI,ECX
TEST EAX,EAX
JZ 0x2B
DEC EAX
MOV dword ptr [ESI + 0x4],EAX
JNZ 0x2B
CALL 0x004231bc
MOV EAX,dword ptr [ESI]
CMP EAX,-0x1
JZ 0x2B
PUSH EAX
CALL 0x004230d9
ADD ESP,0x4
MOV dword ptr [ESI],0xffffffff
POP ESI
RET
*/
void VBuffer::InvalidateVideoMode()
{
    if ((this->field1_0x4 != 0) && (this->field1_0x4--, this->field1_0x4 == 0))
    {
        ::InvalidateVideoMode();
        if (this->videoMode != 0xffffffff)
        {
            _SetVideoMode(this->videoMode);
            this->videoMode = 0xffffffff;
        }
    }
}

/*
Function: ClearScreen
Address: 0x41ABC0

PUSH ESI
MOV EAX,dword ptr [ECX + 0x1c]
MOV ESI,ECX
PUSH EAX
CALL 0x0041ac50
XOR EAX,EAX
MOV AL,byte ptr [ESP + 0x8]
PUSH EAX
CALL 0x00422a01
ADD ESP,0x4
CALL 0x00423099
MOV ECX,ESI
CALL 0x0041ac80
POP ESI
RET 0x4

*/
void VBuffer::ClearScreen(unsigned char param_1)
{
    SetCurrentVideoMode((int)this->field_0x1c);
    SetGraphicsMode(param_1);
    ::GlobalClearScreen();
    InvalidateVideoMode();
}

/*
Function: Free
Address: 0x41AA60

PUSH ESI
MOV ESI,ECX
CMP dword ptr [ECX + 0x10],0x0
JZ 0x2F
MOV EAX,dword ptr [ESI + 0x1c]
PUSH EAX
CALL 0x00422e1a
ADD ESP,0x4
MOV EAX,dword ptr [ESI + 0x1c]
PUSH EAX
CALL 0x0041a9e0
MOV dword ptr [ESI + 0x1c],0xffffffff
MOV dword ptr [ESI + 0x10],0x0
ADD ESP,0x4
POP ESI
RET
*/
void VBuffer::Free()
{
    if (this->data != 0)
    {
        FUN_00422e1a(this->field_0x1c);
        FUN_0041a9e0(this->field_0x1c);
        this->field_0x1c = (void*)0xffffffff;
        this->data = 0;
    }
}

/*
Function: Release
Address: 0x41AC10

PUSH ESI
MOV ESI,ECX
CALL 0x004231c6
CMP EAX,dword ptr [ESI + 0x1c]
JNZ 0x12
CALL 0x004231bc
POP ESI
RET

*/
void VBuffer::Release()
{
    unsigned int uVar1;

    uVar1 = GetCurrentVideoMode();
    if (uVar1 == (unsigned int)this->field_0x1c)
    {
        ::InvalidateVideoMode();
    }
}

/*
Function: Initialize
Address: 0x41AA30

PUSH EDI
MOV EDX,ECX
MOV EDI,EDX
XOR EAX,EAX
MOV ECX,0xc
STOSD.REP ES:EDI
MOV dword ptr [EDX + 0x1c],0xffffffff
MOV dword ptr [EDX],0xffffffff
CALL 0x0041a9a0
POP EDI
RET
*/
void VBuffer::Initialize() {
    memset(this, 0, sizeof(VBuffer));
    field_0x1c = (void*)0xffffffff;
    videoMode = 0xffffffff;
    FUN_0041a9a0();
}

/*
Function: ScaleTCCopy
Address: 0x41B110

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41b2a2
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x24
PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
MOV ECX,dword ptr [EBP + 0x10]
FILD dword ptr [ECX + 0x14]
FMUL double ptr [EBP + 0x14]
CALL 0x00425dec
MOV EDI,EAX
MOV ECX,dword ptr [EBP + 0x10]
FILD dword ptr [ECX + 0x18]
FMUL double ptr [EBP + 0x14]
CALL 0x00425dec
MOV EBX,EAX
CMP EDI,0x1
JL 0x142
CMP EBX,0x1
JL 0x142
PUSH EBX
MOV EAX,[0x00436964]
PUSH EDI
PUSH EAX
CALL 0x00423703
ADD ESP,0xc
MOV dword ptr [EBP + -0x10],EAX
TEST EAX,EAX
JGE 0x76
PUSH 0x436b70
CALL 0x00419110
MOV EAX,dword ptr [EBP + -0x10]
PUSH EAX
CALL 0x00422e71
ADD ESP,0x4
MOV ECX,dword ptr [EBP + 0x10]
PUSH EBX
MOV EDX,dword ptr [ECX + 0x18]
PUSH EDI
PUSH EDX
MOV EDX,dword ptr [ECX + 0x14]
PUSH EDX
PUSH EAX
MOV EAX,dword ptr [ECX + 0x10]
PUSH EAX
CALL 0x004234f9
ADD ESP,0x18
LEA EAX,[EBP + -0x30]
MOV dword ptr [EBP + -0x4],0x0
MOV dword ptr [EAX],0x0
MOV dword ptr [EAX + 0x4],0x0
MOV dword ptr [EAX + 0x8],0x0
MOV dword ptr [EAX + 0xc],0x0
MOV byte ptr [EBP + -0x4],0x1
LEA EAX,[EBP + -0x20]
MOV ECX,dword ptr [ESI + 0x20]
MOV dword ptr [EBP + -0x2c],ECX
MOV EDX,dword ptr [ESI + 0x2c]
MOV dword ptr [EAX],0x0
MOV dword ptr [EBP + -0x28],EDX
MOV dword ptr [EAX + 0x4],0x0
MOV dword ptr [EAX + 0x8],0x0
MOV dword ptr [EAX + 0xc],0x0
MOV EAX,dword ptr [ESI + 0x28]
DEC EDI
DEC EBX
MOV dword ptr [EBP + -0x30],EAX
MOV EAX,dword ptr [ESI + 0x24]
MOV dword ptr [EBP + -0x18],EDI
LEA ECX,[EBP + 0x8]
MOV dword ptr [EBP + -0x24],EAX
LEA EAX,[EBP + 0xc]
LEA EDX,[EBP + -0x20]
PUSH EAX
MOV dword ptr [EBP + -0x14],EBX
PUSH ECX
LEA EAX,[EBP + -0x30]
MOV dword ptr [EBP + -0x20],0x0
MOV dword ptr [EBP + -0x1c],0x0
PUSH EDX
PUSH EAX
CALL 0x0041b590
ADD ESP,0x10
TEST EAX,EAX
JNZ 0x154
MOV byte ptr [EBP + -0x4],0x0
CALL 0x0041b29a
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x0041b2ac
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
MOV FS:[0x0],EAX
POP ESI
POP EBX
MOV ESP,EBP
POP EBP
RET 0x14
MOV EAX,dword ptr [ESI + 0x1c]
MOV ECX,dword ptr [EBP + -0x10]
PUSH EAX
MOV EDX,dword ptr [EBP + 0xc]
PUSH ECX
MOV EAX,dword ptr [EBP + 0x8]
PUSH EDX
MOV ECX,dword ptr [EBP + -0x14]
PUSH EAX
MOV EDX,dword ptr [EBP + -0x1c]
PUSH ECX
MOV EAX,dword ptr [EBP + -0x18]
PUSH EDX
MOV ECX,dword ptr [EBP + -0x20]
PUSH EAX
PUSH ECX
CALL 0x004233e8
ADD ESP,0x20
MOV ECX,dword ptr [EBP + -0x10]
PUSH ECX
CALL 0x004234d5
ADD ESP,0x4
JMP 0x12D

*/
void VBuffer::ScaleTCCopy(int param_1, int param_2, int param_3)
{
    unsigned int uVar1;
    unsigned int uVar2;
    void* puVar3;
    int iVar4;
    int local_34;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    unsigned int local_14;

    try {
        uVar1 = *(unsigned int*)(param_3 + 0x14);
        uVar2 = *(unsigned int*)(param_3 + 0x18);

        if ((0 < (int)uVar1) && (0 < (int)uVar2))
        {
            local_14 = FUN_00423703(DAT_00436964, uVar1, uVar2);
            if ((int)local_14 < 0)
            {
                ShowError("VBuffer::ScaleTCCopy");
            }
            puVar3 = FUN_00422e71((void*)local_14);
            FUN_004234f9(*(void**)(param_3 + 0x10), puVar3, *(unsigned int*)(param_3 + 0x14), *(unsigned int*)(param_3 + 0x18), uVar1, uVar2);

            local_34 = this->field_0x20;
            local_1c = uVar1 - 1;
            local_18 = uVar2 - 1;
            local_24 = 0;
            local_20 = 0;

            iVar4 = FUN_0041b590(&local_34, &local_24, &param_1, &param_2);
            if (iVar4 != 0)
            {
                FUN_004233e8(local_24, local_1c, local_20, local_18, param_1, param_2, local_14, this->field_0x1c);
                FUN_004234d5(local_14);
            }
        }
    } catch (...) {
        FUN_0041b29a();
        FUN_0041b2ac();
    }
}

/*
Function: VBuffer::SetCurrentVideoMode
Address: 0x41AC50

PUSH ESI
INC dword ptr [ECX + 0x4]
MOV ESI,ECX
CALL 0x004231c6
MOV ECX,dword ptr [ESP + 0x8]
CMP ECX,EAX
JZ 0x1E
PUSH ECX
MOV dword ptr [ESI],EAX
CALL 0x004230d9
ADD ESP,0x4
POP ESI
RET 0x4
*/
void VBuffer::SetCurrentVideoMode(int mode) {
    this->field1_0x4++;
    int current_mode = GetCurrentVideoMode();
    if (mode != current_mode) {
        this->videoMode = current_mode;
        _SetVideoMode(mode);
    }
}

/*
Function: _SetVideoMode
Address: 0x4230D9

PUSH EBP
MOV EBP,ESP
PUSH EDI
PUSH EBX
MOV EBX,dword ptr [EBP + 0x8]
CMP EBX,0x1f
JA 0x29
MOV EDX,EBX
LEA EDI,[0x43826c]
SHL EBX,0x2
MOV EAX,dword ptr [EBX + EDI*0x1]
OR EAX,EAX
JNZ 0x33
MOV EAX,0xfffffffe
JMP 0xDD
MOV EAX,0xffffffff
JMP 0xDD
MOV ECX,dword ptr [0x0043841c]
JECXZ 0x4F
PUSH EBX
PUSH EDX
LEA EDI,[0x437fec]
MOV EAX,dword ptr [EBX + EDI*0x1]
PUSH EAX
PUSH ECX
CALL 0x0043020a
POP EDX
POP EBX
MOV byte ptr [0x00437f54],DL
LEA EDI,[0x43836c]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x00437f5e],EAX
MOV [0x004374c6],EAX
LEA EDI,[0x4382ec]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x004374d2],EAX
DEC EAX
MOV [0x004374ca],EAX
LEA EDI,[0x43816c]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x004374de],EAX
LEA EDI,[0x43806c]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x004374e2],EAX
LEA EDI,[0x4381ec]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x004374e6],EAX
LEA EDI,[0x4380ec]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV [0x004374ea],EAX
LEA EDI,[0x43826c]
MOV EAX,dword ptr [EBX + EDI*0x1]
MOV word ptr [0x00437f6a],DS
MOV [0x00437f66],EAX
MOV EAX,[0x004374c6]
MOV EBX,dword ptr [0x004374d2]
MUL EBX
MOV [0x00437f62],EAX
XOR EAX,EAX
POP EBX
POP EDI
MOV ESP,EBP
POP EBP
RET
*/

int _SetVideoMode(int mode) {
    if (mode > 0x1f) {
        return -1;
    }
    if (DAT_0043826c[mode] == 0) {
        return -2;
    }
    if (DAT_0043841c != 0) {
        FUN_0043020a((void*)DAT_0043841c, DAT_00437fec[mode], mode, 0);
    }
    DAT_00437f54 = (char)mode;
    DAT_00437f5e = DAT_0043836c[mode];
    DAT_004374c6 = DAT_0043836c[mode];
    DAT_004374d2 = DAT_004382ec[mode];
    DAT_004374ca = DAT_004382ec[mode] - 1;
    DAT_004374de = DAT_0043816c[mode];
    DAT_004374e2 = DAT_0043806c[mode];
    DAT_004374e6 = DAT_004381ec[mode];
    DAT_004374ea = DAT_004380ec[mode];
    DAT_00437f66 = DAT_0043826c[mode];
    DAT_00437f6a = 0; // This is a guess, DS is likely 0
    DAT_00437f62 = DAT_004374c6 * DAT_004374d2;
    return 0;
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
extern "C" char DAT_00437491;

int InvalidateVideoMode() {
    DAT_00437f54 = -1;
    return 0;
}
