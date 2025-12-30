#include "globals.h"
#include "Animation.h"
#include "Palette.h"
#include "PaletteUtils.h"
#include "VBuffer.h"
#include "string.h"
#include <smack.h>
#include "GameConfig.h"
#include "JoystickManager.h"

#include "Memory.h"
#include <windows.h>

extern "C" {
void FUN_0041fbd3();
void *GetGameWindowHandle();
void *FUN_004224f0();
void *FUN_004224e0();
void FUN_0041ac50(int);
void FUN_0041ac80();
}

int WaitForInput();

/* Function start: 0x419390 */
void BlankScreen() {
  if (g_WorkBuffer_00436974 != 0) {
    g_WorkBuffer_00436974->ClearScreen(0);
    VBuffer *vbuffer = g_WorkBuffer_00436974;
    vbuffer->CallBlitter5(
        vbuffer->clip_x1, vbuffer->clip_x2, vbuffer->saved_video_mode,
        vbuffer->video_mode_lock_count, 0, *(int *)FUN_004224e0() - 1, 0,
        *(int *)FUN_004224f0() - 1);
  }
}

extern "C" {

/* Function start: 0x41EB90 */
void __cdecl SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count) {
  SetPaletteEntries_(start, count, (unsigned char *)palette + start * 3);
}
}

/* Function start: 0x41FA50 */
Animation *Animation::Init() {
  int *p = &field_0x10;
  *p = 0;
  *(p + 1) = 0;
  try {
    CleanArray10();
  } catch (...) {
  }
  return this;
}

/* Function start: 0x41FAC0 */
void Animation::Delete(unsigned char param_1) {
  this->~Animation();
  if ((param_1 & 1) != 0) {
    FreeMemory(this);
  }
}

/* Function start: 0x41FAE0 */
Animation *Animation::InitWithFilename(char *filename) {
  int *p = &field_0x10;
  *p = 0;
  *(p + 1) = 0;
  try {
    CleanArray10();
    OpenAndConvertToBuffer(filename);
  } catch (...) {
  }
  return this;
}

/* Function start: 0x41FB60 */
void Animation::CleanArray10() {
  int *p = (int *)((char *)this + 4);
  for (int i = 0; i < 10; i++) {
    *p++ = 0;
  }
}

/* Function start: 0x41FB70 */
Animation::~Animation() {
  try {
    this->FreeVBuffer();
    this->CloseSmackerBuffer();
    this->CloseSmackerFile();
  } catch (...) {
  }
  //FUN_0041fbd3();
}

/* Function start: 0x41FBE0 */
void Animation::CloseSmackerFile() {
  if (this->smk != 0) {
    SmackClose(this->smk);
    this->smk = 0;
  }
}

/* Function start: 0x41FC00 */
void Animation::CloseSmackerBuffer() {
  if (this->smack_buffer != 0) {
      SmackBufferClose(this->smack_buffer);
      this->smack_buffer = 0;
  }
}

/* Function start: 0x41FC20 */
int Animation::SetPalette(unsigned int param_1, unsigned int param_2) {
  if (this->smk != 0 && this->smack_buffer != 0) {
    this->data->SetCurrentVideoMode(this->data->handle);
    if (this->smk->NewPalette != 0) {
      SmackBufferNewPalette(this->smack_buffer, this->smk->Palette, 0);
      SmackColorRemap(this->smk, this->smack_buffer->Palette,
                      this->smack_buffer->PalColorsInUse,
                      this->smack_buffer->Unk43C);
    }
    SetPaletteEntriesAnimation((char *)this->smk->Palette, param_1, param_2);
    this->data->InvalidateVideoMode();
    return 0;
  }
  return 0;
}

/* Function start: 0x41FCA0 */
void Animation::DoFrame() {
  if (this->smk != 0) {
    SmackDoFrame(this->smk);
  }
}

/* Function start: 0x41FCB0 */
void Animation::NextFrame() {
  if (this->smk != 0) {
    SmackNextFrame(this->smk);
  }
}

/* Function start: 0x41FCC0 */
void Animation::GotoFrame(int frame) {
  if (this->smk != 0) {
    if (g_GameConfig_00436970->data[2] == '\x02') {
      SmackSoundOnOff(this->smk, 0);
    }
    SmackGoto(this->smk, frame);
    if (g_GameConfig_00436970->data[2] == '\x02') {
      SmackSoundOnOff(this->smk, 1);
    }
  }
}

/* Function start: 0x41FD20 */
int Animation::Open(char *filename, int param_2, int param_3) {
  if (this->smk != 0) {
    return 1;
  }

  if (g_GameConfig_00436970->data[2] != '\x02') {
    param_2 = param_2 & 0xfff01fff;
  }

  this->smk = SmackOpen(filename, param_2, param_3);
  if (this->smk == 0) {
    ShowError("Animation::Open - Cannot open file %s", filename);
  }
  return 1;
}

/* Function start: 0x41FD80 */
void Animation::VBInit() {
  if (this->vbuffer != 0) {
    ShowError("Animation::VBInit() - Virtual Buffer already defined");
  }

  VBuffer *vbuffer = (VBuffer *)AllocateMemory(0x30);
  try {
    if (vbuffer != 0) {
      HSMACK smk = this->smk;
      vbuffer = vbuffer->CreateAndClean(smk->Width, smk->Height);
    }
  } catch (...) {
  }
  this->vbuffer = vbuffer;
}

/* Function start: 0x41FE20 */
void Animation::FreeVBuffer() {
  if (this->vbuffer != 0) {
    VBuffer *vbuffer = this->vbuffer;
    vbuffer->~VBuffer();
    FreeMemory(vbuffer);
    this->vbuffer = 0;
  }
  this->data = 0;
}

/* Function start: 0x41FE50 */
void Animation::OpenAndConvertToBuffer(char *filename) {
  Open(filename, 0xfe000, -1);
  this->ToBuffer();
}

/* Function start: 0x41FE70 */
void Animation::ToBuffer() {
  if (this->smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }
  this->VBInit();
  this->ToBufferVB(this->vbuffer);
}

/* Function start: 0x41FEA0 */
void Animation::ToBufferVB(VBuffer *buffer) {
  if (this->smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }

  this->smack_handle = GetGameWindowHandle();
  this->smack_buffer = SmackBufferOpen(this->smack_handle, 4, 4, 4, 0, 0);

  if (this->smack_buffer == 0) {
    ShowError("Animation::ToBuffer() - Buffer creation failed");
  }

  if (this->data != 0) {
    ShowError("Animation::ToBuffer() - Virtual Buffer already defined");
  }

  this->data = buffer;
  unsigned int uVar3 = this->smack_buffer->Reversed;
  void *uVar1 = buffer->GetData();
  SmackToBuffer(this->smk, 0, 0, this->smk->Width, this->smk->Height, uVar1,
                uVar3);
}

/* Function start: 0x41FF30 */
void Animation::Play(char *filename, unsigned int flags) {
  PaletteBuffer *palette;

  this->flags = flags;
  this->palette = 0;

  if ((flags & 1) == 0) {
    void *mem = AllocateMemory(8);
    try {
      palette = 0;
      if (mem != 0) {
        palette = CreatePaletteBuffer((PaletteBuffer *)mem);
      }
    } catch (...) {
    }
    palette->CopyEntries(0, 0x100);
  }

  if ((this->flags & 2) == 0) {
    BlankScreen();
  }

  Open(filename, 0xfe000, -1);
  ToBuffer();
  MainLoop();
  CloseSmackerFile();

  if (palette != 0) {
    BlankScreen();
    palette->SetEntries(0, 0x100);
    if (palette != 0) {
      palette->Cleanup();
      FreeMemory(palette);
    }
  }
}

/* Function start: 0x420020 */
void Animation::MainLoop() {
  if (smk == 0) {
    return;
  }

  int frame = 1;
  this->data->SetCurrentVideoMode(this->data->handle);

  if (smk->Frames >= frame) {
    do {
      if (smk->NewPalette != 0) {
        SetPalette(0, 0x100);
      }
      this->DoFrame();
      do {
        if (g_JoystickManager_00436968->CheckInput(1) != 0) {
          goto end_loop;
        }
        if ((flags & 4) == 0) {
          int iVar1 = *(int *)((char *)g_JoystickManager_00436968 + 0x1a0);
          unsigned int uVar3 = 0;
          if (iVar1 != 0) {
            uVar3 = *(unsigned int *)(iVar1 + 8) & 2;
          }
          if (uVar3 != 0 || (*(char *)(iVar1 + 0xc) & 2) == 0) {
            int bVar5 = 0;
            if (DAT_004373bc != 0) {
              bVar5 = WaitForInput() == 0x1b;
            }
            if (!bVar5) {
              goto wait;
            }
          }
          *(unsigned int *)&this->palette |= 1;
          goto end_loop;
        }
      wait:;
      } while (SmackWait(smk) != 0);

      VBuffer *vbuffer = this->data;
      void *piVar2 = FUN_004224f0();
      int iVar1 = *(int *)piVar2 - 1;
      piVar2 = FUN_004224e0();
      vbuffer->CallBlitter5(
          vbuffer->clip_x1, vbuffer->clip_x2, vbuffer->saved_video_mode,
          vbuffer->video_mode_lock_count, 0, *(int *)piVar2 - 1, 0, iVar1);

      if (smk->Frames - 1 <= frame) {
        break;
      }
      frame++;
      NextFrame();
    } while (frame <= smk->Frames);
  }
end_loop:
  this->data->InvalidateVideoMode();
}
