#include "Animation.h"
#include <string.h>
#include "string.h"

typedef unsigned int size_t;


extern "C" void Init(Animation* anim);
extern "C" void Array_Cleanup(int param_1, int param_2, int param_3, void* param_4);
extern "C" void FreeFromGlobalHeap(int* param_1);
extern "C" int* AllocateMemory_Wrapper(int size);
extern "C" void FUN_00424b00(int* param_1, int param_2, int param_3, void* param_4, void* param_5);
extern "C" int FUN_004224d0();
extern "C" char DAT_00436970[];
extern "C" void* DAT_00436968;
extern "C" int DAT_004373bc;
extern "C" void* FUN_004249c0(int size);
extern "C" void FUN_0041fc20(Animation* anim, int, int);
extern "C" int FUN_00421d10(void*);
extern "C" int FUN_00421af0();
extern "C" int _SmackWait_4(SmackStruct*);
extern "C" int* FUN_004224f0();
extern "C" int* FUN_004224e0();
extern "C" void FUN_0041acf0(void*, int, int, int, int, int, int, int, int);
extern "C" void FUN_0041fcb0(int);

void Animation::SetRange(int param_1, int param_2, int param_3)
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

void Animation::SetState(int param_1)
{
    if (param_1 == -1) {
        this->field138_0x90 = -1;
        return;
    }

    int iVar6 = 0;
    if ((this->animation_data == 0) || (this->animation_data->field_0x18 == 0)) {
        Init(this);
    }

    if (param_1 < 0 || param_1 >= this->field_0xac) {
        ShowError("s_Sprite__SetState_0__d__s_00436ba0", param_1, &this->filename);
    }

    if (this->animation_data == 0) {
        ShowError("s_range_error_00436bbc");
    }

    int current_frame = this->animation_data->smacker_data->current_frame;
    if (this->field136_0x88 == 0) {
        ShowError("s_range_error_00436bbc");
    }

    int* piVar5 = (int*)((char*)this->field136_0x88 + param_1 * 8);
    int bVar4 = (current_frame >= *piVar5) && (current_frame <= piVar5[1]);
    if (!bVar4) {
        this->flags |= 0x20;
    }

    if ((this->flags & 0x20) || (this->field138_0x90 != param_1)) {
        if (this->flags & 0x10) {
            int iVar2 = (int)this->field136_0x88;
            iVar6 = this->animation_data->smacker_data->current_frame - *(int*)(iVar2 + this->field138_0x90 * 8);
            int iVar3 = *(int*)(iVar2 + param_1 * 8);
            int iVar1_2 = iVar3 + 1 + iVar6;
            iVar6 = iVar6 + 1;

            if (this->animation_data == 0 || iVar2 == 0) {
                ShowError("s_range_error_00436bbc");
            }

            int bVar4_2 = (iVar1_2 >= iVar3) && (iVar1_2 <= *(int*)(iVar2 + 4 + param_1 * 8));
            if (!bVar4_2) {
                iVar6 = 0;
            }
        }

        this->field138_0x90 = param_1;
        this->animation_data->UpdateSmacker(*(int*)((char*)this->field136_0x88 + param_1 * 8) + iVar6);
        piVar5 = (int*)((char*)this->field136_0x88 + this->field138_0x90 * 8);
        if (piVar5[1] == *piVar5) {
            this->flags |= 4;
        }
        this->flags &= ~0x20;
    }
}

void Smack::UpdateSmacker(int param_1)
{
    if (this->smacker_data != 0) {
        if (*(char*)(DAT_00436970 + 0x46) == '\x02') {
            SmackSoundUseDirectSound((int)this->smacker_data, 0);
        }
        SmackGoto(this->smacker_data, param_1);
        if (*(char*)(DAT_00436970 + 0x46) == '\x02') {
            SmackSoundUseDirectSound((int)this->smacker_data, 1);
        }
    }
}

void Animation::SetLogic(int param_1, int param_2)
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

void Animation::AllocateLogic(int param_1)
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

void Animation::CheckRanges1()
{
    if (this->animation_data == 0) {
        ShowError("s_error_Sprite__CheckRanges0_00436c04");
    }
    if (this->field136_0x88 == 0) {
        ShowError("s_error_Sprite__CheckRanges1_00436be8");
    }

    int iVar4 = 0;
    if (0 < this->field_0xac) {
        int iVar3 = 0;
        do {
            int* piVar2 = (int*)((char*)this->field136_0x88 + 4 + iVar3);
            int total_frames = this->animation_data->smacker_data->total_frames;
            if (total_frames < *piVar2) {
                *piVar2 = total_frames;
            }
            piVar2 = (int*)((char*)this->field136_0x88 + iVar3);
            int start_frame = *piVar2;
            if (piVar2[1] < start_frame) {
                ShowError("s_bad_range__d__start____d_in__s_00436bc8", iVar4, start_frame, &this->filename);
            }
            iVar3 = iVar3 + 8;
            iVar4 = iVar4 + 1;
        } while (iVar4 < this->field_0xac);
    }
}

void Animation::ToBuffer2(void* param_1)
{
    if (this->smkStruct == 0) {
        ShowError("s_Animation__ToBuffer_____No_smk_d_004370e4");
    }
    int uVar1 = FUN_004224d0();
    this->field_0x28 = uVar1;
    SmackStruct* piVar2 = SmackBufferOpen(uVar1, 4, 4, 4, 0, 0);
    this->smkStruct2 = piVar2;
    if (piVar2 == 0) {
        ShowError("s_Animation__ToBuffer_____Buffer_c_00437144");
    }
    if (this->virtualBuffer != 0) {
        ShowError("s_Animation__ToBuffer_____Virtual_B_0043710c");
    }
    this->virtualBuffer = (VBuffer*)param_1;
    unsigned int uVar3 = (unsigned int)*(char*)this->smkStruct2;
    int uVar1_2 = this->virtualBuffer->GetSomething();
    SmackToBuffer(this->smkStruct, 0, 0, this->smkStruct->width * 2, this->smkStruct->height, (void*)uVar1_2, uVar3);
}

void Animation::ToBuffer()
{
    if (this->smkStruct == 0) {
        ShowError("s_Animation__ToBuffer_____No_smk_d_004370e4");
    }
    this->VBInit();
    this->ToBuffer2(this->virtualBuffer);
}

void Animation::CleanArray10()
{
    memset((char*)this + 4, 0, 40);
}


/*
Function: VBInit
Address: 0x41FD80

Disassembled:
MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41fdfc
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
PUSH ESI
PUSH EDI
CMP dword ptr [ECX + 0x8],0x0
MOV EDI,ECX
JZ 0x32
PUSH 0x4370ac
CALL 0x00419110
PUSH 0x30
CALL 0x004249c0
MOV dword ptr [EBP + -0x10],EAX
ADD ESP,0x4
MOV dword ptr [EBP + -0x4],0x0
MOV ESI,EAX
MOV EAX,0x0
TEST ESI,ESI
JZ 0x63
MOV EAX,dword ptr [EDI + 0xc]
MOV ECX,dword ptr [EAX + 0x8]
MOV EDX,dword ptr [EAX + 0x4]
PUSH ECX
PUSH EDX
MOV ECX,ESI
CALL 0x0041a9f0
MOV dword ptr [EBP + -0x4],0xffffffff
MOV dword ptr [EDI + 0x8],EAX
MOV EAX,dword ptr [EBP + -0xc]
MOV FS:[0x0],EAX
POP EDI
POP ESI
MOV ESP,EBP
POP EBP
RET

*/
void Animation::VBInit()
{
    try {
        if (this->virtualBuffer != 0) {
            ShowError("s_VBuffer_already_created_004370ac");
        }
        VBuffer* new_buffer = (VBuffer*)FUN_004249c0(0x30);
        if (new_buffer != 0) {
            this->virtualBuffer = new_buffer->VirtualBufferCreateAndClean(this->smkStruct->width, this->smkStruct->height);
        }
    } catch (...) {
        // Catch all exceptions
    }
}

/*
Function: Animation::Free
Address: 0x41FE20

PUSH ESI
PUSH EDI
MOV ESI,dword ptr [ECX + 0x8]
MOV EDI,ECX
TEST ESI,ESI
JZ 0x22
MOV ECX,ESI
CALL 0x0041aa10
PUSH ESI
CALL 0x00424940
MOV dword ptr [EDI + 0x8],0x0
ADD ESP,0x4
MOV dword ptr [EDI + 0x18],0x0
POP EDI
POP ESI
RET

*/
extern "C" void FUN_0041aa10(VBuffer* p);
extern "C" void FUN_00424940(VBuffer* p);

void Animation::Free() {
    if (this->virtualBuffer != 0) {
        FUN_0041aa10(this->virtualBuffer);
        FUN_00424940(this->virtualBuffer);
        this->virtualBuffer = 0;
        this->field0_0x0 = 0;
    }
}

/*
Function: DoFrame
Address: 0x41FCA0

MOV EAX,dword ptr [ECX + 0xc]
TEST EAX,EAX
JZ 0xE
PUSH EAX
CALL dword ptr [0x00440518]
RET
*/
void Animation::DoFrame()
{
  if (this->smkStruct != 0) {
    SmackDoFrame(this->smkStruct);
  }
}

/*
Function: MainLoop
Address: 0x420020

PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
PUSH EBP
CMP dword ptr [ESI + 0xc],0x0
JZ 0x117
MOV ECX,dword ptr [ESI + 0x18]
MOV EDI,0x1
MOV EAX,dword ptr [ECX + 0x1c]
PUSH EAX
CALL 0x0041ac50
MOV ECX,dword ptr [ESI + 0xc]
CMP dword ptr [ECX + 0xc],EDI
JL 0x10F
XOR EBX,EBX
MOV EAX,dword ptr [ESI + 0xc]
CMP dword ptr [EAX + 0x68],EBX
JZ 0x44
PUSH 0x100
MOV ECX,ESI
PUSH EBX
CALL 0x0041fc20
MOV ECX,ESI
MOV EBP,0x4
CALL 0x0041fca0
PUSH 0x1
MOV ECX,dword ptr [0x00436968]
CALL 0x00421d10
TEST EAX,EAX
JNZ 0x10F
TEST dword ptr [ESI + 0x1c],EBP
JNZ 0xAE
MOV EAX,[0x00436968]
MOV ECX,0x0
MOV EAX,dword ptr [EAX + 0x1a0]
TEST EAX,EAX
JZ 0x84
MOV ECX,dword ptr [EAX + 0x8]
AND ECX,0x2
TEST ECX,ECX
JNZ 0x8E
TEST byte ptr [EAX + 0xc],0x2
JNZ 0x10B
MOV EAX,0x0
CMP dword ptr [0x004373bc],EBX
JZ 0xAA
CALL 0x00421af0
SUB EAX,0x1b
CMP EAX,0x1
SBB EAX,EAX
NEG EAX
TEST EAX,EAX
JNZ 0x10B
MOV EAX,dword ptr [ESI + 0xc]
PUSH EAX
CALL dword ptr [0x00440530]
TEST EAX,EAX
JNZ 0x50
MOV EBP,dword ptr [ESI + 0x18]
CALL 0x004224f0
MOV EAX,dword ptr [EAX]
DEC EAX
PUSH EAX
PUSH EBX
CALL 0x004224e0
MOV EAX,dword ptr [EAX]
MOV ECX,dword ptr [EBP + 0x24]
DEC EAX
MOV EDX,dword ptr [EBP + 0x20]
PUSH EAX
PUSH EBX
PUSH ECX
MOV EAX,dword ptr [EBP + 0x2c]
PUSH EDX
MOV ECX,dword ptr [EBP + 0x28]
PUSH EAX
PUSH ECX
MOV ECX,EBP
CALL 0x0041acf0
MOV ECX,dword ptr [ESI + 0xc]
MOV EAX,dword ptr [ECX + 0xc]
DEC EAX
CMP EAX,EDI
JLE 0x10F
MOV ECX,ESI
INC EDI
CALL 0x0041fcb0
MOV EAX,dword ptr [ESI + 0xc]
CMP dword ptr [EAX + 0xc],EDI
JGE 0x2F
JMP 0x10F
OR dword ptr [ESI + 0x20],0x1
MOV ECX,dword ptr [ESI + 0x18]
CALL 0x0041ac80
POP EBP
POP EDI
POP ESI
POP EBX
RET
*/
void Animation::MainLoop()
{
    if (this->smkStruct == 0) {
        return;
    }

    VBuffer* vb = (VBuffer*)this->field0_0x0;
    vb->SetCurrentVideoMode(*(int*)((char*)vb + 0x1c));

    if (this->smkStruct->total_frames > 0) {
        for (int i = 1; i <= this->smkStruct->total_frames; i++) {
            if (*(int*)((char*)this->smkStruct + 0x68) != 0) {
                FUN_0041fc20(this, 0, 0x100);
            }
            this->DoFrame();

            do {
                if (FUN_00421d10(DAT_00436968) != 0) {
                    goto LAB_0042012f;
                }
                if ((this->flags & 4) == 0) {
                    unsigned int uVar3 = 0;
                    int iVar1 = *(int*)((char*)DAT_00436968 + 0x1a0);
                    if (iVar1 != 0) {
                        uVar3 = *(unsigned int*)(iVar1 + 8) & 2;
                    }
                    if (uVar3 == 0 && (*(char*)(iVar1 + 0xc) & 2) == 0) {
                        char bVar5 = 0;
                        if (DAT_004373bc != 0) {
                            bVar5 = FUN_00421af0() == 0x1b;
                        }
                        if (bVar5) {
                            *(int*)&this->virtualBuffer |= 1;
                            goto LAB_0042012f;
                        }
                    }
                }
            } while (_SmackWait_4(this->smkStruct) != 0);

            void* this_00 = this->field0_0x0;
            int* piVar2 = FUN_004224f0();
            int iVar1 = *piVar2 - 1;
            int iVar6 = 0;
            piVar2 = FUN_004224e0();
            FUN_0041acf0(this_00, *(int*)((char*)this_00 + 0x28), *(int*)((char*)this_00 + 0x2c), *(int*)((char*)this_00 + 0x20), *(int*)((char*)this_00 + 0x24), 0, *piVar2 - 1, iVar6, iVar1);

            if (this->smkStruct->total_frames - 1 <= i) {
                break;
            }
            FUN_0041fcb0((int)this);
        }
    }

LAB_0042012f:
    vb->InvalidateVideoMode();
}
