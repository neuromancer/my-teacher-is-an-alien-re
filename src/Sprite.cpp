#include "Smacker.h"
#include "Animation.h"

typedef char bool;
#define false 0
#define true 1

extern "C" {
    void ShowError(const char* fmt, ...);
    int GameState_Error_Handler_3(int);
    void Init(void*);
    void FUN_0041fcc0(void*, int);
    void FUN_0041fcb0(int);
    __int64 __cdecl __ftol();
    void FUN_0041be20(void*, int, int, int, int, int, int, int);
}

class Sprite {
public:
    // placeholder fields based on offsets
    char pad_0x0[0xc];
    SmackStruct* smack_struct; // 0xc
    char pad_0x10[0x24 - 0x10];
    void* smacker_buffer; // 0x24
    char pad_0x28[0x88 - 0x28];
    int* field136_0x88; // 0x88
    int flags; // 0x8c
    int field138_0x90; // 0x90
    int field139_0x94; // 0x94
    char pad_0x98[0xac-0x98];
    int field_0xac; // 0xac
    char pad_0xb0[0xb4 - 0xb0];
    const char* filename; // 0xb4
    char pad_0xb8[0xd0 - 0xb8];
    Animation* animation_data; // 0xd0
    char pad_0xd4[4];

    void CheckRanges1();
    void CloseSmackerBuffer();
    void CloseSmackerFile();
    unsigned char Do(int x, int y, int param_3, int param_4);
};

/*
Function: CheckRanges1
Address: 0x41D500

PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
CMP dword ptr [ECX + 0xd0],0x0
JNZ 0x1B
PUSH 0x436c04
CALL 0x00419110
CMP dword ptr [ESI + 0x88],0x0
JNZ 0x31
PUSH 0x436be8
CALL 0x00419110
XOR EDI,EDI
CMP dword ptr [ESI + 0xac],EDI
JLE 0x8A
XOR EBX,EBX
MOV EAX,dword ptr [ESI + 0x88]
MOV EDX,dword ptr [ESI + 0xd0]
LEA ECX,[EAX + EBX*0x1 + 0x4]
MOV EAX,dword ptr [EDX + 0xc]
MOV EAX,dword ptr [EAX + 0xc]
CMP dword ptr [ECX],EAX
JLE 0x59
MOV dword ptr [ECX],EAX
MOV EAX,dword ptr [ESI + 0x88]
ADD EAX,EBX
MOV ECX,dword ptr [EAX]
CMP dword ptr [EAX + 0x4],ECX
JGE 0x7E
LEA EAX,[ESI + 0xb4]
PUSH EAX
PUSH ECX
PUSH EDI
PUSH 0x436bc8
CALL 0x00419110
ADD EBX,0x8
INC EDI
CMP dword ptr [ESI + 0xac],EDI
JG 0x3D
POP EDI
POP ESI
POP EBX
RET
*/
void Sprite::CheckRanges1() {
    if (this->animation_data == (void *)0x0) {
        ShowError("error Sprite::CheckRanges0");
    }
    if (this->field136_0x88 == 0) {
        ShowError("error Sprite::CheckRanges1");
    }
    int i = 0;
    if (0 < this->field_0xac) {
        int j = 0;
        do {
            int* piVar2 = (int *)((char*)this->field136_0x88 + 4 + j);
            int iVar1 = *(int *)(*(int *)((int)this->animation_data + 0xc) + 0xc);
            if (iVar1 < *piVar2) {
                *piVar2 = iVar1;
            }
            piVar2 = (int *)((char*)this->field136_0x88 + j);
            iVar1 = *piVar2;
            if (piVar2[1] < iVar1) {
                ShowError("bad range: %d (start %d) in %s", i, iVar1, &this->filename);
            }
            j += 8;
            i++;
        } while (i < this->field_0xac);
    }
}

/*
Function: CloseSmackerBuffer
Address: 0x41FC00

PUSH ESI
MOV EAX,dword ptr [ECX + 0x24]
MOV ESI,ECX
TEST EAX,EAX
JZ 0x18
PUSH EAX
CALL dword ptr [0x0044050c]
MOV dword ptr [ESI + 0x24],0x0
POP ESI
RET
*/
void Sprite::CloseSmackerBuffer()
{
  if (this->smacker_buffer != 0) {
    SmackBufferClose((int)this->smacker_buffer);
    this->smacker_buffer = 0;
  }
}

/*
Function: CloseSmackerFile
Address: 0x41FBE0

PUSH ESI
MOV EAX,dword ptr [ECX + 0xc]
MOV ESI,ECX
TEST EAX,EAX
JZ 0x18
PUSH EAX
CALL dword ptr [0x00440508]
MOV dword ptr [ESI + 0xc],0x0
POP ESI
RET
*/
void Sprite::CloseSmackerFile()
{
  if (this->smack_struct != 0) {
    SmackClose(this->smack_struct);
    this->smack_struct = 0;
  }
}

/*
Function: Do
Address: 0x41D300

SUB ESP,0x4
PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
CMP dword ptr [ECX + 0x90],-0x1
JNZ 0x1F
MOV EAX,0x1
POP EDI
POP ESI
POP EBX
ADD ESP,0x4
RET 0x10
XOR EBX,EBX
XOR EDI,EDI
MOV ECX,ESI
CALL 0x0041d590
TEST EAX,EAX
JNZ 0x3C
MOV EAX,0x1
POP EDI
POP ESI
POP EBX
ADD ESP,0x4
RET 0x10
TEST byte ptr [ESI + 0x8c],0x80
JZ 0x53
MOV EAX,0x1
POP EDI
POP ESI
POP EBX
ADD ESP,0x4
RET 0x10
MOV EAX,dword ptr [ESI + 0xd0]
TEST EAX,EAX
JZ 0x63
CMP dword ptr [EAX + 0x18],0x0
JNZ 0x6A
MOV ECX,ESI
CALL 0x0041cf10
MOV EAX,dword ptr [ESI + 0x90]
SHL EAX,0x3
ADD EAX,dword ptr [ESI + 0x88]
MOV ECX,dword ptr [EAX]
CMP dword ptr [EAX + 0x4],ECX
JNZ 0x9F
MOV EAX,dword ptr [ESI + 0x8c]
MOV EBX,0x1
TEST AL,0x4
JZ 0x9A
XOR EBX,EBX
AND EAX,0xfffffffb
MOV dword ptr [ESI + 0x8c],EAX
MOV EDI,0x1
TEST EBX,EBX
JNZ 0x119
MOV ECX,dword ptr [ESI + 0xd0]
CALL 0x0041fca0
MOV ECX,dword ptr [ESI + 0xd0]
MOV EAX,dword ptr [ESI + 0x90]
MOV EDX,dword ptr [ESI + 0x88]
TEST ECX,ECX
JNZ 0xCD
CMP dword ptr [EDX + EAX*0x8 + 0x4],0x1
JNZ 0x114
JMP 0xDF
MOV EAX,dword ptr [EDX + EAX*0x8 + 0x4]
MOV EBX,dword ptr [ECX + 0xc]
SUB EAX,dword ptr [EBX + 0x374]
CMP EAX,0x1
JNZ 0x114
TEST byte ptr [ESI + 0x8d],0x2
JZ 0xEF
CALL 0x0041fcb0
JMP 0x104
MOV EDX,dword ptr [ESI + 0x90]
MOV EAX,dword ptr [ESI + 0x88]
MOV EBX,dword ptr [EAX + EDX*0x8]
PUSH EBX
CALL 0x0041fcc0
TEST byte ptr [ESI + 0x8c],0x1
JNZ 0x119
MOV EDI,0x1
JMP 0x119
CALL 0x0041fcb0
MOV EBX,dword ptr [ESI + 0x8c]
TEST BH,0x1
JZ 0x12F
MOV EAX,EDI
POP EDI
POP ESI
POP EBX
ADD ESP,0x4
RET 0x10
TEST BL,0x2
JNZ 0x190
TEST BL,0x8
MOV EAX,dword ptr [ESI + 0xd0]
JZ 0x176
TEST EAX,EAX
JZ 0x163
MOV EAX,dword ptr [EAX + 0x18]
MOV EAX,dword ptr [EAX + 0x18]
DEC EAX
MOV dword ptr [ESP + 0xc],EAX
FILD dword ptr [ESP + 0xc]
FMUL double ptr [ESP + 0x1c]
CALL 0x00425dec
MOV ECX,dword ptr [ESP + 0x18]
ADD ECX,EAX
JMP 0x194
FLD double ptr [ESP + 0x1c]
FCHS
CALL 0x00425dec
MOV ECX,dword ptr [ESP + 0x18]
ADD ECX,EAX
JMP 0x194
TEST EAX,EAX
JZ 0x189
MOV EAX,dword ptr [EAX + 0x18]
MOV ECX,dword ptr [ESP + 0x18]
MOV EAX,dword ptr [EAX + 0x18]
DEC EAX
ADD ECX,EAX
JMP 0x194
MOV ECX,dword ptr [ESP + 0x18]
DEC ECX
JMP 0x194
MOV ECX,dword ptr [ESP + 0x18]
XOR EDX,EDX
TEST BL,0x8
JZ 0x1A8
AND EBX,0x40
CMP EBX,0x1
SBB EDX,EDX
ADD EDX,0x3
JMP 0x1B2
TEST BL,0x40
JZ 0x1B2
MOV EDX,0x1
MOV EAX,dword ptr [ESP + 0x20]
MOV EBX,dword ptr [ESP + 0x1c]
PUSH EAX
PUSH EBX
PUSH EDX
MOV EAX,dword ptr [ESI + 0x94]
PUSH ECX
MOV EDX,dword ptr [ESI + 0xd0]
MOV ECX,dword ptr [ESP + 0x24]
PUSH ECX
PUSH EAX
MOV ECX,dword ptr [EDX + 0x18]
PUSH ECX
MOV ECX,dword ptr [0x0043698c]
CALL 0x0041be20
MOV EAX,dword ptr [ESI + 0x8c]
AND EAX,0x1
CMP EAX,0x1
SBB EAX,EAX
AND EAX,EDI
POP EDI
POP ESI
POP EBX
ADD ESP,0x4
RET 0x10
*/
unsigned char Sprite::Do(int x, int y, int param_3, int param_4)
{
    if (this->field138_0x90 == -1) {
        return 1;
    }
    bool bVar6 = false;
    unsigned char bVar5 = 0;
    if (GameState_Error_Handler_3((int)this) == 0) {
        return 1;
    }
    if ((this->flags & 0x80) != 0) {
        return 1;
    }
    if ((this->animation_data == (void *)0x0) || (*(int *)((int)this->animation_data + 0x18) == 0)) {
        Init(this);
    }
    int* piVar4 = (int *)(this->field138_0x90 * 8 + (int)this->field136_0x88);
    if (piVar4[1] == *piVar4) {
        bVar6 = (this->flags & 4) == 0;
        if (!bVar6) {
            this->flags = this->flags & 0xfffffffb;
        }
        bVar5 = 1;
    }
    if (!bVar6) {
        this->animation_data->DoFrame();
        void* pvVar1 = this->animation_data;
        int iVar3;
        if (pvVar1 == (void *)0x0) {
            iVar3 = *(int *)((char*)this->field136_0x88 + 4 + this->field138_0x90 * 8);
        }
        else {
            iVar3 = *(int *)((char*)this->field136_0x88 + 4 + this->field138_0x90 * 8) - *(int *)(*(int *)((int)pvVar1 + 0xc) + 0x374);
        }
        if (iVar3 == 1) {
            if ((this->flags & 0x200) == 0) {
                FUN_0041fcc0(pvVar1,*(int *)((char*)this->field136_0x88 + this->field138_0x90 * 8));
            }
            else {
                FUN_0041fcb0((int)pvVar1);
            }
            if ((this->flags & 1) == 0) {
                bVar5 = 1;
            }
        }
        else {
            FUN_0041fcb0((int)pvVar1);
        }
    }
    unsigned int uVar2 = this->flags;
    if ((uVar2 & 0x100) != 0) {
        return bVar5;
    }
    if ((uVar2 & 2) == 0) {
        void* pvVar1 = this->animation_data;
        if ((uVar2 & 8) == 0) {
            if (pvVar1 == (void *)0x0) {
                y = y + -1;
            }
            else {
                y = y + *(int *)(*(int *)((int)pvVar1 + 0x18) + 0x18) + -1;
            }
        }
        else if (pvVar1 == (void *)0x0) {
            __int64 lVar7 = __ftol();
            y = y + (int)lVar7;
        }
        else {
            __int64 lVar7 = __ftol();
            y = y + (int)lVar7;
        }
    }
    int iVar3 = 0;
    if ((uVar2 & 8) == 0) {
        if ((uVar2 & 0x40) != 0) {
            iVar3 = 1;
        }
    }
    else {
        iVar3 = 3 - (unsigned int)((uVar2 & 0x40) == 0);
    }
    FUN_0041be20(0, *(int *)((int)this->animation_data + 0x18), this->field139_0x94, x, y, iVar3, param_3, param_4);
    if ((this->flags & 1) == 0) {
        return bVar5;
    }
    return 0;
}
