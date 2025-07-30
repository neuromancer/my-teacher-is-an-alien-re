#ifndef SPRITE_H
#define SPRITE_H

#include "BaseObject.h"

class VBuffer;
class Animation;
struct SmackStruct;

class Sprite : public BaseObject {
public:
    // placeholder fields based on offsets
    SmackStruct* smack_struct; // 0xc - This is likely wrong, VBuffer has a field here.
    char pad_0x10[0x24 - 0x10];
    void* smacker_buffer; // 0x24
    char pad_0x28[0x88 - 0x28];
    int* field136_0x88; // 0x88
    int flags; // 0x8c
    int field138_0x90; // 0x90
    int field139_0x94; // 0x94
    int field140_0x98;
    char pad_0x9c[0xa0 - 0x9c];
    int field142_0xa0;
    int field_0xa4; // 0xa4
    int field_0xa8; // 0xa8
    int field_0xac; // 0xac
    char pad_0xb0[0xb4 - 0xb0];
    const char* filename; // 0xb4
    char pad_0xb8[0xd0 - 0xb8];
    Animation* animation_data; // 0xd0
    char pad_0xd4[4];
    VBuffer* vbuffer;

    void CheckRanges1();
    void CloseSmackerBuffer();
    void CloseSmackerFile();
    unsigned char Do(int x, int y, int param_3, int param_4);
    void SetState(int state);
    void SetRanges(int);
    Sprite(char* filename);
    ~Sprite();
    void ToBuffer();
    void MainLoop();
    void VBInit();
    void ToBuffer2(void*);
    void DoFrame();
    void SetRange(int, int, int);
    void SetLogic(int, int);
    void AllocateLogic(int);
    void CleanArray10();
    void Free();
};

#endif // SPRITE_H
