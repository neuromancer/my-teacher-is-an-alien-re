#include "Animation.h"
#include "Smacker.h"
#include "VBuffer.h"
#include <string.h>
#include "string.h"

extern "C" {
    void OpenAndConvertToBuffer(Animation* anim, char* filename);
    void Play(Animation* anim, unsigned int flags);
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
    void FUN_0041fcb0(int);
}


/* Function start: 0x41FCA0 */
void Animation::DoFrame()
{
  if (this->smack_struct != 0) {
    SmackDoFrame(this->smack_struct);
  }
}

/* Function start: 0x41FD20 */
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
    //this->animation_data = this;
    this->CheckRanges1();
    this->VBInit();
    this->ToBuffer();
}

/* Function start: 0x41FD80 */
void Animation::VBInit()
{
    try {
        if (this->vbuffer->data != 0) {
            ShowError("s_VBuffer_already_created_004370ac");
        }
        VBuffer* new_buffer = (VBuffer*)FUN_004249c0(0x30);
        if (new_buffer != 0) {
            new_buffer->VirtualBufferCreateAndClean(this->smack_struct->width, this->smack_struct->height);
        }
    } catch (...) {
        // Catch all exceptions
    }
}

/* Function start: 0x41FE70 */
void Animation::ToBuffer()
{
    if (this->smack_struct == 0) {
        ShowError("s_Animation__ToBuffer_____No_smk_d_004370e4");
    }
    this->VBInit();
    this->ToBuffer2(this);
}

/* Function start: 0x41FEA0 */
void Animation::ToBuffer2(void* param_1)
{
    if (this->smack_struct == 0) {
        ShowError("s_Animation__ToBuffer_____No_smk_d_004370e4");
    }
    int uVar1 = FUN_004224d0();
    this->vbuffer->field_0x28 = uVar1;
    SmackStruct* piVar2 = SmackBufferOpen(uVar1, 4, 4, 4, 0, 0);
    this->smacker_buffer = piVar2;
    if (piVar2 == 0) {
        ShowError("s_Animation__ToBuffer_____Buffer_c_00437144");
    }
    if (this->vbuffer->data != 0) {
        ShowError("s_Animation__ToBuffer_____Virtual_B_0043710c");
    }
    this->vbuffer->data = (VBuffer*)param_1;
    unsigned int uVar3 = (unsigned int)*(char*)this->smacker_buffer;
    int uVar1_2 = this->vbuffer->GetSomething();
    SmackToBuffer(this->smack_struct, 0, 0, this->smack_struct->width * 2, this->smack_struct->height, (void*)uVar1_2, uVar3);
}

/* Function start: 0x41FF30 */
void Animation::Play(char* filename, unsigned int flags)
{
    Sprite::Sprite(filename);
    this->flags |= flags;
    this->MainLoop();
}

/* Function starT: 0x420020 */
void Animation::MainLoop()
{
    if (this->smack_struct == 0) {
        return;
    }

    this->vbuffer->SetCurrentVideoMode(*(int*)((char*)this->vbuffer + 0x1c));

    if (this->smack_struct->total_frames > 0) {
        for (int i = 1; i <= this->smack_struct->total_frames; i++) {
            if (*(int*)((char*)this->smack_struct + 0x68) != 0) {
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
                            *(int*)&this->vbuffer->videoMode |= 1;
                            goto LAB_0042012f;
                        }
                    }
                }
            } while (_SmackWait_4(this->smack_struct) != 0);

            void* this_00 = this;
            int* piVar2 = FUN_004224f0();
            int iVar1 = *piVar2 - 1;
            int iVar6 = 0;
            piVar2 = FUN_004224e0();
            FUN_0041acf0(this_00, *(int*)((char*)this_00 + 0x28), *(int*)((char*)this_00 + 0x2c), *(int*)((char*)this_00 + 0x20), *(int*)((char*)this_00 + 0x24), 0, *piVar2 - 1, iVar6, iVar1);

            if (this->smack_struct->total_frames - 1 <= i) {
                break;
            }
            FUN_0041fcb0((int)this);
        }
    }

LAB_0042012f:
    this->vbuffer->InvalidateVideoMode();
}