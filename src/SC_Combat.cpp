#include "SC_Combat.h"
#include "Palette.h"
#include "Sprite.h"
#include "SoundList.h"
#include "SlimeTable.h"
#include "SpriteAction.h"
#include "ZBufferManager.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

extern "C" void WriteToLog(const char*, ...);

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" void SetVideoRes(int, int);
extern void FreeMemory(void*);

/* Function start: 0x449100 */
SC_Combat::SC_Combat() {
    memset(&statusPtr, 0, 0x70);
}

SC_Combat::~SC_Combat() {
}

/* Function start: 0x449600 */
int SC_Combat::LBLParse(char* line) {
    char local_bc[128];
    char local_3c[32];
    int local_1c;
    int local_14;

    local_14 = 0;
    local_1c = 0;
    local_bc[0] = 0;
    local_3c[0] = 0;
    sscanf(line, " %s ", local_3c);

    if (strcmp(local_3c, "PALETTE") == 0) {
        sscanf(line, "%s %s", local_3c, local_bc);
        field_0x104 = new Palette();
        field_0x104->Load(local_bc);
    } else if (strcmp(local_3c, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite((char*)0);
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(local_3c, "CONSOLE_SPRITE") == 0) {
        field_0x10C = new Sprite((char*)0);
        Parser::ProcessFile(field_0x10C, this, (char*)0);
    } else if (strcmp(local_3c, "VIDEO_RES") == 0) {
        sscanf(line, " %s %d %d", local_3c, &combatParams[3], &combatParams[4]);
        SetVideoRes(combatParams[3], combatParams[4]);
    } else if (strcmp(local_3c, "BG_SOUND") == 0) {
        sscanf(line, " %s %d", local_3c, &field_0x114);
    } else if (strcmp(local_3c, "MAX_SOUNDS") == 0) {
        sscanf(line, " %s %d", local_3c, &local_14);
        bgSound->Allocate(local_14);
    } else if (strcmp(local_3c, "SOUND") == 0) {
        sscanf(line, " %s %d %s %d ", local_3c, &local_14, local_bc, &local_1c);
        if (local_14 < 0 || bgSound->fields[0] - 1 < local_14) {
            ReportUnknownLabel("SC_Combat");
        } else {
            bgSound->LoadEntry(local_14, local_bc, local_1c);
        }
    } else if (strcmp(local_3c, "MAX_ACTIONS") == 0) {
        sscanf(line, " %s %d", local_3c, &local_14);
        int count = local_14;
        if (statusPtr != 0) {
            FreeMemory(statusPtr);
            statusPtr = 0;
        }
        combatParams[2] = count;
        int* arr = (int*)operator new(count * 4);
        statusPtr = arr;
        {
            unsigned int n = combatParams[2] & 0x3FFFFFFF;
            while (n != 0) {
                *arr = 0;
                arr++;
                n--;
            }
        }
    } else if (strcmp(local_3c, "ACTION") == 0) {
        sscanf(line, " %s %d %d", local_3c, &local_14, &local_1c);
        statusPtr[local_14] = local_1c;
    } else if (strcmp(local_3c, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_Combat");
    }

    return 0;
}
/* Function start: 0x449520 */
/* Function start: 0x449520 */
void SC_Combat::OnProcessEnd() {
    if (pendingAction == 0) {
        pendingAction = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }
    if (field_0x104 != 0) {
        ZBufferManager* mgr = (ZBufferManager*)g_ZBufferManager_0046aa24;
        if (*(int*)((char*)mgr + 0xA8) != 0) {
            WriteToLog("ddouble palette");
        }
        mgr->m_palette = field_0x104;
    }
}

/* Function start: 0x449260 */
void SC_Combat::Init(SC_MessageParser* msg) {
    SpriteAction* action;

    memset(&statusPtr, 0, 0x1C * 4);
    bgSound = new SlimeTable();
    action = (SpriteAction*)msg;
    if (action->childAction != 0) {
        pendingAction = action->childAction;
        action->childAction = 0;
    }
    CopyCommandData(msg);
    moduleParam = action->addressValue;
}

/* Function start: 0x449400 */
int SC_Combat::AddMessage(SC_MessageParser* msg) {
    WriteMessageAddress(msg);
    return 0;
}

/* Function start: 0x449320 */
int SC_Combat::ShutDown(SC_MessageParser* msg) {
    if (field_0x104 != 0) {
        delete field_0x104;
        field_0x104 = 0;
    }
    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }
    if (field_0x10C != 0) {
        delete field_0x10C;
        field_0x10C = 0;
    }
    if (pendingAction != 0) {
        delete pendingAction;
        pendingAction = 0;
    }
    if (bgSound != 0) {
        delete bgSound;
        bgSound = 0;
    }
    if (statusPtr != 0) {
        FreeMemory(statusPtr);
        statusPtr = 0;
    }
    return 0;
}

/* Function start: 0x449480 */
void SC_Combat::Update(int p1, int p2) {
    if (handlerId != p2) return;
    combatParams[0] = 0;
    if (combatParams[2] <= 0) return;
    do {
        int idx = combatParams[0];
        int* entry = &statusPtr[idx];
        if (*entry != 0) {
            ProcessAction(idx, entry);
        }
        combatParams[0]++;
    } while (combatParams[2] > combatParams[0]);
}
/* Function start: 0x449410 */
int SC_Combat::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    switch (action->instruction) {
    case 0:
        return 1;
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        return 1;
    }
    return 0;
}
/* Function start: 0x4494E0 */
void SC_Combat::ProcessLose() {
    EnqueueSpriteAction(pendingAction);
    if (pendingAction != 0) {
        delete pendingAction;
        pendingAction = 0;
    }
}
/* Function start: 0x4491B0 */ /* No assembly extracted */
void SC_Combat::ProcessAction(int action, int* data) {}
