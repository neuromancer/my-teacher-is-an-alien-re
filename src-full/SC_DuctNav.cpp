#include "SC_DuctNav.h"
#include "Sprite.h"
#include "Palette.h"
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

extern "C" extern GameState* g_GameState_0046aa30;
extern "C" extern int g_GameEngine_0046a6ec;
extern FlagArray* g_FlagManager_0046a6e8;
extern char* g_Buffer_0046aa00;
extern SpriteAction DAT_00472d58;
extern "C" FILE* __cdecl OpenSaveFile(char* path, char* mode);
extern "C" int __cdecl DeleteFileAndDir(char* path);

extern void FUN_0043e320(void* ptr, int count);
extern int* FUN_0043e2a0(int* pool, int prev, int next);

// Forward declarations
char* MakeSavePath(char* param);
int ReadConfigFile();

/* Function start: 0x43AF10 */
SC_DuctNav::SC_DuctNav()
{
}

#define DESTROY_BUTTON(slot) \
    { \
        int* _btn = (int*)(slot); \
        if (_btn != 0) { \
            _btn[0] = 0x4613D0; \
            void* _spr = (void*)_btn[0x24]; \
            if (_spr != 0) { \
                ((Sprite*)_spr)->~Sprite(); \
                FreeMemory(_spr); \
                _btn[0x24] = 0; \
            } \
            FreeMemory(_btn); \
            (slot) = 0; \
        } \
    }

#define CREATE_BUTTON(slot, _name, _params) \
    { \
        T_MenuHotspot* _obj = new T_MenuHotspot(_name, _params); \
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
        DESTROY_BUTTON(cancelBtn)
        CREATE_BUTTON(cancelBtn, name, params)
    } else if (strcmp(label, "SAVE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(saveBtn)
        CREATE_BUTTON(saveBtn, name, params)
    } else if (strcmp(label, "LOAD") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(loadBtn)
        CREATE_BUTTON(loadBtn, name, params)
    } else if (strcmp(label, "OVERWRITE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(overwriteBtn)
        CREATE_BUTTON(overwriteBtn, name, params)
    } else if (strcmp(label, "DELETE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(deleteBtn)
        CREATE_BUTTON(deleteBtn, name, params)
    } else if (strcmp(label, "SCROLLUP") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(scrollUpBtn)
        CREATE_BUTTON(scrollUpBtn, name, params)
    } else if (strcmp(label, "SCROLLDOWN") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(scrollDownBtn)
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

SC_DuctNav::~SC_DuctNav()
{
}

void SC_DuctNav::Init(SC_Message* msg) {}
int SC_DuctNav::AddMessage(SC_Message* msg) { return 0; }
int SC_DuctNav::ShutDown(SC_Message* msg) { return 0; }
void SC_DuctNav::Update(int p1, int p2) {}

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
    }
    if (param[4] != 2) {
        if (param[4] == 7) {
            SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
            return 1;
        }
        return 1;
    }

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
                EnqueueSpriteAction((void*)&DAT_00472d58);
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
                FUN_0043e320((void*)(entry + 2), 1);
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
            int* newNode = FUN_0043e2a0(pool, 0, pool[0]);
            memcpy((char*)(newNode) + 8, &findData, 0x118);
            if (pool[0] == 0) {
                pool[1] = (int)newNode;
                pool[0] = (int)newNode;
            } else {
                *(int*)(pool[0] + 4) = (int)newNode;
                pool[0] = (int)newNode;
            }
        } else {
            int* newNode = FUN_0043e2a0(pool, node[1], (int)node);
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
    int* saveData;

    saveData = (int*)operator new(0x48);
    if (saveData != 0) {
        memset(saveData, 0, 0x48);
        strcpy((char*)(saveData + 1), MakeSavePath(saveFilename));
    } else {
        saveData = 0;
    }

    fileArchive = saveData;

    if (*(FILE**)((char*)saveData + 0x44) != 0) {
        fclose(*(FILE**)((char*)saveData + 0x44));
        *(FILE**)((char*)saveData + 0x44) = 0;
    }

    {
        FILE* fp;
        fp = OpenSaveFile((char*)(saveData + 1), "wb");
        *(FILE**)((char*)saveData + 0x44) = fp;
        if (fp == 0) {
            ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
                (char*)(saveData + 1), "wb");
        }
    }

    saveData[0] = 1;
    g_GameState_0046aa30->Serialize(saveData);
    g_FlagManager_0046a6e8->Serialize(saveData);
    ((GameEngine*)g_GameEngine_0046a6ec)->Serialize(saveData);
    DAT_00472d58.Serialize(saveData);

    {
        int* sd = (int*)fileArchive;
        if (*(FILE**)((char*)sd + 0x44) != 0) {
            fclose(*(FILE**)((char*)sd + 0x44));
            *(FILE**)((char*)sd + 0x44) = 0;
        }
    }
    {
        int* sd = (int*)fileArchive;
        if (sd != 0) {
            if (*(FILE**)((char*)sd + 0x44) != 0) {
                fclose(*(FILE**)((char*)sd + 0x44));
                *(FILE**)((char*)sd + 0x44) = 0;
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
    int* saveData;

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
    saveData = (int*)operator new(0x48);
    if (saveData != 0) {
        memset(saveData, 0, 0x48);
        strcpy((char*)(saveData + 1), MakeSavePath(saveFilename));
    } else {
        saveData = 0;
    }

    fileArchive = saveData;

    if (*(FILE**)((char*)saveData + 0x44) != 0) {
        fclose(*(FILE**)((char*)saveData + 0x44));
        *(FILE**)((char*)saveData + 0x44) = 0;
    }

    {
        FILE* fp;
        fp = OpenSaveFile((char*)(saveData + 1), "rb");
        *(FILE**)((char*)saveData + 0x44) = fp;
        if (fp == 0) {
            ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
                (char*)(saveData + 1), "rb");
        }
    }

    saveData[0] = 0;
    g_GameState_0046aa30->Serialize(saveData);
    g_FlagManager_0046a6e8->Serialize(saveData);
    ((GameEngine*)g_GameEngine_0046a6ec)->Serialize(saveData);
    DAT_00472d58.Serialize(saveData);

    {
        int* sd = (int*)fileArchive;
        if (*(FILE**)((char*)sd + 0x44) != 0) {
            fclose(*(FILE**)((char*)sd + 0x44));
            *(FILE**)((char*)sd + 0x44) = 0;
        }
    }
    {
        int* sd = (int*)fileArchive;
        if (sd != 0) {
            if (*(FILE**)((char*)sd + 0x44) != 0) {
                fclose(*(FILE**)((char*)sd + 0x44));
                *(FILE**)((char*)sd + 0x44) = 0;
            }
            operator delete(sd);
            fileArchive = 0;
        }
    }
}

/* Function start: 0x43CD50 */
void InitGameState()
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

    /* Destroy and recreate GameState */
    if (g_GameState_0046aa30 != 0) {
        g_GameState_0046aa30->~GameState();
        operator delete(g_GameState_0046aa30);
        g_GameState_0046aa30 = 0;
    }

    {
        GameState* gs;
        gs = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);
        g_GameState_0046aa30 = gs;
    }

    /* Destroy and recreate FlagArray */
    if (g_FlagManager_0046a6e8 != 0) {
        g_FlagManager_0046a6e8->~FlagArray();
        operator delete(g_FlagManager_0046a6e8);
        g_FlagManager_0046a6e8 = 0;
    }

    {
        FlagArray* fm;
        fm = new FlagArray("cfg\\question.dat", 10000);
        g_FlagManager_0046a6e8 = fm;
        fm->SafeClose();
    }

    {
        QuestionInit qi("mis\\INIT_Q.mis");
    }

    /* Init question state */
    DAT_00472d58.addressType = 0x25;
    DAT_00472d58.addressValue = 1;

    {
        int teacherAlien;
        GameState* gs;
        int idx;

        teacherAlien = ReadConfigFile();

        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("TEACHER_ALIEN");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = teacherAlien;

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
    }

    /* Send screen transition 0x2c with type 0x17 */
    action = new SpriteAction(1, 0x2c, 0, 0, 0x17, 0, 0, 0, 0, 0);
    ((GameEngine*)g_GameEngine_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        operator delete(action);
    }
}

/* Function start: 0x43D170 */
int ReadConfigFile()
{
    int val;
    int difficulty;
    FILE* fp;

    val = 0;
    difficulty = 3;

    fp = OpenSaveFile("cfg\\031568.dat", "rb");
    if (fp != 0) {
        fscanf(fp, "%d %d", &val, &difficulty);
        fclose(fp);
    }

    if (difficulty == 3) {
        int count;
        count = rand() % 10 + 1;
        while (count > 0) {
            val = rand() % 3 + 1;
            count--;
        }
    } else {
        val = val % 3 + 1;
    }

    fp = OpenSaveFile("cfg\\031568.dat", "wb");
    if (fp != 0) {
        fprintf(fp, "%d %d", val, difficulty % 3 + 1);
        fclose(fp);
    }

    return val;
}

/* Function start: 0x43D250 */
void SC_DuctNav::OnProcessStart()
{
    /* Destroy menuSprite (0x244) */
    if (menuSprite != 0) {
        menuSprite->~Sprite();
        FreeMemory(menuSprite);
        menuSprite = 0;
    }

    /* Destroy fontPalette (0x240) */
    if (fontPalette != 0) {
        fontPalette->~Palette();
        FreeMemory(fontPalette);
        fontPalette = 0;
    }

    /* Destroy cancelBtn (0x21C) */
    if (cancelBtn != 0) {
        *(int*)cancelBtn = 0x4613D0;
        {
            Sprite* spr = (Sprite*)((int*)cancelBtn)[0x24];
            if (spr != 0) {
                delete spr;
                ((int*)cancelBtn)[0x24] = 0;
            }
        }
        FreeMemory(cancelBtn);
        cancelBtn = 0;
    }

    /* Destroy saveBtn (0x220) */
    if (saveBtn != 0) {
        *(int*)saveBtn = 0x4613D0;
        {
            Sprite* spr = (Sprite*)((int*)saveBtn)[0x24];
            if (spr != 0) {
                delete spr;
                ((int*)saveBtn)[0x24] = 0;
            }
        }
        FreeMemory(saveBtn);
        saveBtn = 0;
    }

    /* Destroy loadBtn (0x224) */
    if (loadBtn != 0) {
        *(int*)loadBtn = 0x4613D0;
        {
            Sprite* spr = (Sprite*)((int*)loadBtn)[0x24];
            if (spr != 0) {
                delete spr;
                ((int*)loadBtn)[0x24] = 0;
            }
        }
        FreeMemory(loadBtn);
        loadBtn = 0;
    }

    /* Destroy overwriteBtn (0x228) */
    if (overwriteBtn != 0) {
        overwriteBtn->~T_MenuHotspot();
        FreeMemory(overwriteBtn);
        overwriteBtn = 0;
    }

    /* Destroy deleteBtn (0x22C) */
    if (deleteBtn != 0) {
        *(int*)deleteBtn = 0x4613D0;
        {
            Sprite* spr = (Sprite*)((int*)deleteBtn)[0x24];
            if (spr != 0) {
                delete spr;
                ((int*)deleteBtn)[0x24] = 0;
            }
        }
        FreeMemory(deleteBtn);
        deleteBtn = 0;
    }

    /* Destroy scrollUpBtn (0x230) */
    if (scrollUpBtn != 0) {
        scrollUpBtn->~T_MenuHotspot();
        FreeMemory(scrollUpBtn);
        scrollUpBtn = 0;
    }

    /* Destroy scrollDownBtn (0x234) */
    if (scrollDownBtn != 0) {
        *(int*)scrollDownBtn = 0x4613D0;
        {
            Sprite* spr = (Sprite*)((int*)scrollDownBtn)[0x24];
            if (spr != 0) {
                delete spr;
                ((int*)scrollDownBtn)[0x24] = 0;
            }
        }
        FreeMemory(scrollDownBtn);
        scrollDownBtn = 0;
    }
}
