#ifndef SLIMETABLE_H
#define SLIMETABLE_H

class SlimeTable {
    int fields[3];
public:
    SlimeTable();
    ~SlimeTable();
    void Init(int);
    void LoadEntry(int, char*, int);
    void Cleanup();     // 0x4254F0
    void Allocate(int); // 0x4254A0
};

#endif // SLIMETABLE_H
