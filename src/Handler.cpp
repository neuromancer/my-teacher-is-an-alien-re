#include "Handler.h"
#include "SC_Question.h"
#include "string.h"
#include <string.h>

// Handler base class destructor
Handler::~Handler() {
}

/* Function start: 0x4309A0 */
// CopyCommandData - saves FROM fields from SpriteAction message
// Original reads msg+0x08 (field_08) and msg+0x0C (field_0C)
void Handler::CopyCommandData(SC_MessageParser* msg) {
    int* pmsg = (int*)msg;
    if (pmsg != 0) {
        savedCommand = pmsg[2];  // offset 0x08
        savedMsgData = pmsg[3];  // offset 0x0C
    }
}

// Default WriteMessageAddress implementation
// Corresponds to FUN_004171B0
/* Function start: 0x4309C0 */
int Handler::WriteMessageAddress(SC_MessageParser* msg) {
    //ShowMessage("WriteMessage mouseX=%d click=(%d,%d)", msg->mouseX, msg->clickX, msg->clickY);
    int* pmsg = (int*)msg;
    if (pmsg == 0) {
        return -1;
    }
    pmsg[0] = handlerId;
    pmsg[1] = moduleParam;
    pmsg[2] = handlerId;
    pmsg[3] = moduleParam;
    pmsg[4] = 0;
    return 0;
}

// Default virtual method implementations - do nothing
// Derived handlers override these

void Handler::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);
}

int Handler::AddMessage(SC_MessageParser* msg) {
    return 1; // Handled
}

int Handler::ShutDown(SC_MessageParser* msg) {
    return 0;
}

void Handler::Update(int param1, int param2) {
}

int Handler::Exit(SC_MessageParser* msg) {
    return 0;
}

/* Function start: 0x405D60 */
void Handler::Serialize(void* param) {
}

void Handler::OnInput(void* param) {
}

void Handler::InitFromMessage(SC_MessageParser*) {}
