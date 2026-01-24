#include "Handler11.h"
#include "Parser.h"
#include "SC_Question.h"
#include "globals.h"
#include "Message.h"
#include "InputManager.h"
#include "Mouse.h"
#include <string.h>
#include <stdio.h>

extern void WriteToMessageLog(const char* format, ...);
extern "C" char* CDData_FormatPath(char* path, ...);

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
    DialogControl* dc;
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
        dc = field_60C[i];
        if (dc != 0) {
            delete dc;
            field_60C[i] = 0;
        }
    }
}

/* Function start: 0x40AE80 */
void Handler11::Init(SC_Message* msg) {
    WriteToMessageLog("\nENTER SEARCH SCREEN");
    IconBar::InitIconBar(msg);
    if (msg->param1 == 5) {
        field_634 = 0;
    }
    if (msg->param1 == 6) {
        if (field_634 != 0) {
            field_634->enabled = 0;
        }
        field_634 = 0;
    }
    field_640 = GetActiveControlCount();
    if (field_600 != 0) {
        if (g_ZBufferManager_0043698c->m_fieldA8 != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0043698c->m_fieldA8 = (void*)field_600;
    }

    if (field_638 != 0) {
        field_638->Unload();
        delete field_638;
        field_638 = 0;
    }

    field_638 = new Sample();
    field_638->Load("audio\\Snd0027.wav");
    field_638->Play(100, 0);
    field_644 = 0;
}

/* Function start: 0x40AFD0 */
int Handler11::Update(SC_Message* msg) {
    int i;

    if (msg == 0 || msg->targetAddress != 9) {
        if (field_604 != 0) {
            field_604->StopAll();
        }
        if (field_608 != 0) {
            // SC_Dialog StopAll logic (calls Sprite::StopAnimationSound on all)
            // Since we don't have SC_Dialog::StopAll, we might need to call it if it exists.
            // Looking at disassembly 0x408B60, it's a specific method.
            // Let's assume it's StopAll for now.
            field_608->StopAll();
        }
    }

    if ((((msg == 0 || msg->command == 6) || field_640 == 0)) && field_638 != 0) {
        field_638->~Sample();
        field_638->Unload();
        free(field_638);
        field_638 = 0;
    }

    for (i = 0; i < 10; i++) {
        if (field_60C[i] != 0) {
            field_60C[i]->Exit();
        }
    }

    if (g_Mouse_00436978->m_sprite != 0) {
        g_Mouse_00436978->m_sprite->SetState2(0);
    }
    g_Mouse_00436978->DrawCursor();

    IconBar::CleanupIconBar();
    WriteToMessageLog("EXIT SEARCH SCREEN\n");
    return 0;
}

/* Function start: 0x40B0B0 */
int Handler11::HandleMessage(SC_Message* msg) {
    if (IconBar::CheckButtonClick(msg)) {
        return 1;
    }
    if (msg->mouseX > 1 && field_634 == 0) {
        int index = FindControlAtMouse();
        if (index >= 0) {
            field_634 = field_60C[index];
        }
    }
    return 1;
}

/* Function start: 0x40B110 */
int Handler11::Exit(SC_Message* msg) {
    return handlerId <= msg->targetAddress;
}

/* Function start: 0x40B130 */
void Handler11::Draw(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    IconBar::DrawIconBar(param1, param2);

    if (field_604 != 0) {
        field_604->Init();
    }

    if (field_608 != 0) {
        // FUN_00408EE0 call in assembly
        field_608->Draw();
    }

    if (field_634 != 0) {
        if (field_634->Update((int)field_604, (int)field_608, field_8C) == 0) {
            return;
        }
        field_634 = 0;
        field_640 = GetActiveControlCount();
        return;
    }

    if (field_640 != 0) {
        int index = FindControlAtMouse();
        if (g_Mouse_00436978->m_sprite != 0) {
            if (index != -1) {
                g_Mouse_00436978->m_sprite->SetState2(1);
            } else {
                g_Mouse_00436978->m_sprite->SetState2(0);
            }
        }
        g_Mouse_00436978->DrawCursor();
    } else {
        field_644++;
        g_Mouse_00436978->DrawCursor();
        if (field_644 == 60) {
            SC_Message_Send(10, 1, 11, 1, 5, 0, 0, 0, 0, 0);
        }
    }
}


/* Function start: 0x40B230 */
int Handler11::FindControlAtMouse() {
    int x, y;
    InputState* pMouse = g_InputManager_00436968->pMouse;
    if (pMouse == 0) {
        x = 0;
        y = 0;
    } else {
        x = pMouse->x;
        y = pMouse->y;
    }

    for (int i = 0; i < 10; i++) {
        DialogControl* dc = field_60C[i];
        if (dc != 0) {
            if (dc->enabled && x >= dc->rect.left && x <= dc->rect.right &&
                y >= dc->rect.top && y <= dc->rect.bottom) {
                return i;
            }
        }
    }
    return -1;
}

/* Function start: 0x40B2B0 */
int Handler11::GetActiveControlCount() {
    int count = 0;
    for (int i = 0; i < 10; i++) {
        if (field_60C[i] != 0 && field_60C[i]->enabled != 0) {
            count++;
        }
    }
    return count;
}

/* Function start: 0x40B2E0 */
int Handler11::LBLParse(char* line) {
    char token[32];
    char arg1[64];
    
    if (sscanf(line, "%s", token) != 1) {
        return 0;
    }

    if (strcmp(token, "BACKGROUND") == 0) {
        field_604 = new MouseControl();
        ProcessFile(field_604, this, 0);
    } else if (strcmp(token, "PALETTE") == 0) {
        if (field_600 == 0) {
            sscanf(line, "%s %s", token, arg1);
            field_600 = new Palette();
            char* path = CDData_FormatPath(arg1);
            field_600->Load(path);
        }
    } else if (strcmp(token, "AMBIENTS") == 0) {
        field_608 = new SC_Dialog();
        ProcessFile(field_608, this, 0);
    } else if (strcmp(token, "HOTSPOT") == 0) {
        DialogControl* dc = new DialogControl();
        field_60C[field_63C] = dc;
        ProcessFile(dc, this, 0);
        field_63C++;
    } else if (strcmp(token, "MUSIC") == 0) {
        return 1;
    } else {
        return Parser::LBLParse("SCI_SearchScreen");
    }
    
    return 0;
}

