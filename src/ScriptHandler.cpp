#include "Handler.h"
#include "Message.h"

/* Function start: 0x417180 */
void Handler::Init(SC_Message* msg) {
    Handler* other = (Handler*)msg;
    if (other != 0) {
        command = other->command;
        data = other->data;
    }
}

/* Function start: 0x4171B0 */
int Handler::AddMessage(SC_Message* msg) {
    Message* m = (Message*)msg;
    if (m == 0) {
        return -1;
    }

    m->targetAddress = targetAddress;
    m->sourceAddress = sourceAddress;
    m->command = targetAddress;
    m->data = sourceAddress;
    m->priority = 0;

    return 0;
}
