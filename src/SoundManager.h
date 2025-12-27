#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "Queue.h"
#include "Timer.h"

struct SoundCommand;

class SoundManager : public Queue {
public:
    char pad[0x74];
    Timer timer;
    int m_mode;
    void* m_list1;
    Queue* m_commandQueue;
    void* m_list3;

    void PlayAnimationSound(void* data, int priority, int x, int y, int mode, int scale1, int scale2);
    void ShowSubtitle(char* text, int x, int y, int duration);
    void QueueCommand(SoundCommand* cmd);
};

extern SoundManager* g_SoundManager;

#endif
