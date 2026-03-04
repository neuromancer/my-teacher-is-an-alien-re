#include "globals.h"
#include "Animation.h"
#include "Palette.h"
#include "PaletteUtils.h"
#include "VBuffer.h"
#include "string.h"
#include <smack.h>
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

// SetPaletteEntriesAnimation moved to Palette.cpp (0x41DE40)
extern "C" void SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count);

// Smack function pointer table (loaded from SMACKW32.DLL at runtime)
extern "C" {
    extern void (__stdcall *DAT_00476538)(int, void*, unsigned int, unsigned int); // SmackColorRemap
    extern unsigned int (__stdcall *DAT_00476540)(int);                            // SmackDoFrame
    extern void (__stdcall *DAT_00476544)(int);                                    // SmackNextFrame
    extern unsigned int (__stdcall *DAT_00476548)(int, unsigned int);              // SmackGoto
    extern void (__stdcall *DAT_0047654c)(int);                                    // SmackClose
    extern void (__stdcall *DAT_00476550)(void*, void*, unsigned int);             // SmackBufferNewPalette
    extern int (__stdcall *DAT_00476554)(const char*, unsigned int, unsigned int);  // SmackOpen
    extern int (__stdcall *DAT_0047655c)(int);                                     // SmackWait
    extern unsigned int (__stdcall *DAT_00476570)(int, unsigned int);              // SmackSoundOnOff
}

// Full game globals (extern "C" linkage)
extern "C" {
    extern int DAT_0046a6ec;        // Timer manager pointer
    extern void* DAT_0046aa10;      // GameConfig pointer
    extern void* DAT_0046aa30;      // Game state pointer
    extern int DAT_0046ac04;        // WaitForInput flag
    extern char DAT_00472c70[];     // Animation filename buffer
    extern char DAT_00472cb0[];     // Animation filename buffer 2
}

// C++ linkage globals
extern void* DAT_0046aa08;          // InputManager pointer

// Animation cache (g_GameLoopHelper at 0x46a6f0)
class GameLoopHelper {
public:
    void AddAnimation(char* name, int handle);
    void RemoveAnimation(int handle);
};
extern GameLoopHelper* g_GameLoopHelper;

// Font display functions
extern "C" void SetFontPosition(int, int);
extern "C" void SetFontColor(int);
extern "C" void DrawFontText(char*, int);

// WriteToLog (C++ linkage to match stubs.cpp)
void __cdecl FUN_00425d70(char*, ...);

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

/* Function start: 0x41AB60 */
void Animation::CloseSmackerFile() {
  if (smk != 0) {
    if (DAT_0046a6ec != 0) {
      if (*(int*)(DAT_0046a6ec + 0x24) == (int)smk) {
        FUN_00425d70("Animation Close - restoring framerate to %dms",
                     *(int*)(DAT_0046a6ec + 0x20));
        *(int*)(DAT_0046a6ec + 0x24) = 0;
        *(int*)(DAT_0046a6ec + 0x1c) = *(int*)(DAT_0046a6ec + 0x20);
      }
    }
    if (*(char*)((int)DAT_0046aa10 + 0x46) == '\x02') {
      DAT_00476570((int)smk, 0);
    }
    if (g_GameLoopHelper != 0) {
      g_GameLoopHelper->RemoveAnimation((int)smk);
    }
    DAT_0047654c((int)smk);
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

/* Function start: 0x41AC10 */
int Animation::SetPalette(unsigned int param_1, unsigned int param_2) {
  int result;

  if (smk->NewPalette != 0) {
    DAT_00476550(smack_buffer, &smk->Palette, 0);
    if (smack_buffer->PalColorsInUse < 0x100) {
      DAT_00476538((int)smk, &smack_buffer->Palette, smack_buffer->PalColorsInUse,
                   smack_buffer->Unk43C);
    }
  }

  Palette pal;
  pal.CopyEntries(0, 0x100);
  result = pal.IsSimilar(&smk->Palette, 0, 0x100);
  if (result) {
    return 1;
  }
  return 0;
}

/* Function start: 0x41ACF0 */
int Animation::ApplyPalette(unsigned int start, unsigned int count) {
  targetBuffer->SetCurrentVideoMode(targetBuffer->handle);
  SetPaletteEntriesAnimation((char*)&smk->Palette, start, count);
  targetBuffer->InvalidateVideoMode();
  return count;
}

/* Function start: 0x41AD30 */
int Animation::UpdatePalette(unsigned int start, unsigned int count) {
  if (smk->NewPalette != 0) {
    DAT_00476550(smack_buffer, &smk->Palette, 0);
    if (smack_buffer->PalColorsInUse < 0x100) {
      DAT_00476538((int)smk, &smack_buffer->Palette, smack_buffer->PalColorsInUse,
                   smack_buffer->Unk43C);
    }
  }
  ApplyPalette(start, count);
  return count;
}

/* Function start: 0x41AD90 */
void Animation::DoFrame() {
  if (smk != 0) {
    DAT_00476540((int)smk);
  }
}

/* Function start: 0x41ADA0 */
void Animation::NextFrame() {
  if (smk != 0) {
    DAT_00476544((int)smk);
  }
}

/* Function start: 0x41ADB0 */
void Animation::GotoFrame(int frame) {
  if (smk != 0) {
    if (*(char*)((int)DAT_0046aa10 + 0x46) == '\x02') {
      DAT_00476570((int)smk, 0);
    }
    DAT_00476548((int)smk, frame);
    if (*(char*)((int)DAT_0046aa10 + 0x46) == '\x02') {
      DAT_00476570((int)smk, 1);
    }
  }
}

/* Function start: 0x41AE10 */
int Animation::Open(char *filename, int param_2, int param_3) {
  if (smk != 0) {
    return 1;
  }

  if (*(char*)((int)DAT_0046aa10 + 0x46) != '\x02') {
    param_2 = param_2 & 0xfff01fff;
  }
  param_2 = param_2 | 0x400;

  smk = (HSMACK)DAT_00476554(filename, param_2, param_3);
  if (smk == 0) {
    ShowError("Animation::Open - Cannot open file '%s'", filename);
    return 0;
  }

  if (g_GameLoopHelper != 0) {
    g_GameLoopHelper->AddAnimation(filename, (int)smk);
  }
  strcpy(DAT_00472c70, filename);

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

/* Function start: 0x41AFD0 */
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

/* Function start: 0x41B060 */
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

  if (DAT_0046aa30 != 0) {
    int* gs = (int*)DAT_0046aa30;
    if (gs[0x98/4] - 1 < 4) {
      ShowError("Invalid gamestate %d", gs[0x98/4] - 1);
    }
    int* stateArray = (int*)gs[0x90/4];
    if (stateArray[0x10/4] != 0) {
      Animation::flags |= 0x100;
      strcpy(DAT_00472cb0, filename);
    }
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

/* Function start: 0x41B1C0 */
void Animation::MainLoop() {
  int frame;
  int escaped;

  if (!smk) return;

  targetBuffer->SetCurrentVideoMode(targetBuffer->handle);
  frame = 1;

  if (smk->Frames >= (unsigned int)frame) {
    do {
      if (smk->NewPalette != 0) {
        UpdatePalette(0, 0x100);
      }
      DoFrame();

      if (flags & 0x100) {
        SetFontColor(0xfa);
        SetFontPosition(0x14, 0x14);
        DrawFontText(DAT_00472cb0, strlen(DAT_00472cb0));
      }

      while (1) {
        if (((InputManager*)DAT_0046aa08)->PollEvents(1)) goto end_loop;

        if (!(flags & 4)) {
          InputState *pMouse = ((InputManager*)DAT_0046aa08)->pMouse;
          int hasButton = 0;
          if (pMouse != 0) {
            hasButton = (pMouse->ext2 >= 2) ? 1 : 0;
          }

          if (hasButton) {
            playStatus |= 1;
            goto end_loop;
          }

          escaped = 0;
          if (DAT_0046ac04 != 0) {
            escaped = (WaitForInput() == 0x1b);
          }
          if (escaped) {
            playStatus |= 1;
            goto end_loop;
          }
        }

        if (!DAT_0047655c((int)smk)) break;
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
