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

extern Sound* DAT_0046AA0C;              // Sound global
extern ZBufferManager* DAT_0046aa24;     // ZBufferManager*

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
Engine::Engine() : field_0x90(0), field_0x94(0), field_0x98(0), field_0x9C(0), field_0xA0(0), field_0xA4(0) {
    memset(&field_0xA8, 0, 112);
}

/* Function start: 0x4491E0 */
Engine::~Engine() {
    Engine::VirtCleanup(0);
}

/* Function start: 0x449320 */
void Engine::VirtCleanup(int flag) {
    if (Engine::field_0x104 != 0) {
        delete Engine::field_0x104;
        Engine::field_0x104 = 0;
    }
    if (Engine::field_0x108 != 0) {
        delete Engine::field_0x108;
        Engine::field_0x108 = 0;
    }
    if (Engine::field_0x10C != 0) {
        delete Engine::field_0x10C;
        Engine::field_0x10C = 0;
    }
    if (Engine::field_0x100 != 0) {
        delete Engine::field_0x100;
        Engine::field_0x100 = 0;
    }
    if (Engine::field_0x110 != 0) {
        delete Engine::field_0x110;
        Engine::field_0x110 = 0;
    }
    if (Engine::field_0xA8 != 0) {
        FreeMemory(Engine::field_0xA8);
        Engine::field_0xA8 = 0;
    }
}

/* Function start: 0x449400 */
int Engine::CleanupSubsystems(int* param) {
    ((Handler*)this)->WriteMessageAddress((SC_Message*)param);
    return 0;
}

/* Function start: 0x405D60 */
int Engine::CheckNavState(int* param) {
    return 0;
}

/* Function start: 0x4494E0 */
void Engine::ProcessTargets() {
    EnqueueSpriteAction(Engine::field_0x100);
    if (Engine::field_0x100 != 0) {
        delete Engine::field_0x100;
        Engine::field_0x100 = 0;
    }
}

/* Function start: 0x42BF00 */
void Engine::StopAndCleanup() {
    DAT_0046AA0C->StopAllSamples();
    VirtCleanup(0);
}

/* Function start: 0x401160 */
void Engine::fn10(int param) {
    WriteToLog("hIam %d", Engine::field_0x90);
}

/* Function start: 0x4491B0 */
void Engine::fn12(int index, int* action) {
}

/* Function start: 0x449410 */
int Engine::HandleInput(int* param) {
    if (*param != Engine::field_0x90) {
        return 0;
    }
    switch (param[4]) {
    case 0:
        return 1;
    case 7:
        SendGameMessage(1, Engine::field_0x90, Engine::field_0x90, Engine::field_0x94, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        return 1;
    }
    return 0;
}

/* Function start: 0x449480 */
void Engine::ProcessActions(int p1, int p2) {
    if (Engine::field_0x90 != p2) {
        return;
    }
    Engine::field_0xAC = 0;
    if (Engine::m_combatBonus1 <= 0) {
        return;
    }
    do {
        int* action = &Engine::field_0xA8[Engine::field_0xAC];
        if (*action != 0) {
            fn12(Engine::field_0xAC, action);
        }
        Engine::field_0xAC++;
    } while (Engine::m_combatBonus1 > Engine::field_0xAC);
}

/* Function start: 0x449260 */
void Engine::Initialize(int* param) {
    memset(&field_0xA8, 0, 112);
    Engine::field_0x110 = new SlimeTable();
    if (param[13] != 0) {
        Engine::field_0x100 = (SpriteAction*)param[13];
        param[13] = 0;
    }
    ((Handler*)this)->CopyCommandData((SC_Message*)param);
    Engine::field_0x94 = param[1];
}

/* Function start: 0x449520 */
void Engine::OnProcessEnd() {
    if (Engine::field_0x100 == 0) {
        Engine::field_0x100 = new SpriteAction(
            Engine::field_0x98, Engine::field_0x9C,
            Engine::field_0x90, Engine::field_0x94,
            4, 0, 0, 0, 0, 0);
    }
    if (Engine::field_0x104 != 0) {
        int* target = (int*)((int)DAT_0046aa24 + 0xA8);
        if (*target != 0) {
            WriteToLog("ddouble palette");
        }
        *target = (int)Engine::field_0x104;
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
        Engine::field_0x104 = new Palette();
        Engine::field_0x104->Load(filename);
    } else if (strcmp(label, "BACKGROUND_SPRITE") == 0) {
        Engine::field_0x108 = new Sprite(0);
        Parser::ProcessFile((Parser*)Engine::field_0x108, this, 0);
    } else if (strcmp(label, "CONSOLE_SPRITE") == 0) {
        Engine::field_0x10C = new Sprite(0);
        Parser::ProcessFile((Parser*)Engine::field_0x10C, this, 0);
    } else if (strcmp(label, "VIDEO_RES") == 0) {
        sscanf(line, " %s %d %d ", label, &Engine::field_0xB8.field_0, &Engine::field_0xB8.field_4);
        SetVideoRes(Engine::field_0xB8.field_0, Engine::field_0xB8.field_4);
    } else if (strcmp(label, "BG_SOUND") == 0) {
        sscanf(line, " %s %d ", label, &Engine::field_0x114);
    } else if (strcmp(label, "MAX_SOUNDS") == 0) {
        sscanf(line, " %s %d ", label, &local_14);
        Engine::field_0x110->Allocate(local_14);
    } else if (strcmp(label, "SOUND") == 0) {
        sscanf(line, " %s %d %s %d ", label, &local_14, filename, &local_1c);
        if (local_14 < 0) goto lbl_error;
        if (*((int*)Engine::field_0x110) - 1 < local_14) goto lbl_error;
        Engine::field_0x110->LoadEntry(local_14, filename, local_1c);
    } else if (strcmp(label, "MAX_ACTIONS") == 0) {
        sscanf(line, " %s %d ", label, &local_14);
        if (Engine::field_0xA8 != 0) {
            FreeMemory(Engine::field_0xA8);
            Engine::field_0xA8 = 0;
        }
        Engine::m_combatBonus1 = local_14;
        Engine::field_0xA8 = (int*)malloc(local_14 * 4);
        memset(Engine::field_0xA8, 0, Engine::m_combatBonus1 * 4);
    } else if (strcmp(label, "ACTION") == 0) {
        sscanf(line, " %s %d %d ", label, &local_14, &local_1c);
        Engine::field_0xA8[local_14] = local_1c;
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
lbl_error:
        ReportUnknownLabel("SC_Combat");
    }
    return 0;
}

// Demo-compat stubs (not in full game, needed for compilation)
void Engine::Draw() {}
void Engine::UpdateMeter() {}
int Engine::UpdateAndCheck() { return 0; }
void Engine::CopyToGlobals() {}
void Engine::ClearGlobals() {}
void Engine::PlayCompletionSound() {}
void Engine::SetupViewport() {}
