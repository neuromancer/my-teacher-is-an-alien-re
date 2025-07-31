#ifndef ANIMATION_H
#define ANIMATION_H

struct SmkObject {
    char pad[0x374];
    int current_frame;
};

struct AnimationData {
    char pad[0x18];
    int y_offset;
    int field_0x1c;
};

class Animation {
public:
    char pad[0xc];
    SmkObject* smk; // 0xc
    char pad2[0x18 - 0xc - 4];
    AnimationData* data; // 0x18
    char pad3[0x2c - 0x18 - 4];

    void AnimationInit();
    void Open(char*, int, int);
    static void DoFrame(Animation*);
};

#endif // ANIMATION_H
