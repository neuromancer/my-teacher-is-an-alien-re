#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"
#include <smack.h>

// Small class with destructor to trigger SEH generation
// Destructor maps to Cleanup_00405770 (empty function)
class UnknownClassAnimation {
public:
  int field_0;
  int field_4;
  UnknownClassAnimation() {
    field_0 = 0;
    field_4 = 0;
  }
  ~UnknownClassAnimation();
};

class Animation {
public:
  int *field_0x4;     // 0x4
  VBuffer *vbuffer;   // 0x8
  HSMACK smk;         // 0xc
  UnknownClassAnimation unknown; // 0x10-0x17 (8 bytes)
  VBuffer *data;      // 0x18
  unsigned int flags; // 0x1c
  void *palette;      // 0x20
  SmackBuf *smack_buffer; // 0x24
  void *smack_handle; // 0x28

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
