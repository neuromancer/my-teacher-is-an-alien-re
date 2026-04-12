#ifndef SOUNDTRACKER_H
#define SOUNDTRACKER_H

class SoundTracker {
public:
    char field_0;

    SoundTracker(int param);   // 0x412000
    ~SoundTracker();            // 0x4120a0
    __int64 Lookup(char* filename, char* key);   // 0x412130
    void Store(char* filename, char* key, int posLo, int posHi);  // 0x412210
    void ClearPool();           // 0x412400
    void Init();                // 0x412480
    void Cleanup();             // 0x412730
};

#endif // SOUNDTRACKER_H
