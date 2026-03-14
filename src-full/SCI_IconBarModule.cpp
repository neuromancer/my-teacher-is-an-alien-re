#include "SCI_IconBarModule.h"
#include "SC_Question.h"
#include "GameState.h"
#include "InputManager.h"
#include "MMPlayer.h"
#include <stdio.h>
#include <string.h>
#include "TimeOut.h"

extern void* DAT_0046aa08;
extern void* DAT_0046aa18;
extern void* DAT_0046a6e4;
extern char* DAT_0046aa00;
extern char* DAT_0046aa2c;
extern "C" extern void* DAT_0046aa30;
extern int DAT_00468764;

extern void __fastcall FUN_00445970(void*);
extern void __fastcall FUN_004459a0(void*, int, int);
// FUN_0044cb40 is __thiscall (this in ECX, 2 stack params)
class CursorControl {
public:
    void SetCursor(int hotspot, int param2); // 0x44CB40
};
void CursorControl::SetCursor(int, int) {}

extern void __fastcall FUN_00432da0(void*);
extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x401000 */
SCI_IconBarModule::SCI_IconBarModule() {
}

SCI_IconBarModule::~SCI_IconBarModule() {
}

/* Function start: 0x401CF0 */
int SCI_IconBarModule::ShutDown(SC_Message* msg) {
    Sprite** pIcon;
    int i;

    if (msg == 0) {
        if (field_E8 != 0) {
            ((MMPlayer*)field_E8)->StopAll();
        }
    } else {
        if (*(int*)msg == 0x1f) {
            goto skip_cursor;
        }
        if (field_E8 != 0) {
            ((MMPlayer*)field_E8)->StopAll();
        }
    }

skip_cursor:
    pIcon = icons;
    i = 15;
    do {
        if (*pIcon != 0) {
            FUN_00445970(*pIcon);
        }
        pIcon = pIcon + 1;
        i = i - 1;
    } while (i != 0);

    if (*(void**)((char*)DAT_0046aa18 + 0x94) != 0) {
        ((CursorControl*)*(void**)((char*)DAT_0046aa18 + 0x94))->SetCursor(0, 0);
    }

    FUN_00432da0(DAT_0046aa18);

    if (field_AC == 0) {
        IconBar::ShutDown(msg);
    }

    return 0;
}

/* Function start: 0x401D80 */
void SCI_IconBarModule::Update(int param1, int param2) {
    Sprite** pIcon;
    int i;
    int spriteParam;

    if (param2 == 0x1f) {
        ((MMPlayer*)field_E8)->Draw();
    }
    if (handlerId != param2) {
        return;
    }
    if (DAT_00468764 != 0) {
        if (((TimeOut*)field_E0)->IsTimeOut() != 0) {
            SendGameMessage(0x2c, 0, 0, 0, 0x3b, 0, 0, 0, 0, 0);
            return;
        }
    }
    if (field_AC == 0) {
        IconBar::Update(param1, param2);
    }
    spriteParam = param1;

    ((MMPlayer*)field_E8)->Draw();
    pIcon = icons;
    i = 15;
    do {
        if (*pIcon != 0) {
            FUN_004459a0(*pIcon, 0, spriteParam);
        }
        pIcon = pIcon + 1;
        i = i - 1;
    } while (i != 0);

    UpdateCursor();
}

/* Function start: 0x401E40 */
int SCI_IconBarModule::AddMessage(SC_Message* msg) {
    int* gsPtr;
    int idx;
    int* pCounter;
    int iconIdx;

    ((TimeOut*)field_E0)->Start(2000);

    if (field_AC == 0) {
        if (IconBar::AddMessage(msg) != 0) {
            return 1;
        }
    } else {
        IconBar::CheckButtonClick(msg);
    }

    if (((int*)msg)[0xb] != 0) {
        return 1;
    }

    if (((int*)msg)[9] >= 2) {
        iconIdx = FindClickedIcon(((int*)msg)[7], ((int*)msg)[8]);
        if (iconIdx != -1) {
            ((int*)msg)[5] = iconIdx;
            if (((int*)msg)[9] == 2) {
                ((int*)msg)[4] = 2;
            } else if (((int*)msg)[9] == 3) {
                ((int*)msg)[4] = 3;
            }

            if (DAT_0046a6e4 != 0) {
                ((int*)msg)[6] = *(int*)((char*)DAT_0046a6e4 + 0x94);
            } else {
                ((int*)msg)[6] = 0;
            }

            if (field_C4 == 0) {
                gsPtr = (int*)DAT_0046aa30;
                idx = ((GameState*)DAT_0046aa30)->FindLabel("NUM_ACTIONS");
                if (idx < 0 || gsPtr[0x98 / 4] - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                pCounter = (int*)(gsPtr[0x90 / 4] + idx * 4);
                *pCounter = *pCounter + 1;
            }

            Exit(msg);

            *(int*)msg = 0;
        }

        if (field_B8 != 0) {
            int mx = ((int*)msg)[7];
            int my = ((int*)msg)[8];
            int inRect;
            if (field_D0 > mx || field_D8 < mx || field_D4 > my || field_DC < my) {
                inRect = 0;
            } else {
                inRect = 1;
            }
            if (inRect != 0) {
                SendGameMessage(0x2c, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
            }
        }
    } else if (((int*)msg)[10] >= 2) {
        gsPtr = (int*)DAT_0046aa30;
        idx = ((GameState*)DAT_0046aa30)->FindLabel("NUM_ACTIONS");
        if (idx < 0 || gsPtr[0x98 / 4] - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        pCounter = (int*)(gsPtr[0x90 / 4] + idx * 4);
        *pCounter = *pCounter + 1;
    }

    return 1;
}

/* Function start: 0x4020A0 */
int SCI_IconBarModule::FindClickedIcon(int x, int y) {
    int i;
    Sprite** pIcon;
    Sprite* icon;
    int inRect;

    i = 0;
    pIcon = icons;
    do {
        icon = *pIcon;
        if (icon != 0) {
            if (x < icon->loc_x || icon->num_states < x ||
                y < icon->loc_y || icon->field_0xb0 < y) {
                inRect = 0;
            } else {
                inRect = 1;
            }
            if (inRect != 0) {
                return i;
            }
        }
        pIcon = pIcon + 1;
        i = i + 1;
        if (i >= 15) {
            return -1;
        }
    } while (1);
}

/* Function start: 0x402100 */
void SCI_IconBarModule::UpdateCursor() {
    int* mousePos;
    int mouseX;
    int mouseY;
    int iconIdx;
    int hotspot;

    mousePos = *(int**)((char*)DAT_0046aa08 + 0x1a0);
    mouseY = 0;
    if (mousePos != 0) {
        mouseY = mousePos[1];
    }
    if (mousePos != 0) {
        mouseX = mousePos[0];
    } else {
        mouseX = 0;
    }

    iconIdx = FindClickedIcon(mouseX, mouseY);

    if (DAT_0046a6e4 != 0) {
        hotspot = *(int*)((char*)DAT_0046a6e4 + 0x94) + 0x1d;
        if (*(void**)((char*)DAT_0046aa18 + 0x94) == 0) {
            goto done;
        }
        ((CursorControl*)*(void**)((char*)DAT_0046aa18 + 0x94))->SetCursor(hotspot, 0);
    } else if (iconIdx != -1) {
        Sprite* icon = icons[iconIdx];
        int state = 0;
        int frameIdx = *(int*)((char*)icon + 0x9c);
        int* anim = *(int**)((char*)icon + 0xb4 + frameIdx * 4);
        if (anim != 0) {
            state = *(int*)((char*)anim + 0xb0);
        }
        if (*(void**)((char*)DAT_0046aa18 + 0x94) == 0) {
            goto done;
        }
        ((CursorControl*)*(void**)((char*)DAT_0046aa18 + 0x94))->SetCursor(state, 0);
    } else {
        if (*(void**)((char*)DAT_0046aa18 + 0x94) == 0) {
            goto done;
        }
        ((CursorControl*)*(void**)((char*)DAT_0046aa18 + 0x94))->SetCursor(0, 0);
    }

done:
    FUN_00432da0(DAT_0046aa18);
}

/* Function start: 0x4021C0 */
void SCI_IconBarModule::Serialize(void* param) {
    int strLen;
    FILE* fp;
    int id;

    strLen = strlen("INVENTORY_INFO") + 1;
    fp = *(FILE**)((char*)param + 0x44);

    if (*(int*)param != 0) {
        /* SAVE */
        id = handlerId;
        fwrite(&id, 4, 1, fp);
        fwrite("INVENTORY_INFO", strLen, 1, fp);
        fwrite(DAT_0046aa2c, 0x40, 1, fp);
    } else {
        /* LOAD */
        *DAT_0046aa00 = 0;
        fread(DAT_0046aa00, strLen, 1, fp);

        if (strcmp(DAT_0046aa00, "INVENTORY_INFO") != 0) {
            ShowError("SCI_SearchScreen::Serialize() - Error Loading (Wrong ID '%s')", DAT_0046aa00);
        }
        fread(DAT_0046aa2c, 0x40, 1, fp);
    }
}
