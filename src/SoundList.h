#ifndef SOUNDLIST_H
#define SOUNDLIST_H

class SoundList {
public:
    SoundList(int count);
    ~SoundList();
    void StopAll();

private:
    void** m_sounds;
    int m_count;
    void** m_field8;
    int m_fieldc;
};

#endif // SOUNDLIST_H
