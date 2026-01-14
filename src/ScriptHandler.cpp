#include "ScriptHandler.h"
#include "Message.h"

/* Function start: 0x401100 */
ScriptHandler::~ScriptHandler() {
}

/* Function start: 0x417180 */
void ScriptHandler::CopyCommandData(ScriptHandler* other) {
    if (other != 0) {
        targetAddress = other->targetAddress;
        sourceAddress = other->sourceAddress;
        command = other->command;
        data = other->data;
    }
}

/* Function start: 0x4171B0 */
int ScriptHandler::WriteMessageAddress(Message* msg) {
    if (msg != 0) {
        msg->targetAddress = targetAddress;
    }
    return 0;
}

// NOTE: Functions at 0x417180 and 0x4171B0 are implemented here
// They belong to ScriptHandler based on SC_Timer and SC_OnScreenMessage usage
