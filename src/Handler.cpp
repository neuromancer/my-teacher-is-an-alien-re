#include "Handler.h"
#include "SC_Question.h"
#include <string.h>

// Handler base class constructor
// Most handlers override this entirely and don't call it directly
Handler::Handler() {
    // Base Handler constructor - zeroing of fields 0x88-0x9F is done in derived classes
}

// Handler base class destructor
Handler::~Handler() {
}

// Default Init implementation - copies command data from message
// Corresponds to FUN_00417180
/* Function start: 0x417180 */
void Handler::CopyCommandData(SC_Message* msg) {
    if (msg != 0) {
        field_90 = msg->command;
        field_94 = msg->data;
    }
}

// Default WriteMessageAddress implementation
// Corresponds to FUN_004171B0
/* Function start: 0x4171B0 */
int Handler::WriteMessageAddress(SC_Message* msg) {
    if (msg == 0) {
        return -1;
    }
    msg->targetAddress = handlerId;
    msg->sourceAddress = field_8C;
    msg->command = handlerId;
    msg->data = field_8C;
    msg->priority = 0;
    return 0;
}

// Default virtual method implementations - do nothing
// Derived handlers override these

void Handler::Init(SC_Message* msg) {
    CopyCommandData(msg);
}

int Handler::HandleMessage(SC_Message* msg) {
    return 1; // Handled
}

void Handler::Update(SC_Message* msg) {
}

void Handler::Draw(int param1, int param2) {
}

void Handler::Exit() {
}

void Handler::OnInput() {
}
