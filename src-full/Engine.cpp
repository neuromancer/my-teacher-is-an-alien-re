#include "Engine.h"
#include "DrawEntry.h"
#include "Animation.h"
#include "VBuffer.h"
#include "Memory.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sound.h"
#include "ZBufferManager.h"
#include "Handler.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" void WriteToLog(const char* format, ...);
extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

#include "SlimeTable.h"

extern Sound* g_EngineSound_0046aa0c;              // Sound global
extern ZBufferManager* g_ZBufferManager_0046aa24;     // ZBufferManager*

// DrawEntry implementation (originally in same source file as Engine)
DrawEntry::~DrawEntry() {
    VBuffer* videoBuffer;
    void* childObject;

    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        videoBuffer->~VBuffer();
        FreeFromGlobalHeap(videoBuffer);
        DrawEntry::m_videoBuffer = 0;
    }

    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete (Animation*)childObject;
        DrawEntry::m_childObject = 0;
    }
}

void Engine::OnProcessStart() {}

/* Function start: 0x449100 */
Engine::Engine() : handlerId(0), moduleParam(0), savedCommand(0), savedMsgData(0), field_A0(0), field_A4(0) {
    memset(&actionArray, 0, 112);
}

/* Function start: 0x4491E0 */
Engine::~Engine() {
    ShutDown(0);
}

/* Function start: 0x449320 */
void Engine::ShutDown(int flag) {
    if (palette != 0) { delete palette; palette = 0; }
    if (bgSprite != 0) { delete bgSprite; bgSprite = 0; }
    if (consoleSprite != 0) { delete consoleSprite; consoleSprite = 0; }
    if (pendingAction != 0) { delete pendingAction; pendingAction = 0; }
    if (soundTable != 0) { delete soundTable; soundTable = 0; }
    if (actionArray != 0) { FreeMemory(actionArray); actionArray = 0; }
}

/* Function start: 0x449400 */
int Engine::AddMessage(int* param) {
    ((Handler*)this)->WriteMessageAddress((SC_Message*)param);
    return 0;
}

/* Function start: 0x405D60 */
void Engine::CheckNavState(int* param) {}

/* Function start: 0x4494E0 */
void Engine::ProcessTargets() {
    EnqueueSpriteAction(pendingAction);
    if (pendingAction != 0) { delete pendingAction; pendingAction = 0; }
}

/* Function start: 0x42BF00 */
void Engine::StopAndCleanup() {
    g_EngineSound_0046aa0c->StopAllSamples();
    ShutDown(0);
}

/* Function start: 0x401160 */
void Engine::LogHandler(int param) {
    WriteToLog("hIam %d", handlerId);
}

/* Function start: 0x4491B0 */
void Engine::ProcessAction(int index, int* action) {}

/* Function start: 0x449410 */
int Engine::Exit(int* param) {
    if (*param != handlerId) return 0;
    switch (param[4]) {
    case 0: return 1;
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        return 1;
    }
    return 0;
}

/* Function start: 0x449480 */
void Engine::Update(int p1, int p2) {
    if (handlerId != p2) return;
    actionIndex = 0;
    if (actionCount <= 0) return;
    do {
        int* action = &actionArray[actionIndex];
        if (*action != 0) {
            ProcessAction(actionIndex, action);
        }
        actionIndex++;
    } while (actionCount > actionIndex);
}

/* Function start: 0x449260 */
void Engine::Init(int* param) {
    memset(&actionArray, 0, 112);
    soundTable = new SlimeTable();
    if (param[13] != 0) {
        pendingAction = (SpriteAction*)param[13];
        param[13] = 0;
    }
    ((Handler*)this)->CopyCommandData((SC_Message*)param);
    moduleParam = param[1];
}

/* Function start: 0x449520 */
void Engine::OnProcessEnd() {
    if (pendingAction == 0) {
        pendingAction = new SpriteAction(
            savedCommand, savedMsgData,
            handlerId, moduleParam,
            4, 0, 0, 0, 0, 0);
    }
    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }
}

/* Function start: 0x449600 */
int Engine::LBLParse(char* line) {
    int local_14 = 0;
    int local_1c = 0;
    char filename[128];
    char label[32];

    filename[0] = 0;
    label[0] = 0;
    sscanf(line, " %s", label);

    if (strcmp(label, "PALETTE") == 0) {
        sscanf(line, "%s %s", label, filename);
        palette = new Palette();
        palette->Load(filename);
    } else if (strcmp(label, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite(0);
        Parser::ProcessFile((Parser*)bgSprite, this, 0);
    } else if (strcmp(label, "CONSOLE_SPRITE") == 0) {
        consoleSprite = new Sprite(0);
        Parser::ProcessFile((Parser*)consoleSprite, this, 0);
    } else if (strcmp(label, "VIDEO_RES") == 0) {
        sscanf(line, " %s %d %d ", label, &videoDimensions.x, &videoDimensions.y);
        SetVideoRes(videoDimensions.x, videoDimensions.y);
    } else if (strcmp(label, "BG_SOUND") == 0) {
        sscanf(line, " %s %d ", label, &bgSoundHandle);
    } else if (strcmp(label, "MAX_SOUNDS") == 0) {
        sscanf(line, " %s %d ", label, &local_14);
        soundTable->Allocate(local_14);
    } else if (strcmp(label, "SOUND") == 0) {
        sscanf(line, " %s %d %s %d ", label, &local_14, filename, &local_1c);
        if (local_14 < 0) goto lbl_error;
        if (*((int*)soundTable) - 1 < local_14) goto lbl_error;
        soundTable->LoadEntry(local_14, filename, local_1c);
    } else if (strcmp(label, "MAX_ACTIONS") == 0) {
        sscanf(line, " %s %d ", label, &local_14);
        if (actionArray != 0) { FreeMemory(actionArray); actionArray = 0; }
        actionCount = local_14;
        actionArray = (int*)malloc(local_14 * 4);
        memset(actionArray, 0, actionCount * 4);
    } else if (strcmp(label, "ACTION") == 0) {
        sscanf(line, " %s %d %d ", label, &local_14, &local_1c);
        actionArray[local_14] = local_1c;
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
lbl_error:
        ReportUnknownLabel("SC_Combat");
    }
    return 0;
}
