#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"
#include <smack.h>
#include <windows.h>

class Animation {
public:
  int field_4;              // 0x4 - unused/reserved
  VBuffer *vbuffer;         // 0x8 - internal frame buffer
  HSMACK smk;               // 0xc - smacker file handle
  int field_10;             // 0x10 - unused/reserved
  int field_14;             // 0x14 - unused/reserved
  VBuffer *targetBuffer;    // 0x18 - render target buffer
  unsigned int flags;       // 0x1c - playback flags
  unsigned int playStatus;  // 0x20 - status (bit 0: user cancelled)
  SmackBuf *smack_buffer;   // 0x24 - smacker buffer
  HWND windowHandle;        // 0x28 - game window handle

  Animation();
  Animation(char *filename);
  ~Animation();
  void AnimationInit();
  int Open(char *, int, int);
  void ToBuffer();
  void ToBufferVB(VBuffer *buffer);
  void OpenAndConvertToBuffer(char *filename);
  void Play(char *filename, unsigned int flags);
  void MainLoop();
  void FreeVBuffer();
  void VBInit();
  void GotoFrame(int frame);
  void NextFrame();
  int SetPalette(unsigned int, unsigned int);
  void CloseSmackerBuffer();
  void CloseSmackerFile();
  
  virtual void Delete(unsigned char);
  void DoFrame();

private:
  void CleanArray10();
};

void BlankScreen();

#endif // ANIMATION_H
