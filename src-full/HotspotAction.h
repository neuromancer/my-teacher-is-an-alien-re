#ifndef HOTSPOTACTION_H
#define HOTSPOTACTION_H

#include "Parser.h"
#include "LinkedList.h"

class MMPlayer;

class HotspotAction : public Parser {
public:
    HotspotAction(int id);
    ~HotspotAction();
    int LBLParse(char* line);

    void Reset();
    int HandleMessage(int* msg);
    int Update(int param);
    int CheckConditions();
    void ProcessQueue100();
    void ProcessQueue104();
    void ProcessQueueFC();

    int field_90;           // 0x90
    int field_94;           // 0x94
    int field_98;           // 0x98
    int hotspotId;          // 0x9C
    int state;              // 0xA0
    int field_A4;           // 0xA4
    int field_A8;           // 0xA8
    int field_AC;           // 0xAC
    int field_B0;           // 0xB0
    char field_B4[64];      // 0xB4
    MMPlayer* field_F4;           // 0xF4
    MMPlayer* field_F8;           // 0xF8
    LinkedList* queueFC;    // 0xFC
    LinkedList* queue100;   // 0x100
    LinkedList* queue104;   // 0x104
    LinkedList* queue108;   // 0x108
    int field_10C;          // 0x10C
};

#endif // HOTSPOTACTION_H
