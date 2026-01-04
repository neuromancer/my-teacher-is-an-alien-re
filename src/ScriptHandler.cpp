#include "ScriptHandler.h"
#include "Message.h"

/* Function start: 0x401100 */
ScriptHandler::~ScriptHandler() {
}

/* Function start: 0x417180 */
void ScriptHandler::CopyCommandData(ScriptHandler* other)
{
    if (other != 0) {
        command = other->command;
        data = other->data;
    }
}

/* Function start: 0x4171B0 */
int ScriptHandler::WriteMessageAddress(Message* msg)
{
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
