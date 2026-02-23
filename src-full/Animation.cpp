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
#include <new>

extern "C" {
void *GetGameWindowHandle();
int *GetWindowHeight();
int *GetWindowWidth();
}

// BlankScreen moved to Graphics.cpp (0x419390)
void BlankScreen();

// SetPaletteEntriesAnimation moved to Palette.cpp (0x41EB90)
extern "C" void SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count);

/* Function start: 0x41A9D0 */
Animation::Animation() {
  CleanArray10();
}

/* Function start: 0x41AA60 */
Animation::Animation(char *filename) {
  CleanArray10();
  OpenAndConvertToBuffer(filename);
}

/* Function start: 0x41AAE0 */
void Animation::CleanArray10() {
  memset(&field_4, 0, 40);
}

/* Function start: 0x41AAF0 */
Animation::~Animation() {
  FreeVBuffer();
  CloseSmackerBuffer();
  CloseSmackerFile();
}

/* Function start: 0x41FBE0 */ /* DEMO ONLY - no full game match */
void Animation::CloseSmackerFile() {
  if (smk != 0) {
    SmackClose(smk);
    smk = 0;
  }
}

/* Function start: 0x41ABF0 */
void Animation::CloseSmackerBuffer() {
  if (smack_buffer != 0) {
      SmackBufferClose(smack_buffer);
      smack_buffer = 0;
  }
}

/* Function start: 0x41FC20 */ /* DEMO ONLY - no full game match */
int Animation::SetPalette(unsigned int param_1, unsigned int param_2) {
  if (smk != 0 && smack_buffer != 0) {
    targetBuffer->SetCurrentVideoMode(targetBuffer->handle);
    if (smk->NewPalette != 0) {
      SmackBufferNewPalette(smack_buffer, smk->Palette, 0);
      SmackColorRemap(smk, smack_buffer->Palette,
                      smack_buffer->PalColorsInUse,
                      smack_buffer->Unk43C);
    }
    SetPaletteEntriesAnimation((char *)smk->Palette, param_1, param_2);
    targetBuffer->InvalidateVideoMode();
    return 0;
  }
  return 0;
}

/* Function start: 0x41AD90 */
void Animation::DoFrame() {
  if (smk != 0) {
    SmackDoFrame(smk);
  }
}

/* Function start: 0x41ADA0 */
void Animation::NextFrame() {
  if (smk != 0) {
    SmackNextFrame(smk);
  }
}

/* Function start: 0x41FCC0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41FD20 */ /* DEMO ONLY - no full game match */
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
    return 0;
  }
  return 1;
}

/* Function start: 0x41AEB0 */
void Animation::VBInit() {
  if (vbuffer != 0) {
    ShowError("Animation::VBInit() - Virtual Buffer already defined");
  }

  vbuffer = new VBuffer(smk->Width, smk->Height);
}

/* Function start: 0x41AF50 */
void Animation::FreeVBuffer() {
  if (vbuffer != 0) {
    delete vbuffer;
    vbuffer = 0;
  }
  targetBuffer = 0;
}

/* Function start: 0x41AF80 */
void Animation::OpenAndConvertToBuffer(char *filename) {
  Open(filename, 0xfe000, -1);
  ToBuffer();
}

/* Function start: 0x41AFA0 */
void Animation::ToBuffer() {
  if (smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }
  VBInit();
  ToBufferVB(vbuffer);
}

/* Function start: 0x41AFD0 */ /* ~98% match */
void Animation::ToBufferVB(VBuffer *buffer) {
  if (smk == 0) {
    ShowError("Animation::ToBuffer() - No smk defined");
  }

  windowHandle = (HWND)GetGameWindowHandle();
  smack_buffer = SmackBufferOpen(windowHandle, 4, 4, 4, 0, 0);

  if (smack_buffer == 0) {
    ShowError("Animation::ToBuffer() - Buffer creation failed");
  }

  if (targetBuffer != 0) {
    ShowError("Animation::ToBuffer() - Virtual Buffer already defined");
  }

  targetBuffer = buffer;
  unsigned int uVar3 = *(unsigned char*)smack_buffer;
  void *uVar1 = buffer->GetData();
  SmackToBuffer(smk, 0, 0, smk->Width, smk->Height, uVar1,
                uVar3);
}

/* Function start: 0x41FF30 */ /* DEMO ONLY - no full game match */
void Animation::Play(char *filename, unsigned int flags) {
  Animation::flags = flags;
  Palette *palette = 0;
  Animation::playStatus = 0;

  if ((flags & 1) == 0) {
    palette = new Palette();
    palette->CopyEntries(0, 0x100);
  }

  if ((Animation::flags & 2) == 0) {
    BlankScreen();
  }

  Open(filename, 0xfe000, -1);
  ToBuffer();
  MainLoop();
  CloseSmackerFile();

  if (palette) {
    BlankScreen();
    palette->SetPalette(0, 0x100);
    delete palette;
  }
}

/* Function start: 0x420020 */ /* DEMO ONLY - no full game match */
void Animation::MainLoop() {
  if (!smk) return;

  targetBuffer->SetCurrentVideoMode(targetBuffer->handle);
  int frame = 1;
  int skipFlag = 4;

  if ((int)smk->Frames >= frame) {
    do {
      if (smk->NewPalette) {
        SetPalette(0, 0x100);
      }
      DoFrame();

      while (true) {
        if (g_InputManager_00436968->PollEvents(1)) goto end_loop;

        if (!(flags & skipFlag)) {
          InputState *pMouse = g_InputManager_00436968->pMouse;
          int buttons = 0;
          if (pMouse) buttons = pMouse->buttons & 2;

          if (!buttons) {
            if (pMouse->prevButtons & 2) {
              playStatus |= 1;
              goto end_loop;
            }
          }

          int escaped = 0;
          if (g_WaitForInputValue_004373bc) {
            escaped = (WaitForInput() == 0x1b);
          }
          if (escaped) {
            playStatus |= 1;
            goto end_loop;
          }
        }

        if (!SmackWait(smk)) break;
      }

      VBuffer *vb = targetBuffer;
      vb->CallBlitter5(vb->clip_x1, vb->clip_x2, vb->clip_y1, vb->clip_y2, 0,
                       *GetWindowWidth() - 1, 0, *GetWindowHeight() - 1);

      if ((int)smk->Frames - 1 <= frame) break;
      frame++;
      NextFrame();
    } while ((int)smk->Frames >= frame);
  }

end_loop:
  targetBuffer->InvalidateVideoMode();
}

