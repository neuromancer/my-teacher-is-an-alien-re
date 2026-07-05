#include "Handler.h"
#include "SpriteAction.h"
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
    SpriteAction* pmsg = (SpriteAction*)msg;
    if (pmsg != 0) {
        savedCommand = pmsg->fromType;  // offset 0x08
        savedMsgData = pmsg->fromValue;  // offset 0x0C
    }
}

// Default WriteMessageAddress implementation
// Corresponds to FUN_004171B0
/* Function start: 0x4309C0 */
int Handler::WriteMessageAddress(SC_MessageParser* msg) {
    //ShowMessage("WriteMessage mouseX=%d click=(%d,%d)", msg->mouseX, msg->clickX, msg->clickY);
    SpriteAction* pmsg = (SpriteAction*)msg;
    if (pmsg == 0) {
        return -1;
    }
    pmsg->addressType = handlerId;
    pmsg->addressValue = moduleParam;
    pmsg->fromType = handlerId;
    pmsg->instruction = 0;
    pmsg->fromValue = moduleParam;
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

/* Function start: 0x405D40 */
void Handler::ShutDown(SC_MessageParser* msg) {
    return;  // reached via path that doesn't set EAX explicitly in original
}

/* Function start: 0x405D50 */
int Handler::Exit(SC_MessageParser* msg) {
    return 0;
}

/* Function start: 0x405D60 */
void Handler::Serialize(void* param) {
}

/* Function start: 0x401160 */
void Handler::OnInput(void* param) {
    WriteToMessageLog("hIam %d", handlerId);
}

void Handler::InitFromMessage(SC_MessageParser*) {}
