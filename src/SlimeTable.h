#ifndef SLIMETABLE_H
#define SLIMETABLE_H

class SlimeTable {
public:
    int fields[3];
    SlimeTable();
    ~SlimeTable();
    int Play(int);      // 0x425550
    void StopPlaying(); // 0x4255D0
    int IsSamplePlaying(int); // 0x4256D0
    void LoadEntry(int, char*, int);
    void Cleanup();     // 0x4254F0
    void Allocate(int); // 0x4254A0
};

#endif // SLIMETABLE_H
