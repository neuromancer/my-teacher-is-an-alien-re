#include "ScriptHandler.h"
#include "Message.h"

/* Function start: 0x401100 */
ScriptHandler::~ScriptHandler() {
}

/* Function start: 0x417180 */
void ScriptHandler::CopyCommandData(ScriptHandler* other)
{
    if (other != 0) {
        this->command = other->command;
        this->data = other->data;
    }
}

/* Function start: 0x4171B0 */
int ScriptHandler::WriteMessageAddress(Message* msg)
{
    if (msg == 0) {
        return -1;
    }
    msg->targetAddress = this->targetAddress;
    msg->sourceAddress = this->sourceAddress;
    msg->command = this->targetAddress;
    msg->data = this->sourceAddress;
    msg->priority = 0;
    return 0;
}
