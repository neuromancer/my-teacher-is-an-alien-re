#include "ScriptHandler.h"
#include "Message.h"

/* Function start: 0x449D80 */
ScriptHandler::~ScriptHandler() {
}

/* Function start: 0x417180 */ /* DEMO ONLY - no full game match */
void ScriptHandler::CopyCommandData(ScriptHandler* other) {
    if (other != 0) {
        command = other->command;
        data = other->data;
    }
}

/* Function start: 0x4171B0 */ /* DEMO ONLY - no full game match */
int ScriptHandler::WriteMessageAddress(Message* msg) {
    if (msg == 0) {
        return -1;
    }

    msg->targetAddress = targetAddress;
    msg->sourceAddress = sourceAddress;
    msg->command = targetAddress;
    msg->data = sourceAddress;
    msg->priority = 0;

    return 0;
}

// NOTE: Functions at 0x417180 and 0x4171B0 are implemented here
// They belong to ScriptHandler based on SC_Timer and SC_OnScreenMessage usage
