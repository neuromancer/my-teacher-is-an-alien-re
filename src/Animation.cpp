#include "Animation.h"
#include "VBuffer.h"
#include <windows.h>

extern "C" {
    void __cdecl ShowError(const char*, ...);
    void FUN_0041fbd3();
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
    void __fastcall Animation_VBInit(Animation* this_ptr);
    void __fastcall VBuffer_destructor(VBuffer* this_ptr);
    VBuffer* __fastcall VBuffer_VirtualBufferCreateAndClean(VBuffer* this_ptr, int width, int height);
    void __fastcall Sprite_CloseSmackerFile(void* this_ptr);
    void* __stdcall SmackOpen(char* filename, int flags, int extra);
    void __stdcall SmackSoundOnOff(void* smack, int onoff);
    void __stdcall SmackGoto(void* smack, int frame);
    void __stdcall SmackNextFrame(void* smack);
    int __stdcall SmackDoFrame(void* smack);
    void __stdcall SmackBufferClose(void* smack);
    void __stdcall SmackClose(void* smack);
    void __stdcall SmackBufferNewPalette(void* smack, void* palette, int);
    void __stdcall SmackColorRemap(void* smack, void* remap_table, int num_entries, int);
    void FUN_0041eb90(void*, int, int);
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
    extern char DAT_00436970[];
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

/* Function start: 0x41FAE0 */
Animation::Animation(char* filename)
{
    try {
        this->CleanArray10();
        this->OpenAndConvertToBuffer(filename);
    } catch (...) {
        // TODO: Figure out what the exception handler does
    }
}

/* Function start: 0x41FA50 */
Animation::Animation()
{
    this->vtable = (void**)0x431570;
    *(int*)((char*)this + 0x10) = 0;
    *(int*)((char*)this + 0x14) = 0;
    this->CleanArray10();
}

/* Function start: 0x41FB60 */
void Animation::CleanArray10()
{
    int* p = (int*)((char*)this + 4);
    for (int i = 0; i < 10; i++) {
        *p++ = 0;
    }
}

/* Function start: 0x41FB70 */
Animation::~Animation()
{
    this->vtable = (void**)0x431570;
    this->FreeVBuffer();
    this->CloseSmackerBuffer();
    this->CloseSmackerFile();
    FUN_0041fbd3();
}

/* Function start: 0x41FD20 */
int Animation::Open(char* filename, int param_2, int param_3)
{
    if (this->smk != 0) {
        return 1;
    }

    if (*(char*)(DAT_00436970 + 0x46) != '\x02') {
        param_2 = param_2 & 0xfff01fff;
    }

    this->smk = (SmkObject*)SmackOpen(filename, param_2, param_3);
    if (this->smk == 0) {
        ShowError("Animation::Open - Cannot open file %s", filename);
    }
    return 1;
}

/* Function start: 0x41FD80 */
void Animation::VBInit()
{
    if (this->vbuffer != 0) {
        ShowError("Animation::VBInit() - Virtual Buffer already defined");
    }

    VBuffer* vbuffer = (VBuffer*)AllocateMemory_Wrapper(0x30);
    if (vbuffer != 0) {
        this->vbuffer = VBuffer_VirtualBufferCreateAndClean(vbuffer, this->smk->width, this->smk->height);
    }
}

/* Function start: 0x41FE20 */
void Animation::FreeVBuffer()
{
    if (this->vbuffer != 0) {
        VBuffer *vbuffer = this->vbuffer;
        VBuffer_destructor(vbuffer);
        FreeFromGlobalHeap(vbuffer);
        this->vbuffer = 0;
    }
    this->data = 0;
}

/* Function start: 0x41FE50 */
void Animation::OpenAndConvertToBuffer(char* filename)
{
    Animation_Open(this, filename, 0xfe000, -1);
    this->ToBuffer();
}

/* Function start: 0x41FE70 */
void Animation::ToBuffer()
{
    if (this->smk == 0) {
        ShowError(s_Animation_ToBuffer_No_smk_defined);
    }
    Animation_VBInit(this);
    this->ToBuffer(this->vbuffer);
}

/* Function start: 0x41FEA0 */
void Animation::ToBuffer(VBuffer* buffer)
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
    vbuffer->SetCurrentVideoMode(vbuffer->field_0x1c);

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
    vbuffer->InvalidateVideoMode();
}

/* Function start: 0x41FAC0 */
void Animation::Delete(unsigned char param_1)
{
    this->~Animation();
    if ((param_1 & 1) != 0) {
        FreeFromGlobalHeap(this);
    }
}

/* Function start: 0x41FBE0 */
void Animation::CloseSmackerFile()
{
    if (this->smk != 0) {
        SmackClose(this->smk);
        this->smk = 0;
    }
}

/* Function start: 0x41FC00 */
void Animation::CloseSmackerBuffer()
{
    if (this->smack_buffer != 0) {
        SmackBufferClose(this->smack_buffer);
        this->smack_buffer = 0;
    }
}

/* Function start: 0x41FC20 */
void Animation::SetPalette(unsigned int param_1, unsigned int param_2)
{
    if (this->smk != 0 && this->smack_buffer != 0) {
        this->data->SetCurrentVideoMode(this->data->field_0x1c);
        if (this->smk->field_0x68 != 0) {
            SmackBufferNewPalette(this->smack_buffer, (char*)this->smk + 0x6c, 0);
            SmackColorRemap(this->smk, (char*)this->smack_buffer + 0x3c, *(int*)((char*)this->smack_buffer + 0x2c), *(int*)((char*)this->smack_buffer + 0x43c));
        }
        FUN_0041eb90((char*)this->smk + 0x6c, param_1, param_2);
        this->data->InvalidateVideoMode();
    }
}

/* Function start: 0x41FCA0 */
void Animation::DoFrame(Animation* anim)
{
    if (anim->smk != 0) {
        SmackDoFrame(anim->smk);
    }
}

/* Function start: 0x41FCB0 */
void Animation::NextFrame()
{
    if (this->smk != 0) {
        SmackNextFrame(this->smk);
    }
}

/* Function start: 0x41FCC0 */
void Animation::GotoFrame(int frame)
{
    if (this->smk != 0) {
        if (*(char*)(DAT_00436970 + 0x46) == '\x02') {
            SmackSoundOnOff(this->smk, 0);
        }
        SmackGoto(this->smk, frame);
        if (*(char*)(DAT_00436970 + 0x46) == '\x02') {
            SmackSoundOnOff(this->smk, 1);
        }
    }
}
