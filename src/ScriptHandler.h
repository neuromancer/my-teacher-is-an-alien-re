#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include "Parser.h"
#include <string.h>

struct Message;

// ScriptHandler - Base class for script/message handlers
// Extends Parser and adds message command/data fields at 0x88-0x94
// Size: 0x98 bytes (includes Parser)
class ScriptHandler : public Parser {
public:
    ScriptHandler() {
        memset(&targetAddress, 0, 24);
    }
    ~ScriptHandler() {}
    void CopyCommandData(ScriptHandler* other);
    int WriteMessageAddress(Message* msg);

    int targetAddress;  // 0x88 - handler address identifier
    int sourceAddress;  // 0x8c - source address (from)
    int command;        // 0x90 - message command/type
    int data;           // 0x94 - message data
};

#endif // SCRIPTHANDLER_H
