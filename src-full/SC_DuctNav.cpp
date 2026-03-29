#include "SC_DuctNav.h"
#include "Sprite.h"
#include "ZBufferManager.h"
#include "Palette.h"
#include "T_MenuHotspot.h"
#include "CombatSprite.h"
#include "MouseControl.h"
#include "Memory.h"
#include "SpriteAction.h"

#include "GameState.h"
#include "GameEngine.h"
#include "FlagArray.h"
#include "QuestionInit.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>
#include <io.h>

#include "T_MenuHotspot.h"
#include "globals.h"

extern "C" FILE* __cdecl OpenSaveFile(char* path, char* mode);
extern "C" int __cdecl DeleteFileAndDir(char* path);

/* Function start: 0x43E2A0 */
int* __cdecl SavePoolAllocNode(int* pool, int prev, int next) {
    if (pool[3] == 0) {
        int* block = (int*)AllocateMemory(pool[5] * 0x120 + 4);
        *block = pool[4];
        pool[4] = (int)block;
        int count = pool[5];
        int* entry = block + count * 0x48 - 0x47;
        while (count-- >= 0) {
            *entry = pool[3];
            pool[3] = (int)entry;
            entry -= 0x48;
        }
    }
    int* node = (int*)pool[3];
    pool[3] = *node;
    node[1] = prev;
    *node = next;
    pool[2]++;
    int* payload = node + 2;
    int i;
    for (i = 0x46; i != 0; i--) {
        *payload = 0;
        payload++;
    }
    return node;
}

/* Function start: 0x43E320 */
void __cdecl ClearSaveEntries(void* buf, int count) {
    unsigned int dwords = (unsigned int)(count * 0x118) >> 2;
    int* p = (int*)buf;
    for (; dwords != 0; dwords--) {
        *p = 0;
        p++;
    }
}

#include "TextInput.h"

// FileArchive - simple file handle wrapper (0x48 bytes)
// Constructor evidence: 0x43C8B0 (new + memset + strcpy pattern with SEH)
struct FileArchive {
    int mode;           // 0x00
    char filename[64];  // 0x04
    FILE* fp;           // 0x44
    FileArchive(char* path) {
        memset(this, 0, 0x48);
        strcpy(filename, path);
    }
};

// Forward declarations
char* MakeSavePath(char* param);
int ReadConfigFile();

/* Function start: 0x43AF10 */
SC_DuctNav::SC_DuctNav()
{
    handlerId = 0x2E;
    timer.Reset();
    strcpy(searchPattern, "SaveGame__*.sav");
}

#define CREATE_BUTTON(slot, _name, _params) \
    { \
        T_MenuButton* _obj = new T_MenuButton(_name, _params); \
        (slot) = _obj; \
        ((Sprite*)_obj->sprite)->ConfigRange(1, 2, 10, 1); \
    }

/* Function start: 0x43D5B0 */
int SC_DuctNav::LBLParse(char* line)
{
    char label[32];
    char name[128];
    int slotIdx = 0;
    int params[4];
    params[0] = 0;
    params[1] = 0;
    params[2] = 0;
    params[3] = 0;

    sscanf(line, "%s", label);

    if (strcmp(label, "FONT") == 0) {
        sscanf(line, "%s %s", label, name);
        if (fontPalette != 0) {
            delete fontPalette;
            fontPalette = 0;
        }
        fontPalette = new Palette(name);
    } else if (strcmp(label, "SPRITE") == 0) {
        if (menuSprite != 0) {
            delete (Sprite*)menuSprite;
            menuSprite = 0;
        }
        menuSprite = new Sprite((char*)0);
        Parser::ProcessFile(menuSprite, this, (char*)0);
    } else if (strcmp(label, "CANCEL") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (cancelBtn != 0) { delete cancelBtn; cancelBtn = 0; }
        CREATE_BUTTON(cancelBtn, name, params)
    } else if (strcmp(label, "SAVE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (saveBtn != 0) { delete saveBtn; saveBtn = 0; }
        CREATE_BUTTON(saveBtn, name, params)
    } else if (strcmp(label, "LOAD") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (loadBtn != 0) { delete loadBtn; loadBtn = 0; }
        CREATE_BUTTON(loadBtn, name, params)
    } else if (strcmp(label, "OVERWRITE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (overwriteBtn != 0) { delete overwriteBtn; overwriteBtn = 0; }
        CREATE_BUTTON(overwriteBtn, name, params)
    } else if (strcmp(label, "DELETE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (deleteBtn != 0) { delete deleteBtn; deleteBtn = 0; }
        CREATE_BUTTON(deleteBtn, name, params)
    } else if (strcmp(label, "SCROLLUP") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (scrollUpBtn != 0) { delete scrollUpBtn; scrollUpBtn = 0; }
        CREATE_BUTTON(scrollUpBtn, name, params)
    } else if (strcmp(label, "SCROLLDOWN") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        if (scrollDownBtn != 0) { delete scrollDownBtn; scrollDownBtn = 0; }
        CREATE_BUTTON(scrollDownBtn, name, params)
    } else if (strcmp(label, "EDIT_FOCUS_SPRITE") == 0) {
        editFocusSprite = new Sprite((char*)0);
        Parser::ProcessFile(editFocusSprite, this, (char*)0);
    } else if (strcmp(label, "CHOICE_FOCUS_SPRITE") == 0) {
        choiceFocusSprite = new Sprite((char*)0);
        Parser::ProcessFile(choiceFocusSprite, this, (char*)0);
    } else if (strcmp(label, "SLOT") == 0) {
        sscanf(line, " %s %d %d %d %d %d", label, &slotIdx, &params[0], &params[1], &params[2], &params[3]);
        int* rect = &slotRects[slotIdx * 4];
        rect[0] = params[0];
        rect[1] = params[1];
        rect[2] = params[2];
        rect[3] = params[3];
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_DuctNav");
    }

    return 0;
}

extern "C" void WriteToLog(const char* format, ...);
// GlyphFont::LoadFont now in GlyphFont.cpp
extern void ShowError(const char* format, ...);

/* Function start: 0x43D170 */
int GetRandomTeacher() {
    int teacher = 0;
    int counter = 3;

    FILE* fp = OpenSaveFile("cfg\\031568.dat", "r");
    if (fp != 0) {
        fscanf(fp, "%d %d", &teacher, &counter);
        fclose(fp);
    }

    if (counter != 3) {
        teacher = teacher % 3 + 1;
    } else {
        int count = rand() % 10 + 1;
        if (count > 0) {
            do {
                teacher = rand() % 3 + 1;
                count--;
            } while (count != 0);
        }
    }

    fp = OpenSaveFile("cfg\\031568.dat", "w");
    if (fp != 0) {
        fprintf(fp, "%d %d", teacher, counter % 3 + 1);
        fclose(fp);
    }

    return teacher;
}

/* Function start: 0x43CD50 */
void InitNewGame() {
    SpriteAction* action;

    action = new SpriteAction(1, 0x20, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        free(action);
    }

    action = new SpriteAction(1, 0x1e, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        free(action);
    }

    action = new SpriteAction(1, 0x2c, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        free(action);
    }

    if (g_GameState_0046aa30 != 0) {
        g_GameState_0046aa30->~GameState();
        free(g_GameState_0046aa30);
        g_GameState_0046aa30 = 0;
    }

    g_GameState_0046aa30 = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);

    if (g_FlagManager_0046a6e8 != 0) {
        g_FlagManager_0046a6e8->~FlagArray();
        free(g_FlagManager_0046a6e8);
        g_FlagManager_0046a6e8 = 0;
    }

    g_FlagManager_0046a6e8 = new FlagArray("cfg\\question.dat", 10000);
    g_FlagManager_0046a6e8->ClearAllFlags();

    {
        QuestionInit qi("mis\\INIT_Q.mis");
    }

    g_PendingAction_00472d58.addressType = 0x25;
    g_PendingAction_00472d58.addressValue = 1;

    int teacherVal = GetRandomTeacher();

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindLabel("TEACHER_ALIEN");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = teacherVal;

    gs = g_GameState_0046aa30;
    idx = gs->FindLabel("NO_RESUME_NO_SAVE");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 0;

    gs = g_GameState_0046aa30;
    idx = gs->FindLabel("MUST_SAVEGAME");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 1;

    action = new SpriteAction(1, 0x2c, 0, 0, 0x17, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        free(action);
    }
}
SC_DuctNav::~SC_DuctNav()
{
    ShutDown(0);
}

/* Function start: 0x43B2C0 */
int SC_DuctNav::ShutDown(SC_Message* msg)
{
    {
        void* list = saveFileList;
        if (list != 0) {
            ((SpriteHashTable*)list)->Clear();
            FreeMemory(list);
            saveFileList = 0;
        }
    }
    {
        TextInput* ti = (TextInput*)field_0x24C;
        if (ti != 0) {
            ti->~TextInput();
            FreeMemory(ti);
            field_0x24C = 0;
        }
    }
    if (menuSprite != 0) {
        delete menuSprite;
        menuSprite = 0;
    }
    if (fontPalette != 0) {
        delete fontPalette;
        fontPalette = 0;
    }
    if (editFocusSprite != 0) {
        delete editFocusSprite;
        editFocusSprite = 0;
    }
    if (choiceFocusSprite != 0) {
        delete choiceFocusSprite;
        choiceFocusSprite = 0;
    }
    if (cancelBtn != 0) { delete cancelBtn; cancelBtn = 0; }
    if (saveBtn != 0) { delete saveBtn; saveBtn = 0; }
    if (loadBtn != 0) { delete loadBtn; loadBtn = 0; }
    if (overwriteBtn != 0) { delete overwriteBtn; overwriteBtn = 0; }
    if (deleteBtn != 0) { delete deleteBtn; deleteBtn = 0; }
    if (scrollUpBtn != 0) { scrollUpBtn->~T_MenuButton(); FreeMemory(scrollUpBtn); scrollUpBtn = 0; }
    if (scrollDownBtn != 0) { delete scrollDownBtn; scrollDownBtn = 0; }
    return 0;
}

/* Function start: 0x43B0F0 */
void SC_DuctNav::Init(SC_Message* msg) {
    int* param = (int*)msg;

    ((Handler*)this)->CopyCommandData(msg);
    if (param != 0) {
        int mode = param[1];
        moduleParam = mode;
        if (mode == 0) {
            ParseFile(this, "mis\\menu.mis", " SAVEGAME ");
        } else if (mode == 1) {
            ParseFile(this, "mis\\menu.mis", " LOADGAME ");
        } else if (mode == 2) {
            InitNewGame();
            SendGameMessage(3, 10, g_PendingAction_00472d58.addressType, g_PendingAction_00472d58.addressValue, 4, 0, 0, 0, 0, 0);
            return;
        }
    }

    ReadSaveFiles(searchPattern);
    saveFilename[0] = 0;
    selectedRow = -1;
    scrollOffset = 0;

    // Copy first save name to saveFilename
    int* list = (int*)saveFileList;
    if (list[2] != 0) {
        int* node = (int*)list[0];
        strcpy(saveFilename, (char*)(node + 7));
    }

    // Set palette
    if (fontPalette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = fontPalette;
    }

    if (moduleParam != 0) {
        return;
    }

    // Save mode: destroy and recreate text input object
    if (field_0x24C != 0) {
        ((TextInput*)field_0x24C)->~TextInput();
        FreeMemory((void*)field_0x24C);
        field_0x24C = 0;
    }

    ((GlyphFont*)g_GlyphFont_0046aa28)->LoadFont("cfg\\Teacher.pal");

    field_0x24C = (int)new TextInput(saveFilename, 0x36, g_GlyphFont_0046aa28, g_FontFilename_0046bd78);
}

// TextInput::ProcessKey is thiscall (declared in TextInput class above)

#define HOTSPOT_HIT(btn, mx, my) \
    ((btn)->bounds.left <= (mx) && (btn)->bounds.right >= (mx) && \
     (btn)->bounds.top <= (my) && (btn)->bounds.bottom >= (my))

/* Function start: 0x43BB10 */
int SC_DuctNav::AddMessage(SC_Message* msg) {
    int* param = (int*)msg;

    ((Handler*)this)->WriteMessageAddress(msg);
    timer.Reset();

    if (param[0xb] != 0) {
        // Keyboard input
        if (field_0x24C != 0) {
            if (param != 0) {
                int result = ((TextInput*)field_0x24C)->ProcessKey(param[0xb]);
                if (result != 0) {
                    SendGameMessage(4, 0x498, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                    if (field_0x24C != 0) {
                        ((TextInput*)field_0x24C)->~TextInput();
                        FreeMemory((void*)field_0x24C);
                        field_0x24C = 0;
                    }
                    param[4] = 2;
                    param[0xb] = 0xd;
                    goto set_clear_return;
                }
            }
            param[4] = 0;
        } else {
            param[4] = 2;
        }
        return 1;
    }

    // Mouse click
    if (param[9] <= 1) return 1;

    if (moduleParam == 0) {
        // Save mode: check overwriteBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(overwriteBtn, mx, my)) {
            selectedRow = -1;
            param[4] = 0;
            if (field_0x24C == 0) {
                field_0x24C = (int)new TextInput(saveFilename, 0x36, g_GlyphFont_0046aa28, g_FontFilename_0046bd78);
            }
            return 1;
        }
    }

    {
        // Check cancelBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(cancelBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0x1b;
            goto set_clear_return;
        }
    }

    if (moduleParam == 0) {
        // Check saveBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(saveBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0xd;
            goto set_clear_return;
        }
    }

    if (moduleParam == 1) {
        // Check loadBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(loadBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0xd;
            goto set_clear_return;
        }
    }

    {
        // Check deleteBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(deleteBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0x2e;
            goto set_clear_return;
        }
    }

    {
        // Check scrollUpBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(scrollUpBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0x26;
            goto set_clear_return;
        }
    }

    {
        // Check scrollDownBtn
        int mx = param[7];
        int my = param[8];
        SlimeDim coords;
        if (HOTSPOT_HIT(scrollDownBtn, mx, my)) {
            param[4] = 2;
            param[0xb] = 0x28;
            goto set_clear_return;
        }
    }

    {
        // Check slot rects
        int row = 0;
        int* rect = slotRects;
        int mx = param[7];
        do {
            if (rect[0] <= mx && rect[2] >= mx &&
                rect[1] <= param[8] && rect[3] >= param[8]) {
                break;
            }
            rect += 4;
            row++;
        } while (row < 10);

        if (row < 10) {
            selectedRow = row;
            if (field_0x24C != 0) {
                ((TextInput*)field_0x24C)->~TextInput();
                FreeMemory((void*)field_0x24C);
                field_0x24C = 0;
            }
            SendGameMessage(4, 0x498, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
        }
        param[4] = 0;
        return 1;
    }

set_clear_return:
    param[9] = 0;
    return 1;
}

#include "GlyphFont.h"

/* Function start: 0x43B7E0 */
void SC_DuctNav::Update(int p1, int p2) {
    unsigned int elapsed = timer.Update();
    if (elapsed > 60000) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    if (handlerId != p2) {
        return;
    }
    timer.Reset();

    if (menuSprite != 0) {
        menuSprite->Do(menuSprite->loc_x, menuSprite->loc_y, 1.0);
    }

    int* list = (int*)saveFileList;
    int listCount = list[2];
    int idx = scrollOffset;
    if (idx < listCount && idx >= 0) {
        int* node;
        if (idx < listCount) {
            node = (int*)*list;
            for (; idx != 0; idx--) {
                node = (int*)*node;
            }
        } else {
            node = 0;
        }

        int slotIdx = 0;
        int* rects = &slotRects[3];
        do {
            if (node == 0) break;
            char* name = (char*)(node + 7);
            int* nextNode = (int*)*node;
            slotIdx++;
            g_ZBufferManager_0046aa24->ShowText(name, rects[-3], rects[0], 10000, -1);
            rects += 4;
            node = nextNode;
        } while (slotIdx <= 9);
    }

    if (selectedRow != -1) {
        int absIdx = scrollOffset + selectedRow;
        int count = ((int*)saveFileList)[2];
        if (absIdx < count && absIdx >= 0) {
            int* node;
            if (absIdx < count) {
                node = (int*)*(int*)saveFileList;
                for (; absIdx != 0; absIdx--) {
                    node = (int*)*node;
                }
            } else {
                node = 0;
            }
            strcpy(saveFilename, (char*)(node + 7));
        } else {
            saveFilename[0] = 0;
        }
    }

    if (field_0x24C != 0) {
        if (moduleParam == 0 && editFocusSprite != 0) {
            editFocusSprite->Do(editFocusSprite->loc_x, editFocusSprite->loc_y, 1.0);
        }
    } else {
        if (selectedRow != -1 && choiceFocusSprite != 0) {
            int row = selectedRow;
            int* rowData = &slotRects[row * 4];
            choiceFocusSprite->loc_x = rowData[0] - 7;
            choiceFocusSprite->loc_y = rowData[1] - 6;
            choiceFocusSprite->Do(choiceFocusSprite->loc_x, choiceFocusSprite->loc_y, 1.0);
        }
    }

    cancelBtn->SimpleUpdate();
    if (moduleParam == 0) {
        saveBtn->SimpleUpdate();
        overwriteBtn->SimpleUpdate();
    }
    if (moduleParam == 1) {
        loadBtn->SimpleUpdate();
    }
    deleteBtn->SimpleUpdate();
    scrollUpBtn->SimpleUpdate();
    scrollDownBtn->SimpleUpdate();

    sprintf(g_Buffer_0046aa00, "%s", saveFilename);

    if (field_0x24C != 0) {
        int textWidth = ((GlyphFont*)g_GlyphFont_0046aa28)->GetTextWidth(g_Buffer_0046aa00);
        if (textWidth <= (int)g_FontFilename_0046bd78) {
            int blink = g_FontField_0046bd7c;
            g_FontField_0046bd7c++;
            int val = blink % 8;
            if (val < 4) {
                if (moduleParam == 0) {
                    g_ZBufferManager_0046aa24->ShowText(g_Buffer_0046aa00, 0x13a, 0x64, 10000, -1);
                } else {
                    g_ZBufferManager_0046aa24->ShowText(g_Buffer_0046aa00, 0x15c, 0x64, 10000, -1);
                }
                goto done_text;
            }
        }
    }
    if (moduleParam == 0) {
        g_ZBufferManager_0046aa24->ShowText(saveFilename, 0x13a, 0x64, 10000, -1);
    } else {
        g_ZBufferManager_0046aa24->ShowText(saveFilename, 0x15c, 0x64, 10000, -1);
    }
done_text:
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x43C100 */
int SC_DuctNav::Exit(SC_Message* msg)
{
    int* param;
    int* node;

    param = (int*)msg;
    if (param[0] != handlerId) {
        return 0;
    }

    timer.Reset();

    if (param[4] == 1) {
        SendGameMessage(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        return 1;
    } else if (param[4] == 2) {
        goto case_2;
    } else if (param[4] == 7) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    } else {
        return 1;
    }

    case_2:

    {
        int numFiles;
        int maxScroll;
        int* list;

        list = (int*)saveFileList;
        numFiles = list[2];
        maxScroll = numFiles - 9;
        if (maxScroll < 0) {
            maxScroll = 0;
        }

        SendGameMessage(4, 0x498, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);

        switch (param[0xb]) {
        case 0xd:
            if (moduleParam == 0 && saveFilename[0] != 0) {
                WriteSaveFile();
                {
                    GameState* gs;
                    int idx;

                    gs = g_GameState_0046aa30;
                    idx = gs->FindLabel("MUST_SAVEGAME");
                    if (idx < 0 || gs->maxStates - 1 < idx) {
                        ShowError("Invalid gamestate %d", idx);
                    }
                    gs->stateValues[idx] = 0;
                }
                SendGameMessage(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
            } else if (moduleParam == 1 && saveFilename[0] != 0) {
                LoadSaveFile();
                {
                    GameState* gs;
                    int idx;

                    gs = g_GameState_0046aa30;
                    idx = gs->FindLabel("NO_RESUME_NO_SAVE");
                    if (idx < 0 || gs->maxStates - 1 < idx) {
                        ShowError("Invalid gamestate %d", idx);
                    }
                    gs->stateValues[idx] = 0;
                }
                {
                    GameState* gs;
                    int idx;

                    gs = g_GameState_0046aa30;
                    idx = gs->FindLabel("MUST_SAVEGAME");
                    if (idx < 0 || gs->maxStates - 1 < idx) {
                        ShowError("Invalid gamestate %d", idx);
                    }
                    gs->stateValues[idx] = 1;
                }
                EnqueueSpriteAction((void*)&g_PendingAction_00472d58);
            }

            selectedRow = -1;
            ReadSaveFiles(searchPattern);
            return 1;

        case 0x1b:
            SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
            selectedRow = -1;
            return 1;

        case 0x21:
            if (scrollOffset == 0) {
                selectedRow = 0;
            }
            scrollOffset -= 10;
            if (scrollOffset < 0) {
                scrollOffset = 0;
            }
            return 1;

        case 0x22:
            if (maxScroll == scrollOffset) {
                selectedRow = 9;
            }
            scrollOffset += 10;
            if (maxScroll < scrollOffset) {
                scrollOffset = maxScroll;
            }
            return 1;

        case 0x23:
            selectedRow = 9;
            scrollOffset = maxScroll;
            return 1;

        case 0x24:
            scrollOffset = 0;
            selectedRow = 0;
            return 1;

        case 0x26:
            selectedRow--;
            if (selectedRow > 0) {
                return 1;
            }
            selectedRow = 0;
            scrollOffset--;
            if (scrollOffset >= 0) {
                return 1;
            }
            scrollOffset = 0;
            return 1;

        case 0x28:
            selectedRow++;
            if (selectedRow < 10) {
                return 1;
            }
            selectedRow = 9;
            scrollOffset++;
            if (maxScroll >= scrollOffset) {
                return 1;
            }
            scrollOffset = maxScroll;
            return 1;

        case 0x2e:
        {
            char* namePtr;
            namePtr = saveFilename;
            if (saveFilename[0] != 0) {
                DeleteFileAndDir(MakeSavePath(namePtr));
                numFiles--;
                ReadSaveFiles(searchPattern);
                {
                    int curOff = scrollOffset;
                    int curRow = selectedRow;
                    int total = curRow + curOff;
                    saveFilename[0] = 0;
                    if (total >= numFiles) {
                        selectedRow = (numFiles - curOff) - 1;
                    }
                }
                if (selectedRow > -1) {
                    int nodeIdx;
                    nodeIdx = scrollOffset + selectedRow;
                    list = (int*)saveFileList;
                    if (list[2] > nodeIdx) {
                        node = (int*)list[0];
                        while (nodeIdx != 0) {
                            node = (int*)node[0];
                            nodeIdx--;
                        }
                    } else {
                        node = 0;
                    }
                    if (node != 0) {
                        strcpy(namePtr, (char*)(node) + 0x1c);
                    }
                }
            }
            ReadSaveFiles(searchPattern);
            return 1;
        }

        default:
            return 1;
        }
    }
}

/* Function start: 0x43C5B0 */
char* MakeSavePath(char* param)
{
    static char g_savePathBuf[256];
    sprintf(g_savePathBuf, "SaveGame\\%s.sav", param);
    return g_savePathBuf;
}

/* Function start: 0x43C5D0 */
void SC_DuctNav::ReadSaveFiles(char* pattern)
{
    int* pool;
    int* node;
    int* prevNode;
    int count;
    struct _finddata_t findData;
    struct _finddata_t nodeData;
    long handle;

    pool = (int*)saveFileList;
    if (pool != 0) {
        node = (int*)pool[0];
        while (node != 0) {
            { volatile int n = 0; while (n-- != 0); }
            node = (int*)node[0];
        }
        pool[2] = 0;
        pool[3] = 0;
        pool[1] = 0;
        pool[0] = 0;
        {
            int* block = (int*)pool[4];
            while (block != 0) {
                int* nextBlock = (int*)block[0];
                operator delete(block);
                block = nextBlock;
            }
        }
        pool[4] = 0;
        operator delete(pool);
        saveFileList = 0;
    }

    pool = (int*)operator new(0x18);
    if (pool != 0) {
        pool[2] = 0;
        pool[3] = 0;
        pool[1] = 0;
        pool[0] = 0;
        pool[4] = 0;
        pool[5] = 0x14;
    } else {
        pool = 0;
    }

    saveFileList = pool;

    handle = _findfirst(pattern, &findData);
    if (handle == -1) {
        return;
    }

    do {
        _splitpath(findData.name, 0, 0, g_Buffer_0046aa00, 0);
        strcpy((char*)&nodeData, g_Buffer_0046aa00);

        node = (int*)pool[0];
        while (node != 0) {
            memcpy(&nodeData, (char*)(node) + 8, 0x118);
            if (findData.time_write > *(long*)((char*)&nodeData + 0xC)) {
                break;
            }
            node = (int*)node[0];
        }

        if (node == 0) {
            /* Insert at tail */
            prevNode = (int*)pool[1];

            if (pool[3] == 0) {
                int growBy = pool[5];
                int* block = (int*)operator new(growBy * 0x120 + 4);
                block[0] = pool[4];
                pool[4] = (int)block;
                count = growBy;
                {
                    int* ptr = (int*)((char*)block + count * 0x120 - 0x11c);
                    count--;
                    while (count >= 0) {
                        ptr[0] = pool[3];
                        pool[3] = (int)ptr;
                        ptr = (int*)((char*)ptr - 0x120);
                        count--;
                    }
                }
            }

            {
                int* entry = (int*)pool[3];
                pool[3] = entry[0];
                entry[1] = (int)prevNode;
                entry[0] = 0;
                pool[2]++;
                ClearSaveEntries((void*)(entry + 2), 1);
                memcpy((char*)(entry) + 8, &findData, 0x118);
                if ((int*)pool[1] != 0) {
                    *(int*)pool[1] = (int)entry;
                    pool[1] = (int)entry;
                } else {
                    pool[0] = (int)entry;
                    pool[1] = (int)entry;
                }
            }
        } else if (node == 0) {
            /* This branch never happens, but keep for structure */
            int* newNode = SavePoolAllocNode(pool, 0, pool[0]);
            memcpy((char*)(newNode) + 8, &findData, 0x118);
            if (pool[0] == 0) {
                pool[1] = (int)newNode;
                pool[0] = (int)newNode;
            } else {
                *(int*)(pool[0] + 4) = (int)newNode;
                pool[0] = (int)newNode;
            }
        } else {
            int* newNode = SavePoolAllocNode(pool, node[1], (int)node);
            memcpy((char*)(newNode) + 8, &findData, 0x118);
            {
                int* prevPtr = (int*)node[1];
                if (prevPtr == 0) {
                    prevPtr = pool;
                }
                prevPtr[0] = (int)newNode;
                node[1] = (int)newNode;
            }
        }
    } while (_findnext(handle, &findData) == 0);
}

/* Function start: 0x43C8B0 */
void SC_DuctNav::WriteSaveFile()
{
    FileArchive* fa;

    fa = new FileArchive(MakeSavePath(saveFilename));

    fileArchive = fa;

    if (fa->fp != 0) {
        fclose(fa->fp);
        fa->fp = 0;
    }

    {
        FILE* fp;
        fp = OpenSaveFile(fa->filename, "wb");
        fa->fp = fp;
        if (fp == 0) {
            ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
                fa->filename, "wb");
        }
    }

    fa->mode = 1;
    g_GameState_0046aa30->Serialize(fa);
    g_FlagManager_0046a6e8->Serialize(fa);
    ((GameEngine*)g_GameEngine_0046a6ec)->Serialize(fa);
    g_PendingAction_00472d58.Serialize(fa);

    {
        FileArchive* sd = (FileArchive*)fileArchive;
        if (sd->fp != 0) {
            fclose(sd->fp);
            sd->fp = 0;
        }
    }
    {
        FileArchive* sd = (FileArchive*)fileArchive;
        if (sd != 0) {
            if (sd->fp != 0) {
                fclose(sd->fp);
                sd->fp = 0;
            }
            operator delete(sd);
            fileArchive = 0;
        }
    }
}

/* Function start: 0x43CA50 */
void SC_DuctNav::LoadSaveFile()
{
    SpriteAction* action;

    /* Send screen transition 0x20 */
    action = new SpriteAction(1, 0x20, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        operator delete(action);
    }

    /* Send screen transition 0x1e */
    action = new SpriteAction(1, 0x1e, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        operator delete(action);
    }

    /* Send screen transition 0x2c */
    action = new SpriteAction(1, 0x2c, 0, 0, 0x18, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        operator delete(action);
    }

    /* Delete the save file */
    DeleteFileAndDir(MakeSavePath(saveFilename));

    /* Allocate and init FileArchive */
    FileArchive* fa;
    fa = new FileArchive(MakeSavePath(saveFilename));

    fileArchive = fa;

    if (fa->fp != 0) {
        fclose(fa->fp);
        fa->fp = 0;
    }

    {
        FILE* fp;
        fp = OpenSaveFile(fa->filename, "rb");
        fa->fp = fp;
        if (fp == 0) {
            ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
                fa->filename, "rb");
        }
    }

    fa->mode = 0;
    g_GameState_0046aa30->Serialize(fa);
    g_FlagManager_0046a6e8->Serialize(fa);
    ((GameEngine*)g_GameEngine_0046a6ec)->Serialize(fa);
    g_PendingAction_00472d58.Serialize(fa);

    {
        FileArchive* sd = (FileArchive*)fileArchive;
        if (sd->fp != 0) {
            fclose(sd->fp);
            sd->fp = 0;
        }
    }
    {
        FileArchive* sd = (FileArchive*)fileArchive;
        if (sd != 0) {
            if (sd->fp != 0) {
                fclose(sd->fp);
                sd->fp = 0;
            }
            operator delete(sd);
            fileArchive = 0;
        }
    }
}

// Old InitGameState (0x43CD50) and ReadConfigFile (0x43D170) removed —
// superseded by InitNewGame and GetRandomTeacher above

/* Function start: 0x43D250 */
void SC_DuctNav::OnProcessStart()
{
    if (menuSprite != 0) { delete menuSprite; menuSprite = 0; }
    if (fontPalette != 0) { delete fontPalette; fontPalette = 0; }
    if (cancelBtn != 0) { delete cancelBtn; cancelBtn = 0; }
    if (saveBtn != 0) { delete saveBtn; saveBtn = 0; }
    if (loadBtn != 0) { delete loadBtn; loadBtn = 0; }
    if (overwriteBtn != 0) { overwriteBtn->~T_MenuButton(); FreeMemory(overwriteBtn); overwriteBtn = 0; }
    if (deleteBtn != 0) { delete deleteBtn; deleteBtn = 0; }
    if (scrollUpBtn != 0) { scrollUpBtn->~T_MenuButton(); FreeMemory(scrollUpBtn); scrollUpBtn = 0; }
    if (scrollDownBtn != 0) { delete scrollDownBtn; scrollDownBtn = 0; }
}
