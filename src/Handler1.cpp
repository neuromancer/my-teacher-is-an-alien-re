#include "Handler1.h"
#include "SC_Question.h"
#include "string.h"
#include "globals.h"
#include "Message.h"
#include "Mouse.h"
#include <string.h>

extern "C" {
    char* CDData_FormatPath(char*, ...);
}

/* Function start: 0x403340 */
Handler1::Handler1() {
    palette = 0;
    sprite = 0;
    handlerId = 1;
    field_8C = 1;

    palette = new Palette();
    palette->Load(CDData_FormatPath("demo\\Demoscrn.col"));

    sprite = new Sprite("demo\\Demoscrn.smk");
    sprite->flags &= ~2;
    sprite->priority = 0;
    sprite->loc_x = 0;
    sprite->loc_y = 0;
}

/* Function start: 0x4034C0 */
Handler1::~Handler1() {
    if (sprite) {
        delete sprite;
        sprite = 0;
    }
    if (palette) {
        delete palette;
        palette = 0;
    }
}

/* Function start: 0x403570 */
void Handler1::Init(SC_Message* msg) {
    Palette* pal;
    void** palettePtr;
    WriteToMessageLogIfEnabled(L"\"\\nENTER INTRO GAME TEXT\"");
    Handler1::CopyCommandData(msg);
    pal = palette;
    if (pal != 0) {
        palettePtr = &g_ZBufferManager_0043698c->m_fieldA8;
        if (*palettePtr != 0) {
            WriteToMessageLogIfEnabled(L"ddouble palette");
        }
        *palettePtr = pal;
    }
}

/* Function start: 0x403650 */
int Handler1::HandleMessage(SC_Message* msg) {
    Handler1::WriteMessageAddress(msg);
    if (msg->mouseX >= 2) {
        msg->command = 10;
        msg->priority = 5;
        msg->data = 1;
        msg->targetAddress = 1;
        msg->sourceAddress = 1;
    }
    return 1;
}

/* Function start: 0x4035C0 */
int Handler1::Update(SC_Message* msg) {
    Sprite* spr;
    Palette* pal;
    spr = sprite;
    if (spr != 0) {
        delete spr;
        sprite = 0;
    }
    pal = palette;
    if (pal != 0) {
        delete pal;
        palette = 0;
    }
    SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    WriteToMessageLogIfEnabled(L"\"EXIT INTRO GAME TEXT\"");
    return 0;
}

/* Function start: 0x4036C0 */
void Handler1::Draw(int param1, int param2) {
    if (handlerId == param2) {
        sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        g_Mouse_00436978->DrawCursor();
    }
}

/* Function start: 0x4036A0 */
int Handler1::Exit(SC_Message* msg) {
    return handlerId == msg->command;
}

