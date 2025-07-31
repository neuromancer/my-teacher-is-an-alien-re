#include "Animation.h"
#include "VBuffer.h"
#include <windows.h>

extern "C" {
    void __cdecl ShowError(const char*);
    void* __stdcall SmackBufferOpen(void* smack, int, int, int, int, int);
    void __stdcall SmackToBuffer(void* smack, int, int, int, int, void*, int);
    void* FUN_004224d0();
    void* FUN_0041ac30(int);
    void* __cdecl AllocateMemory_Wrapper(int size);
    void* __cdecl CreatePaletteBuffer(void*);
    void FUN_0041eb50(void*, int, int);
    void FUN_00419390();
    void __fastcall Animation_Open(Animation* this_ptr, char* filename, int, int);
    void __fastcall Animation_ToBuffer_ext(Animation* this_ptr);
    void __fastcall Sprite_CloseSmackerFile(void* this_ptr);
    void FUN_0041eb70(void*, int, int);
    void FUN_0041ea80(void*);
    void __cdecl FreeFromGlobalHeap(void*);
    int __stdcall SmackWait(void*);
    void FUN_0041fc20(Animation*, int, int);
    int FUN_00421d10(void*);
    int FUN_00421af0();
    void* FUN_004224f0();
    void* FUN_004224e0();
    void FUN_0041acf0(void*, int, int, int, int, int, int, int, int);
    void FUN_0041fcb0(int);
    void FUN_0041ac50(int);
    void FUN_0041ac80();

    extern void* DAT_00436968;
    extern int DAT_004373bc;
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
void Animation::ToBuffer(AnimationData* buffer)
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

/* Function start: 0x41FF30 */
void Animation::Play(char* filename, unsigned int flags)
{
    this->flags = flags;
    this->palette = 0;

    if ((flags & 1) == 0) {
        void* mem = AllocateMemory_Wrapper(8);
        if (mem != 0) {
            this->palette = CreatePaletteBuffer(mem);
        }
        FUN_0041eb50(this->palette, 0, 0x100);
    }

    if ((this->flags & 2) == 0) {
        FUN_00419390();
    }

    Animation_Open(this, filename, 0xfe000, -1);
    Animation_ToBuffer_ext(this);
    this->MainLoop();
    Sprite_CloseSmackerFile(this);

    if (this->palette != 0) {
        FUN_00419390();
        FUN_0041eb70(this->palette, 0, 0x100);
        if (this->palette != 0) {
            FUN_0041ea80(this->palette);
            FreeFromGlobalHeap(this->palette);
        }
    }
}

/* Function start: 0x420020 */
void Animation::MainLoop()
{
    if (smk == 0) {
        return;
    }

    VBuffer* vbuffer = (VBuffer*)this->data;
    VBuffer_SetCurrentVideoMode(vbuffer, vbuffer->field19_0x1c);

    int frame = 1;
    if (smk->frame_count > 0) {
        do {
            if (smk->field_0x68 != 0) {
                FUN_0041fc20(this, 0, 0x100);
            }
            DoFrame(this);
            do {
                if (FUN_00421d10(DAT_00436968) != 0) {
                    goto end_loop;
                }
                if ((flags & 4) == 0) {
                    unsigned int uVar3 = 0;
                    int iVar1 = *(int*)((char*)DAT_00436968 + 0x1a0);
                    if (iVar1 != 0) {
                        uVar3 = *(unsigned int*)(iVar1 + 8) & 2;
                    }
                    if (uVar3 != 0 || (*(char*)(iVar1 + 0xc) & 2) == 0) {
                        int bVar5 = 0;
                        if (DAT_004373bc != 0) {
                            bVar5 = FUN_00421af0() == 0x1b;
                        }
                        if (!bVar5) {
                            goto wait;
                        }
                    }
                    *(unsigned int*)&this->palette |= 1;
                    goto end_loop;
                }
            wait:;
            } while (SmackWait(smk) != 0);

            void* piVar2 = FUN_004224f0();
            int iVar1 = *(int*)piVar2 - 1;
            int iVar6 = 0;
            piVar2 = FUN_004224e0();
            FUN_0041acf0(vbuffer, *(int*)((char*)vbuffer + 0x28), *(int*)((char*)vbuffer + 0x2c), *(int*)((char*)vbuffer + 0x20), *(int*)((char*)vbuffer + 0x24), 0, *(int*)piVar2 - 1, iVar6, iVar1);

            if (smk->frame_count - 1 <= frame) {
                break;
            }
            frame++;
            FUN_0041fcb0((int)this);
        } while (frame <= smk->frame_count);
    }
end_loop:
    VBuffer_InvalidateVideoMode(vbuffer);
}
