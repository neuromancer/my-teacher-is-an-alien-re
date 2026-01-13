#include "globals.h"
#include "Animation.h"
#include "Palette.h"
#include "PaletteUtils.h"
#include "VBuffer.h"
#include "string.h"
#include <smack.h>
#include "GameConfig.h"
#include "InputManager.h"

#include "Memory.h"
#include <windows.h>

extern "C" {
void *GetGameWindowHandle();
int *GetWindowHeight();
int *GetWindowWidth();
}

/* Function start: 0x419390 */
void BlankScreen() {
  if (g_WorkBuffer_00436974 != 0) {
    g_WorkBuffer_00436974->ClearScreen(0);
    VBuffer *vbuffer = g_WorkBuffer_00436974;
    vbuffer->CallBlitter5(
        vbuffer->clip_x1, vbuffer->clip_x2, vbuffer->clip_y1,
        vbuffer->clip_y2, 0, *GetWindowWidth() - 1, 0,
        *GetWindowHeight() - 1);
  }
}

extern "C" {

/* Function start: 0x41EB90 */
void __cdecl SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count) {
  SetPaletteEntries_(start, count, (unsigned char *)palette + start * 3);
}
}

/* Function start: 0x41FA50 */
Animation::Animation() {
  CleanArray10();
}

/* Function start: 0x41FAC0 */
void Animation::Delete(unsigned char param_1) {
  delete this;
  if ((param_1 & 1) != 0) {
    FreeMemory(this);
  }
}

/* Function start: 0x41FAE0 */
Animation::Animation(char *filename) {
  //int *p = &unknown.field_0;
  //*p = 0;
  //p[1] = 0;
  //try {
    CleanArray10();
    OpenAndConvertToBuffer(filename);
  //} catch (...) {
  //}
  //return this;
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
  FreeVBuffer();
  CloseSmackerBuffer();
  CloseSmackerFile();
}

/* Function start: 0x41FBE0 */
void Animation::CloseSmackerFile() {
  if (smk != 0) {
    SmackClose(smk);
    smk = 0;
  }
}

/* Function start: 0x41FC00 */
void Animation::CloseSmackerBuffer() {
  if (smack_buffer != 0) {
      SmackBufferClose(smack_buffer);
      smack_buffer = 0;
  }
}

/* Function start: 0x41FC20 */
int Animation::SetPalette(unsigned int param_1, unsigned int param_2) {
  if (smk != 0 && smack_buffer != 0) {
    data->SetCurrentVideoMode(data->handle);
    if (smk->NewPalette != 0) {
      SmackBufferNewPalette(smack_buffer, smk->Palette, 0);
      SmackColorRemap(smk, smack_buffer->Palette,
                      smack_buffer->PalColorsInUse,
                      smack_buffer->Unk43C);
    }
    SetPaletteEntriesAnimation((char *)smk->Palette, param_1, param_2);
    data->InvalidateVideoMode();
    return 0;
  }
  return 0;
}

/* Function start: 0x41FCA0 */
void Animation::DoFrame() {
  if (smk != 0) {
    SmackDoFrame(smk);
  }
}

/* Function start: 0x41FCB0 */
void Animation::NextFrame() {
  if (smk != 0) {
    SmackNextFrame(smk);
  }
}

/* Function start: 0x41FCC0 */
void Animation::GotoFrame(int frame) {
  if (smk != 0) {
    if (g_GameConfig_00436970->data.rawData[2] == '\x02') {
      SmackSoundOnOff(smk, 0);
    }
    SmackGoto(smk, frame);
    if (g_GameConfig_00436970->data.rawData[2] == '\x02') {
      SmackSoundOnOff(smk, 1);
    }
  }
}

/* Function start: 0x41FD20 */
int Animation::Open(char *filename, int param_2, int param_3) {
  if (smk != 0) {
    return 1;
  }

  if (g_GameConfig_00436970->data.rawData[2] != '\x02') {
    param_2 = param_2 & 0xfff01fff;
  }

  smk = SmackOpen(filename, param_2, param_3);
  if (smk == 0) {
    ShowError("Animation::Open - Cannot open file %s", filename);
  }
  return 1;
}

/* Function start: 0x41FD80 */
void Animation::VBInit() {
  if (vbuffer != 0) {
    ShowError("Animation::VBInit() - Virtual Buffer already defined");
  }

  /*VBuffer *vbuffer = (VBuffer *)AllocateMemory(0x30);
  try {
    if (vbuffer != 0) {
      HSMACK smk = smk;
      vbuffer = vbuffer->CreateAndClean(smk->Width, smk->Height);
    }
  } catch (...) {
  }*/
  vbuffer = new VBuffer(smk->Width, smk->Height);
}

/* Function start: 0x41FE20 */
void Animation::FreeVBuffer() {
  delete vbuffer;
  vbuffer = 0;
  data = 0;
}

/* Function start: 0x41FE50 */
void Animation::OpenAndConvertToBuffer(char *filename) {
  Open(filename, 0xfe000, -1);
  ToBuffer();
}

/* Function start: 0x41FE70 */
void Animation::ToBuffer() {
  if (smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }
  VBInit();
  ToBufferVB(vbuffer);
}

/* Function start: 0x41FEA0 */
void Animation::ToBufferVB(VBuffer *buffer) {
  if (smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }

  smack_handle = GetGameWindowHandle();
  smack_buffer = SmackBufferOpen(smack_handle, 4, 4, 4, 0, 0);

  if (smack_buffer == 0) {
    ShowError("Animation::ToBuffer() - Buffer creation failed");
  }

  if (data != 0) {
    ShowError("Animation::ToBuffer() - Virtual Buffer already defined");
  }

  data = buffer;
  unsigned int uVar3 = *(unsigned char*)smack_buffer;
  void *uVar1 = buffer->GetData();
  SmackToBuffer(smk, 0, 0, smk->Width, smk->Height, uVar1,
                uVar3);
}

/* Function start: 0x41FF30 */
void Animation::Play(char *filename, unsigned int flags) {
  PaletteBuffer *palette;

  Animation::flags = flags;
  Animation::palette = 0;

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

  if ((flags & 2) == 0) {
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
  data->SetCurrentVideoMode(data->handle);

  if (smk->Frames >= frame) {
    do {
      if (smk->NewPalette != 0) {
        SetPalette(0, 0x100);
      }
      DoFrame();
      do {
        if (g_InputManager_00436968->PollEvents(1) != 0) {
          goto end_loop;
        }
        if ((flags & 4) == 0) {
          InputState* pMouse = g_InputManager_00436968->pMouse;
          unsigned int uVar3 = 0;
          if (pMouse != 0) {
            uVar3 = pMouse->buttons & 2; // offset 8
          }
          if (uVar3 != 0 || (pMouse->prevButtons & 2) == 0) { // offset 0xc equals prevButtons
            int bVar5 = 0;
            if (DAT_004373bc != 0) {
              bVar5 = WaitForInput() == 0x1b;
            }
            if (!bVar5) {
              goto wait;
            }
          }
          *(unsigned int *)((char*)this + 0x20) |= 1;
          goto end_loop;
        }
      wait:;
      } while (SmackWait(smk) != 0);

      VBuffer *vbuffer = data;
      int iVar1 = *GetWindowHeight() - 1;
      int iVar2 = *GetWindowWidth() - 1;
      vbuffer->CallBlitter5(
          vbuffer->clip_x1, vbuffer->clip_x2, vbuffer->clip_y1,
          vbuffer->clip_y2, 0, iVar2, 0, iVar1);

      if (smk->Frames - 1 <= frame) {
        break;
      }
      frame++;
      NextFrame();
    } while (frame <= smk->Frames);
  }
end_loop:
  data->InvalidateVideoMode();
}
