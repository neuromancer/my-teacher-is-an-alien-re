#ifndef TARGET_H
#define TARGET_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"
#include "Sample.h"

struct HotspotNode {
    HotspotNode* next;  // 0x00
    HotspotNode* prev;  // 0x04
    int id;             // 0x08
    int reserved;       // 0x0C
    // total size: 0x10 (16 bytes per node in pool allocation)

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
    HotspotNode* AllocateNode(); // 0x443830
};

class TargetMember {
public:
    int val;
    TargetMember() : val(0) {}
    TargetMember(int v) : val(v) {}
    ~TargetMember() {}
};

// Target - Full game layout (extends Sprite at 0xF8)
// Constructor: 0x442350
// Vtable: 0x461948
// Size: 0x178
class Target : public Sprite {
public:
    // Full game Target fields (Sprite ends at 0xF8)
    int active;           // 0xF8 - 0=inactive, 1=active, 3=hit
    int targetFlags;      // 0xFC - bit 0: use cursor position for hit
    char* animFilename;   // 0x100 - animation filename from INIT
    char* identifier;     // 0x104 - identifier string from 'I' label
    int id;               // 0x108
    SlimeDim animRange;   // 0x10C-0x113 (start, end frame)
    SlimeDim hitRange;    // 0x114-0x11B (start, end frame)
    SlimeDim timeRange;   // 0x11C-0x123 (min, max color value)
    IntPair progressRange; // 0x124-0x12B
    IntPair scoreWeight;  // 0x12C-0x133 - score index and weight
    IntPair hitMissPoints; // 0x134-0x13B - hit points and miss penalty
    IntPair combatBonus;  // 0x13C-0x143 - bonus1 and bonus2
    TargetMember combatBonus2; // 0x144
    TargetMember missBonus; // 0x148
    HotspotListData* hotspotList; // 0x14C
    Sample* stopSound;    // 0x150 - sound stopped on hit
    Sample* progressSound; // 0x154 - sound played on progress
    Sample* hitSound;     // 0x158 - sound played on hit
    Sample* sound3;       // 0x15C - extra sound
    IntPair animParam;    // 0x160-0x167 - from 'A' label
    IntPair hitOffset;    // 0x168-0x16F - hit offset from 'O' label
    int pendingAction;    // 0x170 - pending action (0=none, 1=miss, 3=hit)
    int field_174;        // 0x174

    Target();                                   // 0x442350
    virtual ~Target();                          // 0x442A70
    virtual int LBLParse(char* line);           // 0x442BD0
    virtual void OnProcessStart();              // 0x442B20
    virtual void OnProcessEnd();                // 0x442BD0 (alias)

    void Spawn();                               // 0x442AB0
    void Activate();                            // 0x442AB0
    void Deactivate();                          // 0x4427C0
    int CheckTimeInRange();
    int CheckTimeInRangeParam(int* coords);
    int AdvanceHotspot();                       // 0x4428E0
    void UpdateProgress(int delta);             // 0x442940
    int Update();                               // 0x4429A0

    void Init(char* line);
    void ParseSound(char* line);
};

// TargetList - Full game layout (Parser at 0x90)
// Constructor: 0x443280
class TargetList : public Parser {
public:
    int count;                     // 0x90
    Target* targets[70];           // 0x94-0x1AB
    Target* currentTarget;         // 0x1AC
    int field_1B0;                 // 0x1B0
    int field_1B4;                 // 0x1B4
    HashTable* hashTable;          // 0x1B8
    Sample* defaultStopSound;      // 0x1BC
    Sample* defaultProgressSound;  // 0x1C0
    Sample* defaultHitSound;       // 0x1C4
    Sample* defaultSound;          // 0x1C8

    TargetList();
    virtual ~TargetList();
    virtual int LBLParse(char* line);
    virtual void OnProcessEnd();

    Target* ProcessTargets();
};

#endif // TARGET_H
