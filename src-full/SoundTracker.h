#ifndef SOUNDTRACKER_H
#define SOUNDTRACKER_H

class SoundTracker {
public:
    char field_0;

    SoundTracker(int param);   // 0x412000
    ~SoundTracker();            // 0x4120a0
    void ClearPool();           // 0x412400
    void Init();                // 0x412480
    void Cleanup();             // 0x412730
};

#endif // SOUNDTRACKER_H
