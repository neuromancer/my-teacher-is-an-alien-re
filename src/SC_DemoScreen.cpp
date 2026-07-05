#include "SC_DemoScreen.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"

#include "MouseControl.h"
#include "globals.h"
#include "SpriteAction.h"
#include "string.h"

/* Function start: 0x44E5C0 */
SC_DemoScreen::SC_DemoScreen() {
    field_A8 = 0;
    demoSprite = 0;
    handlerId = 0x29;
    moduleParam = 1;

    field_A8 = new Palette();
    field_A8->Load("elements\\Demoscrn.col");

    demoSprite = new Sprite("elements\\Demoscrn.smk");
    demoSprite->flags &= ~2;
    demoSprite->priority = 0;
    demoSprite->loc.x = 0;
    demoSprite->loc.y = 0;
}

/* Function start: 0x44E740 */
SC_DemoScreen::~SC_DemoScreen() {
    if (demoSprite != 0) {
        delete (Sprite*)demoSprite;
        demoSprite = 0;
    }
    if (field_A8 != 0) {
        delete (Palette*)field_A8;
        field_A8 = 0;
    }
}

/* Function start: 0x44E7F0 */
void SC_DemoScreen::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);
    Palette* pal = (Palette*)field_A8;
    if (pal != 0) {
        Palette** palSlot = &g_ZBufferManager_0046aa24->m_palette;
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = pal;
    }
}

/* Function start: 0x44E830 */
void SC_DemoScreen::ShutDown(SC_MessageParser* msg) {
    if (demoSprite != 0) {
        delete (Sprite*)demoSprite;
        demoSprite = 0;
    }
    if (field_A8 != 0) {
        delete (Palette*)field_A8;
        field_A8 = 0;
    }
    SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    return;
}

/* Function start: 0x44E8B0 */
int SC_DemoScreen::AddMessage(SC_MessageParser* msg) {
    SpriteAction* pmsg = (SpriteAction*)msg;
    WriteMessageAddress(msg);
    if (pmsg->button1 >= 2) {
        pmsg->addressType = 0x25;
        pmsg->addressValue = 1;
        pmsg->fromValue = 1;
        pmsg->fromType = 0x29;
        pmsg->instruction = 4;
    }
    return 1;
}

/* Function start: 0x44E8F0 */
int SC_DemoScreen::Exit(SC_MessageParser* msg) {
    SpriteAction* pmsg = (SpriteAction*)msg;
    return (handlerId == pmsg->addressType);
}

/* Function start: 0x44E910 */
void SC_DemoScreen::Update(int param1, int param2) {
    if (handlerId == param2) {
        Sprite* spr = (Sprite*)demoSprite;
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
        (g_Mouse_0046aa18)->DrawCursor();
    }
}
