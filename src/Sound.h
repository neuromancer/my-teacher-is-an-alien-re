#ifndef SOUND_H
#define SOUND_H

class Sound {
public:
    void* Init(int param_1, unsigned short param_2, short param_3);
    void AllocateSampleHandles();
    int FindFreeSampleHandle();

private:
    short num_handles;
    int handles[13];
    char pad[0x38 - 2 - sizeof(int) * 13];
    int field_0x38;
};

#endif // SOUND_H
