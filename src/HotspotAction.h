#ifndef HOTSPOTACTION_H
#define HOTSPOTACTION_H

#include "Parser.h"
#include "Queue.h"

class SpriteAction;

class MMPlayer;

class HotspotAction : public Parser {
public:
    HotspotAction(int id);
    ~HotspotAction();
    int LBLParse(char* line);

    void Reset();
    int HandleMessage(SpriteAction* msg);
    int Update(int param);
    int CheckConditions();
    void ProcessQueue100();
    void ProcessQueue104();
    void ProcessQueueFC();

    int parentHandlerId;    // 0x90 - parent handler type (set from Hotspot)
    int parentModuleParam;  // 0x94 - parent module param (set from Hotspot)
    int field_98;           // 0x98
    int hotspotId;          // 0x9C - HANDLE value
    int state;              // 0xA0 - current state (0=idle, 2=playing correct, 3=playing incorrect)
    int field_A4;           // 0xA4
    int currentSpriteId;    // 0xA8 - current sprite action value from message
    int checkObjectId;      // 0xAC - CHECKOBJECT id
    int rolloverStateIdx;   // 0xB0 - ROLLOVER gamestate index
    char rolloverText[64];  // 0xB4 - ROLLOVER tooltip text
    MMPlayer* correctPlayer;      // 0xF4 - CORRECT multimedia player
    MMPlayer* incorrectPlayer;    // 0xF8 - INCORRECT multimedia player
    Queue* actionsQueue;    // 0xFC - PREMSG/INC_ACTIONS queue
    Queue* messagesQueue;   // 0x100 - MESSAGE/SWITCHFOCUS/etc. queue
    Queue* incorrectQueue;  // 0x104 - INCORRECTMESSAGE queue
    Queue* conditionsQueue; // 0x108 - CHECKMSG/CHECKAVAILIABLE/CHECKGAMESTATE queue
    int field_10C;          // 0x10C
};

#endif // HOTSPOTACTION_H
