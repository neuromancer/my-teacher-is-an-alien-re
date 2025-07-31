#include "Animation.h"

extern "C" {
    void __cdecl ShowError(const char*);
    void* __stdcall SmackBufferOpen(void* smack, int, int, int, int, int);
    void __stdcall SmackToBuffer(void* smack, int, int, int, int, void*, int);
    void* FUN_004224d0();
    void* FUN_0041ac30(int);
}

const char* s_Animation_ToBuffer_No_smk_defined = "Animation::ToBuffer() - No smk defined";
const char* s_Animation_ToBuffer_Virtual_Buffer_already_defined = "Animation::ToBuffer() - Virtual Buffer already defined";
const char* s_Animation_ToBuffer_Buffer_creation_failed = "Animation::ToBuffer() - Buffer creation failed";

/* Function start: 0x41D0F5 */
void Animation::AnimationInit()
{
    this->smk = 0;
    this->data = 0;
}

/* Function start: 0x41FEA0 */
void Animation::ToBuffer2(AnimationData* buffer)
{
    if (this->smk == 0) {
        ShowError(s_Animation_ToBuffer_No_smk_defined);
    }

    this->smack_handle = FUN_004224d0();
    this->smack_buffer = (int*)SmackBufferOpen(this->smack_handle, 4, 4, 4, 0, 0);

    if (this->smack_buffer == 0) {
        ShowError(s_Animation_ToBuffer_Buffer_creation_failed);
    }

    if (this->data != 0) {
        ShowError(s_Animation_ToBuffer_Virtual_Buffer_already_defined);
    }

    this->data = buffer;
    unsigned int uVar3 = *(unsigned char*)this->smack_buffer;
    void* uVar1 = FUN_0041ac30((int)buffer);
    SmackToBuffer(this->smk, 0, 0, this->smk->width, this->smk->height, uVar1, uVar3);
}
