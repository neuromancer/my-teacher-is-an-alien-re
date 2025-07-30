#include "Sprite.h"
#include "Smacker.h"
#include "Animation.h"
#include "VBuffer.h"

#include <string.h>

extern "C" {
    void _sscanf(const char* buffer, const char* format, ...);
    void Array_Cleanup(int, int, int, void*);
    void FreeFromGlobalHeap(int*);
    int* AllocateMemory_Wrapper(int);
    void FUN_00424b00(int*, int, int, void*, void*);
    void FUN_0041fbd3();
}

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
    void FUN_0041fc20(Sprite*, int, int);
    int FUN_00421d10(void*);
    void* DAT_00436968;
    int DAT_004373bc;
    int _SmackWait_4(SmackStruct*);
    int* FUN_004224f0();
    int* FUN_004224e0();
    void FUN_0041acf0(void*, int, int, int, int, int, int, int, int);
    void* FUN_004249c0(int size);
    int FUN_004224d0();
    int FUN_00421af0();
}

/*
Function: Sprite
Address: 0x41CD50

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41ce19
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
PUSH ESI
PUSH EDI
MOV dword ptr [EBP + -0x10],ECX
MOV ESI,ECX
CALL 0x004189f0
LEA EDX,[ESI + 0xa4]
LEA EDI,[ESI + 0x88]
XOR EAX,EAX
MOV ECX,0x14
MOV dword ptr [EBP + -0x4],0x0
MOV byte ptr [EBP + -0x4],0x1
MOV dword ptr [EDX],0x0
MOV dword ptr [EDX + 0x4],0x0
MOV dword ptr [ESI],0x431530
STOSD.REP ES:EDI
MOV ECX,dword ptr [EBP + 0x8]
OR dword ptr [ESI + 0x8c],0x20
CMP ECX,EAX
JZ 0x7D
LEA EAX,[ESI + 0xb4]
PUSH EAX
PUSH 0x435594
PUSH ECX
CALL 0x00424c40
ADD ESP,0xc
PUSH 0x1
MOV ECX,ESI
CALL 0x0041d740
PUSH 0x1388
MOV ECX,ESI
MOV dword ptr [ESI + 0x94],0x0
PUSH 0x1
PUSH 0x0
CALL 0x0041d6d0
MOV EAX,ESI
POP EDI
MOV dword ptr [EBP + -0x4],0xffffffff
MOV ECX,dword ptr [EBP + -0xc]
POP ESI
MOV dword ptr FS:[0x0],ECX
MOV ESP,EBP
POP EBP
RET 0x4
*/
Sprite::Sprite(char* filename) : BaseObject()
{
    this->vbuffer = new VBuffer(0, 0);
    this->field_0xa4 = 0;
    this->field_0xa8 = 0;
    *(void**)this = (void*)0x431530;

    int* p = (int*)&this->field136_0x88;
    for (int i = 0; i < 0x14; i++) {
        *p++ = 0;
    }

    this->flags |= 0x20;

    if (filename != (char *)0x0) {
        _sscanf(filename, "%s", &this->filename);
    }

    SetRanges(1);
    this->field139_0x94 = 0;
    this->SetRange(0, 1, 5000);
}

/*
Function: SetRanges
Address: 0x41D740

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41d831
PUSH EAX
MOV dword ptr FS:[0x0],ESP
MOV EAX,dword ptr [EBP + 0x8]
SUB ESP,0x4
PUSH EBX
PUSH ESI
MOV dword ptr [ECX + 0xac],EAX
PUSH EDI
MOV EAX,dword ptr [ECX + 0x88]
MOV ESI,ECX
TEST EAX,EAX
JZ 0x59
LEA EDI,[EAX + -0x4]
PUSH 0x405770
MOV ECX,dword ptr [EDI]
PUSH ECX
PUSH 0x8
PUSH EAX
CALL 0x004249d0
PUSH EDI
CALL 0x00424940
MOV dword ptr [ESI + 0x88],0x0
ADD ESP,0x4
MOV EDI,dword ptr [ESI + 0xac]
LEA EAX,[EDI*0x8 + 0x0]
ADD EAX,0x4
MOV EBX,0x0
PUSH EAX
CALL 0x004249c0
ADD ESP,0x4
MOV dword ptr [EBP + -0x10],EAX
MOV dword ptr [EBP + -0x4],0x0
TEST EAX,EAX
JZ 0x9D
LEA EBX,[EAX + 0x4]
PUSH 0x405770
PUSH 0x41d850
MOV dword ptr [EAX],EDI
PUSH EDI
PUSH 0x8
PUSH EBX
CALL 0x00424b00
XOR ECX,ECX
XOR EDX,EDX
MOV dword ptr [EBP + -0x4],0xffffffff
MOV dword ptr [ESI + 0x88],EBX
CMP dword ptr [ESI + 0xac],ECX
JLE 0xD8
MOV EDI,0x1388
MOV EAX,dword ptr [ESI + 0x88]
INC EDX
ADD EAX,ECX
ADD ECX,0x8
MOV dword ptr [EAX],0x1
MOV dword ptr [EAX + 0x4],EDI
CMP dword ptr [ESI + 0xac],EDX
JG 0xBB
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
OR dword ptr [ESI + 0x8c],0x20
MOV FS:[0x0],EAX
POP ESI
POP EBX
MOV ESP,EBP
POP EBP
RET 0x4
*/
/*
Function: ~Sprite
Address: 0x41FB70

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41fbc9
MOV dword ptr [ECX],0x431570
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
MOV dword ptr [EBP + -0x4],0x0
MOV dword ptr [EBP + -0x10],ECX
CALL 0x0041fe20
MOV ECX,dword ptr [EBP + -0x10]
CALL 0x0041fc00
MOV ECX,dword ptr [EBP + -0x10]
CALL 0x0041fbe0
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x0041fbd3
MOV EAX,dword ptr [EBP + -0xc]
MOV ESP,EBP
MOV FS:[0x0],EAX
POP EBP
RET
*/
Sprite::~Sprite()
{
    *(void**)this = (void*)0x431570;
    this->vbuffer->Free();
    CloseSmackerBuffer();
    CloseSmackerFile();
    FUN_0041fbd3();
}

void Sprite::SetRanges(int numRanges)
{
    this->field_0xac = numRanges;
    if (this->field136_0x88 != 0) {
        Array_Cleanup((int)this->field136_0x88, 8, *(int*)((int)this->field136_0x88 - 4), (void*)0x405770);
        FreeFromGlobalHeap((int*)((int)this->field136_0x88 - 4));
        this->field136_0x88 = 0;
    }

    int* piVar5 = (int*)0x0;
    int* piVar1 = AllocateMemory_Wrapper(numRanges * 8 + 4);
    if (piVar1 != (int *)0x0) {
        piVar5 = piVar1 + 1;
        *piVar1 = numRanges;
        FUN_00424b00(piVar5, 8, numRanges, (void*)0x41d850, (void*)0x405770);
    }

    this->field136_0x88 = piVar5;
    for (int i = 0; i < numRanges; i++) {
        int* p = (int*)((char*)this->field136_0x88 + i * 8);
        p[0] = 1;
        p[1] = 5000;
    }

    this->flags |= 0x20;
}

void Sprite::SetState(int state) {
    if (state == -1) {
        this->field138_0x90 = -1;
        return;
    }

    if (this->animation_data == 0) {
        Init(this);
    }

    if (state > -1 && state <= this->field_0xac - 1) {
        if (this->animation_data != 0) {
            int iVar1 = this->animation_data->smack_struct->current_frame;
            if (this->field136_0x88 != 0) {
                int* piVar5 = (int*)((char*)this->field136_0x88 + state * 8);
                bool bVar4 = (iVar1 >= *piVar5) && (iVar1 <= piVar5[1]);

                if (!bVar4) {
                    this->flags |= 0x20;
                }

                if ((this->flags & 0x20) != 0 || this->field138_0x90 != state) {
                    int iVar6 = 0;
                    if ((this->flags & 0x10) != 0) {
                        int iVar2 = (int)this->field136_0x88;
                        iVar6 = this->animation_data->smack_struct->current_frame - *(int*)((char*)iVar2 + this->field138_0x90 * 8);
                        int iVar3 = *(int*)((char*)iVar2 + state * 8);
                        int iVar1_ = iVar3 + 1 + iVar6;
                        iVar6++;
                        if (this->animation_data == 0 || iVar2 == 0) {
                            ShowError("range error");
                        }
                        if (iVar1_ < iVar3 || iVar1_ > *(int*)((char*)iVar2 + 4 + state * 8)) {
                            iVar6 = 0;
                        }
                    }
                    this->field138_0x90 = state;
                    FUN_0041fcc0(this->animation_data, *(int*)((char*)this->field136_0x88 + state * 8) + iVar6);
                    int* piVar5_ = (int*)((char*)this->field136_0x88 + this->field138_0x90 * 8);
                    if (piVar5_[1] == *piVar5_) {
                        this->flags |= 4;
                    }
                    this->flags &= ~0x20;
                }
                return;
            }
        }
        ShowError("range error");
    }
    ShowError("Sprite::SetState(%d) %s", state, &this->filename);
}

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
            int iVar1 = this->animation_data->smack_struct->total_frames;
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
    if (this->animation_data == (void *)0x0) {
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
        Animation* anim = this->animation_data;
        int iVar3;
        if (anim == 0) {
            iVar3 = *(int *)((char*)this->field136_0x88 + 4 + this->field138_0x90 * 8);
        }
        else {
            iVar3 = *(int *)((char*)this->field136_0x88 + 4 + this->field138_0x90 * 8) - anim->smack_struct->current_frame;
        }
        if (iVar3 == 1) {
            if ((this->flags & 0x200) == 0) {
                FUN_0041fcc0(anim,*(int *)((char*)this->field136_0x88 + this->field138_0x90 * 8));
            }
            else {
                FUN_0041fcb0((int)anim);
            }
            if ((this->flags & 1) == 0) {
                bVar5 = 1;
            }
        }
        else {
            FUN_0041fcb0((int)anim);
        }
    }
    unsigned int uVar2 = this->flags;
    if ((uVar2 & 0x100) != 0) {
        return bVar5;
    }
    if ((uVar2 & 2) == 0) {
        Animation* anim = this->animation_data;
        if ((uVar2 & 8) == 0) {
            if (anim == 0) {
                y = y + -1;
            }
            else {
                y = y + anim->vbuffer->height + -1;
            }
        }
        else if (anim == 0) {
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
    FUN_0041be20(0, (int)this->animation_data, this->field139_0x94, x, y, iVar3, param_3, param_4);
    if ((this->flags & 1) == 0) {
        return bVar5;
    }
    return 0;
}


void Sprite::SetRange(int param_1, int param_2, int param_3)
{
    if (this->field_0xac <= param_1) {
        ShowError("s_Sprite__SetRange_1__s__d_00436c48", &this->filename, param_1);
    }
    if ((param_2 > 0) && (param_3 > 0)) {
        int* piVar1 = (int*)((char*)this->field136_0x88 + param_1 * 8);
        *piVar1 = param_2;
        piVar1[1] = param_3;
        this->flags = this->flags | 0x20;
        return;
    }
    ShowError("s_Sprite__SetRange_2__s__d_range___00436c20", &this->filename, param_1, param_2, param_3);
}

void Sprite::SetLogic(int param_1, int param_2)
{
    if (this->field140_0x98 == 0) {
        this->AllocateLogic(1);
    }

    int iVar2 = 0;
    if (0 < this->field142_0xa0) {
        int* piVar1 = (int*)(this->field140_0x98 + 4);
        do {
            if (*piVar1 == 0) {
                *(int*)(this->field140_0x98 + iVar2 * 8) = param_1;
                *(int*)(this->field140_0x98 + 4 + iVar2 * 8) = param_2;
                return;
            }
            piVar1 = piVar1 + 2;
            iVar2 = iVar2 + 1;
        } while (iVar2 < this->field142_0xa0);
    }

    ShowError("s_Sprite__SetLogic__s_00436c64", &this->filename);
}

void Sprite::AllocateLogic(int param_1)
{
    if (this->field140_0x98 != 0) {
        Array_Cleanup(this->field140_0x98, 8, *(int*)(this->field140_0x98 - 4), (void*)0x405770);
        FreeFromGlobalHeap((int*)(this->field140_0x98 - 4));
        this->field140_0x98 = 0;
    }

    this->field142_0xa0 = param_1;
    int* piVar4 = 0;
    int* piVar1 = AllocateMemory_Wrapper(param_1 * 8 + 4);

    if (piVar1 != 0) {
        piVar4 = piVar1 + 1;
        *piVar1 = param_1;
        FUN_00424b00(piVar4, 8, param_1, (void*)0x41d850, (void*)0x405770);
    }

    this->field140_0x98 = (int)piVar4;
    if (0 < this->field142_0xa0) {
        int iVar3 = 0;
        int iVar2 = 0;
        do {
            iVar3 = iVar3 + 8;
            iVar2 = iVar2 + 1;
            *(int*)(this->field140_0x98 - 8 + iVar3) = 0;
            *(int*)(this->field140_0x98 - 4 + iVar3) = 0;
        } while (iVar2 < this->field142_0xa0);
    }
}

void Sprite::CleanArray10()
{
    memset((char*)this + 4, 0, 40);
}

void Sprite::Free() {
    if (this->vbuffer != 0) {
        this->vbuffer->Free();
        //FUN_00424940(this->vbuffer);
        this->vbuffer = 0;
        //this->field0_0x0 = 0;
    }
}
