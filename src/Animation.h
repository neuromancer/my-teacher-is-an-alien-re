#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"
#include <smack.h>

class Animation {
public:
  int *field_0x4;     // 0x4
  VBuffer *vbuffer;   // 0x8
  HSMACK smk;         // 0xc
  int field_0x10;     // 0x10
  int field_0x14;     // 0x14
  VBuffer *data;      // 0x18
  unsigned int flags; // 0x1c
  void *palette;      // 0x20
  void *smack_buffer; // 0x24
  void *smack_handle; // 0x28

  Animation() {}
  Animation *Init();
  Animation *InitWithFilename(char *filename);
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
  void SetPalette(unsigned int, unsigned int);
  void CloseSmackerBuffer();
  void CloseSmackerFile();
  
  virtual void Delete(unsigned char);
  void DoFrame();

private:
  void CleanArray10();
};

#endif // ANIMATION_H
