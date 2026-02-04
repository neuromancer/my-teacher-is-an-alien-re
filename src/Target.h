#ifndef TARGET_H
#define TARGET_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"
#include "Sample.h"

struct HotspotNode {
    HotspotNode* next;  // 0x0
    HotspotNode* prev;  // 0x4
    int id;             // 0x8

    HotspotNode() : next(0), prev(0), id(0) {}
};

class HotspotListData {
public:
    HotspotNode* first;    // 0x0
    HotspotNode* last;     // 0x4
    int count;             // 0x8
    HotspotNode* freeList; // 0xc
    void* nodePool;        // 0x10
    int growthRate;        // 0x14
    HotspotNode* processingHead; // 0x18
    int currentId;         // 0x1c

    HotspotListData()
    {
        count = 0;
        freeList = 0;
        last = 0;
        first = 0;
        nodePool = 0;
        growthRate = 10;
        processingHead = 0;
        currentId = -1;
    }
    ~HotspotListData();
};

class TargetMember {
public:
    int val;
    TargetMember() : val(0) {}
    TargetMember(int v) : val(v) {}
    ~TargetMember() {}
};

class Target : public Sprite {
public:
    // Target-specific fields (0xD8 - 0x157)
    // Sprite ends at 0xD8, so Target fields start here
    int active;           // 0xd8 - 0=inactive, 1=active, 3=hit
    int targetFlags;      // 0xdc - bit 0: use hit offset position
    char* animFilename;   // 0xe0 - animation filename from INIT
    char* identifier;     // 0xe4 - identifier string from 'I' label
    int id;               // 0xe8
    Range animRange;      // 0xec
    Range hitRange;       // 0xf4
    Range timeRange;      // 0xfc
    IntPair progressRange;  // 0x104
    IntPair scoreWeight;  // 0x10c - score index (start) and weight (end)
    IntPair hitMissPoints; // 0x114 - hit points (start) and miss points (end)
    IntPair combatBonus;  // 0x11c - combatBonus1 (start), field_0x120 (end)
    TargetMember combatBonus2; // 0x124 - added to combat engine m_combatBonus2
    TargetMember field_0x128;  // 0x128
    HotspotListData* hotspotList; // 0x12c - hotspot list structure
    Sample* stopSound;    // 0x130 - sound stopped on hit
    Sample* progressSound; // 0x134 - sound played on progress
    Sample* hitSound;     // 0x138 - sound played on hit
    Sample* sound3;       // 0x13c - sound 3
    IntPair animParam;    // 0x140 - from 'A' label
    IntPair hitOffset;    // 0x148 - hit offset (from 'O' label)
    int pendingAction;    // 0x150 - pending action (0=none, 1=miss, 3=hit)
    int field_0x154;      // 0x154

    Target(); // 0x413DC0
    virtual ~Target(); // 0x413F10
    virtual int LBLParse(char* line); // 0x414930
    virtual void OnProcessStart(); // 0x414730
    virtual void OnProcessEnd(); // 0x4147F0

    void Spawn(); // 0x414060 - spawn target with palette and sound
    void Activate(); // 0x4140B0 - activate target (sets active=1)
    void Deactivate(); // 0x414230
    int CheckTimeInRange(); // 0x4142C0
    int CheckTimeInRangeParam(int* coords); // 0x414310
    int AdvanceHotspot(); // 0x414350
    void UpdateProgress(int delta); // 0x4143B0
    int Update(); // 0x414410

    void Init(char* line); // 0x4145E0
    void ParseSound(char* line); // 0x414690
};

class TargetList : public Parser {
public:
    int count; // 0x88
    Target* targets[70]; // 0x8c - 0x1a3
    Target* currentTarget; // 0x1a4 (was targets[70])
    int field_0x1a8;  // 0x1a8
    int field_0x1ac;  // 0x1ac
    HashTable* hashTable; // 0x1B0
    int field_0x1b4;  // 0x1b4
    int field_0x1b8;  // 0x1b8
    int field_0x1bc;  // 0x1bc
    void* field_0x1c0; // 0x1c0 - some object pointer

    TargetList(); // 0x414D80
    virtual ~TargetList(); // 0x414DF0
    virtual int LBLParse(char* line); // 0x414FD0
    virtual void OnProcessEnd(); // 0x414F30

    Target* ProcessTargets(); // 0x4150F0
};

#endif // TARGET_H
