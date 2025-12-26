#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"
#include <smack.h>

class Animation {
public:
  int *field_0x4;
  VBuffer *vbuffer; // 0x8
  HSMACK smk;       // 0xc
  char pad2[0x18 - 0xc - 4];
  VBuffer *data;      // 0x18
  unsigned int flags; // 0x1c
  void *palette;      // 0x20
  void *smack_buffer; // 0x24
  void *smack_handle; // 0x28

  Animation();
  Animation(char *filename);
  ~Animation();
  void AnimationInit();
  int Open(char *, int, int);
  void ToBuffer();
  void ToBuffer(VBuffer *buffer);
  void OpenAndConvertToBuffer(char *filename);
  void Play(char *filename, unsigned int flags);
  void MainLoop();
  void FreeVBuffer();
  void VBInit();
  void GotoFrame(int frame);
  void NextFrame();
  void SetPalette(unsigned int, unsigned int);
  void CloseSmackerBuffer();
  void CloseSmackerFile();
  void CleanArray10();
  void Delete(unsigned char);
  static void DoFrame(Animation *);
};

#endif // ANIMATION_H
