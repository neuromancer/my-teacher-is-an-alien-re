#include "SC_Combat.h"
#include "Palette.h"
#include "Sprite.h"
#include "SoundList.h"
#include "SlimeTable.h"
#include "SpriteAction.h"
#include "ZBufferManager.h"
#include <string.h>
#include <stdio.h>

extern "C" void WriteToLog(const char*, ...);
extern ZBufferManager* g_ZBufferManager_0046aa24;

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
        field_0x104 = (int)new Palette();
        ((Palette*)field_0x104)->Load(local_bc);
    } else if (strcmp(local_3c, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite((char*)0);
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(local_3c, "CONSOLE_SPRITE") == 0) {
        field_0x10C = (int)new Sprite((char*)0);
        Parser::ProcessFile((Parser*)field_0x10C, this, (char*)0);
    } else if (strcmp(local_3c, "VIDEO_RES") == 0) {
        sscanf(line, " %s %d %d", local_3c, &field_0xAC[4], &field_0xAC[5]);
        SetVideoRes(field_0xAC[4], field_0xAC[5]);
    } else if (strcmp(local_3c, "BG_SOUND") == 0) {
        sscanf(line, " %s %d", local_3c, &field_0x114);
    } else if (strcmp(local_3c, "MAX_SOUNDS") == 0) {
        sscanf(line, " %s %d", local_3c, &local_14);
        ((SlimeTable*)bgSound)->Allocate(local_14);
    } else if (strcmp(local_3c, "SOUND") == 0) {
        sscanf(line, " %s %d %s %d", local_3c, &local_14, local_bc, &local_1c);
        if (local_14 < 0 || *(int*)bgSound - 1 < local_14) {
            ReportUnknownLabel("SC_Combat");
        } else {
            ((SlimeTable*)bgSound)->LoadEntry(local_14, local_bc, local_1c);
        }
    } else if (strcmp(local_3c, "MAX_ACTIONS") == 0) {
        sscanf(line, " %s %d", local_3c, &local_14);
        int count = local_14;
        if (statusPtr != 0) {
            FreeMemory(statusPtr);
            statusPtr = 0;
        }
        field_0xAC[2] = count;
        int* arr = (int*)operator new(count * 4);
        statusPtr = arr;
        {
            unsigned int n = field_0xAC[2] & 0x3FFFFFFF;
            while (n != 0) {
                *arr = 0;
                arr++;
                n--;
            }
        }
    } else if (strcmp(local_3c, "ACTION") == 0) {
        sscanf(line, " %s %d %d", local_3c, &local_14, &local_1c);
        *(int*)((int)statusPtr + local_14 * 4) = local_1c;
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
            WriteToLog("double palette");
        }
        *(int*)((char*)mgr + 0xA8) = field_0x104;
    }
}

/* Function start: 0x449260 */
void SC_Combat::Init(SC_Message* msg) {
    int* msgData;

    memset(&statusPtr, 0, 0x1C * 4);
    bgSound = (SoundList*)new SlimeTable();
    msgData = (int*)msg;
    if (msgData[0xD] != 0) {
        pendingAction = (SpriteAction*)msgData[0xD];
        msgData[0xD] = 0;
    }
    CopyCommandData(msg);
    moduleParam = msgData[1];
}

/* Function start: 0x449400 */
int SC_Combat::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    return 0;
}

/* Function start: 0x449320 */
int SC_Combat::ShutDown(SC_Message* msg) {
    if (field_0x104 != 0) {
        delete (Palette*)field_0x104;
        field_0x104 = 0;
    }
    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }
    if (field_0x10C != 0) {
        delete (Sprite*)field_0x10C;
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
    field_0xAC[0] = 0;
    if (field_0xAC[2] <= 0) return;
    do {
        int idx = field_0xAC[0];
        int* entry = (int*)((int)statusPtr + idx * 4);
        if (*entry != 0) {
            ProcessAction(idx, entry);
        }
        field_0xAC[0]++;
    } while (field_0xAC[2] > field_0xAC[0]);
}
/* Function start: 0x449410 */
int SC_Combat::Exit(SC_Message* msg) {
    int* msgData = (int*)msg;
    if (msgData[0] != handlerId) {
        return 0;
    }
    int cmd = msgData[4];
    if (cmd == 0) {
        return 1;
    }
    if (cmd == 7) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    }
    if (cmd == 0x17) {
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
/* Function start: 0x4491B0 */
void SC_Combat::ProcessAction(int action, int* data) {}
