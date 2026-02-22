#include "SCI_SearchScreen.h"
#include "Parser.h"
#include "SC_Question.h"
#include "globals.h"
#include "Message.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern "C" char* CDData_FormatPath(char* path, ...);

/* Function start: 0x40ACC0 */
SCI_SearchScreen::SCI_SearchScreen() {
    Palette** ptr = &field_600;
    memset(ptr, 0, 0x12 * sizeof(int));
    handlerId = 0xb;
    ParseFile(this, "mis\\SrSc0001.mis", 0);
}

/* Function start: 0x40AD70 */
SCI_SearchScreen::~SCI_SearchScreen() {
    Palette* pal;
    MMPlayer* mouse;
    MMPlayer2* dialog;
    DialogControl* dc;
    int i;

    pal = field_600;
    if (pal != 0) {
        delete pal;
        field_600 = 0;
    }

    mouse = background;
    if (mouse != 0) {
        delete mouse;
        background = 0;
    }

    dialog = ambients;
    if (dialog != 0) {
        delete dialog;
        ambients = 0;
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
void SCI_SearchScreen::Init(SC_Message* msg) {
    Palette* pal;
    Palette** pPal;
    Sample* smp;

    WriteToMessageLog("\nENTER SEARCH SCREEN");
    IconBar::InitIconBar(msg);
    if (msg->param1 == 5) {
        field_634 = 0;
    }
    if (msg->param1 == 6) {
        field_634->enabled = 0;
        field_634 = 0;
    }
    field_640 = GetActiveControlCount();
    pal = field_600;
    if (pal != 0) {
        pPal = &g_ZBufferManager_0043698c->m_palette;
        if (*pPal != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *pPal = pal;
    }

    smp = field_638;
    if (smp != 0) {
        smp->Unload();
        operator delete(smp);
        field_638 = 0;
    }

    field_638 = new Sample();
    field_638->Load("audio\\Snd0027.wav");
    field_638->Play(100, 0);
    field_644 = 0;
}

/* Function start: 0x40AFD0 */
int SCI_SearchScreen::ShutDown(SC_Message* msg) {
    int i;

    if (msg == 0 || msg->targetAddress != 9) {
        if (background != 0) {
            background->StopAll();
        }
        if (ambients != 0) {
            // MMPlayer2 StopAll logic (calls Sprite::StopAnimationSound on all)
            // Since we don't have MMPlayer2::StopAll, we might need to call it if it exists.
            // Looking at disassembly 0x408B60, it's a specific method.
            // Let's assume it's StopAll for now.
            ambients->StopAll();
        }
    }

    if ((((msg == 0 || msg->command == 6) || field_640 == 0)) && field_638 != 0) {
        Sample* smp;
        field_638->~Sample();
        smp = field_638;
        if (smp != 0) {
            smp->Unload();
            operator delete(smp);
            field_638 = 0;
        }
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

    IconBar::CleanupIconBar(msg);
    WriteToMessageLog("EXIT SEARCH SCREEN\n");
    return 0;
}

/* Function start: 0x40B0B0 */
int SCI_SearchScreen::AddMessage(SC_Message* msg) {
    if (IconBar::CheckButtonClick(msg)) {
        return 1;
    }
    if (msg->mouseX >= 2 && field_634 == 0) {
        int index = FindControlAtMouse();
        if (index > -1) {
            field_634 = field_60C[index];
        }
    }
    return 1;
}

/* Function start: 0x40B110 */
int SCI_SearchScreen::Exit(SC_Message* msg) {
    return handlerId == msg->targetAddress;
}

/* Function start: 0x40B130 */
void SCI_SearchScreen::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    IconBar::Update(param1, param2);

    background->Draw();
    ambients->Draw();

    if (field_634 != 0) {
        if (field_634->Update((int)background, (int)ambients, moduleParam) == 0) {
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
int SCI_SearchScreen::FindControlAtMouse() {
    DialogControl* dc;
    InputState* pMouse;
    int x;
    int y;
    int inRect;

    int i = 0;
    do {
        dc = field_60C[i];
        if (dc != 0) {
            pMouse = g_InputManager_00436968->pMouse;
            y = 0;
            if (pMouse != 0) {
                y = pMouse->y;
            }
            if (pMouse != 0) {
                x = pMouse->x;
            } else {
                x = 0;
            }
            if (dc->enabled == 0) {
                goto lbl_fail;
            }
            if (dc->rect.left > x) {
                goto lbl_fail;
            }
            if (dc->rect.right < x) {
                goto lbl_fail;
            }
            if (dc->rect.top > y) {
                goto lbl_fail;
            }
            if (dc->rect.bottom < y) {
                goto lbl_fail;
            }
            inRect = 1;
            goto lbl_check;
        lbl_fail:
            inRect = 0;
        lbl_check:
            if (inRect != 0) {
                return i;
            }
        }
        i++;
    } while (i < 10);
    return -1;
}

/* Function start: 0x40B2B0 */
int SCI_SearchScreen::GetActiveControlCount() {
    int count = 0;
    for (int i = 0; i < 10; i++) {
        if (field_60C[i] != 0 && field_60C[i]->enabled != 0) {
            count++;
        }
    }
    return count;
}

/* Function start: 0x40B2E0 */
int SCI_SearchScreen::LBLParse(char* line) {
    char token[32];
    char arg1[64];

    sscanf(line, "%s", token);

    if (strcmp(token, "BACKGROUND") == 0) {
        background = new MMPlayer();
        ProcessFile(background, this, 0);
        goto lbl_ret0;
    }
    if (strcmp(token, "PALE") == 0) {
        sscanf(line, "%s %s", token, arg1);
        if (field_600 != 0) {
            goto lbl_ret0;
        }
        Palette* pal;
        pal = new Palette();
        field_600 = pal;
        pal->Load(CDData_FormatPath(arg1));
        goto lbl_ret0;
    }
    if (strcmp(token, "AMBIENTS") == 0) {
        ambients = new MMPlayer2();
        ProcessFile(ambients, this, 0);
        goto lbl_ret0;
    }
    if (strcmp(token, "HOTSPOT") == 0) {
        DialogControl* dc = new DialogControl();
        field_60C[field_63C] = dc;
        ProcessFile(field_60C[field_63C], this, 0);
        field_63C++;
        goto lbl_ret0;
    }
    if (strcmp(token, "END") == 0) {
        return 1;
    }
    Parser::LBLParse("SCI_SearchScreen");

lbl_ret0:
    return 0;
}
