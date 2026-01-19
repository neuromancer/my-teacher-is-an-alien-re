#include "Handler11.h"
#include "Parser.h"
#include "SC_Question.h"
#include <string.h>

/* Function start: 0x40ACC0 */
Handler11::Handler11() {
    Palette** ptr = &field_600;
    memset(ptr, 0, 0x12 * sizeof(int));
    handlerId = 0xb;
    ParseFile(this, "mis\\SrSc0001.mis", 0);
}

/* Function start: 0x40AD70 */
Handler11::~Handler11() {
    Palette* pal;
    MouseControl* mouse;
    SC_Dialog* dialog;
    UnknownClass_4092e0* unk;
    int i;

    pal = field_600;
    if (pal != 0) {
        delete pal;
        field_600 = 0;
    }

    mouse = field_604;
    if (mouse != 0) {
        delete mouse;
        field_604 = 0;
    }

    dialog = field_608;
    if (dialog != 0) {
        delete dialog;
        field_608 = 0;
    }

    for (i = 0; i < 10; i++) {
        unk = field_60C[i];
        if (unk != 0) {
            delete unk;
            field_60C[i] = 0;
        }
    }
}

void Handler11::Init(SC_Message* msg) {
    IconBar::InitIconBar(msg);
}

int Handler11::HandleMessage(SC_Message* msg) {
    return IconBar::CheckButtonClick(msg);
}

int Handler11::Update(SC_Message* msg) {
    IconBar::CleanupIconBar();
    return 0;
}

void Handler11::Draw(int param1, int param2) {
    if (handlerId == param2) {
        IconBar::DrawIconBar(param1, param2);
    }
}

int Handler11::Exit(SC_Message* msg) {
    return handlerId <= msg->targetAddress;
}
