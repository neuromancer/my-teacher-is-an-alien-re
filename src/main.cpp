#include <windows.h>
#include "Animation.h"
#include "GameState.h"
#include "Sound.h"
#include "VBuffer.h"
#include "GameWindow.h"
#include "Memory.h"
#include "AnimatedAsset.h"
#include "string.h"
#include <mbstring.h>
#include <mbctype.h>

extern unsigned char __mbctype[];

extern AnimatedAsset* AnimatedAsset_Ctor(AnimatedAsset*);

Sound* g_sound;

GameState* g_GameState = (GameState*)0x00436998;
extern int DAT_004373bc;
extern int DAT_00437f4c;
extern char DAT_0043d568;
extern int _DAT_0043d564;
extern int _DAT_0043bdf0;
extern int DAT_0043bdf4;
extern unsigned int DAT_0043c760;
extern unsigned int DAT_0043c8c8;
extern int FUN_00422510();

#include "globals.h"

VBuffer *g_WorkBuffer = (VBuffer*)0x00436974;
void *DAT_0043696c = (void*)0x0043696c;
void *DAT_00436968 = (void*)0x00436968;
void *DAT_00436970 = (void*)0x00436970;
char *DAT_0043697c = (char*)0x0043697c;
void *DAT_00436964 = (void*)0x00436964;
char *DAT_00436960 = (char*)0x00436960;

extern "C" {
	void FUN_00421010(void*);
	void FUN_004227a0(void*);
	void FUN_00421ea0(void*);
	void PlayIntroCinematic();
	void FUN_0040c5d0();
	void FUN_00422430(void*);
    unsigned int CalculateBufferSize(unsigned int width, unsigned int height);
    void CheckDebug();
    void ClearMessageLog();
    void CreateGameObject_1();
    void InitWorkBuffer(int, int);
    void* JoystickManager_Constructor(void*, int);
    void* Sound_Init(void*, int, int, int);
    void SetStateFlag(int, int);
    void SetCursorVisible(int);
    void* FUN_00421e40(void*, char*, void*);
    int FileExists(const char*);
    int CheckFileOnDrive(void*, int);
    int ChangeToDriveDirectory(void*, int);
    int ChangeDirectory(void*, unsigned char*);
    void ShowError(const char*, ...);
    void SetErrorCode(unsigned int);
    void ParsePath(const char*, char*, char*, char*, char*);
    int _chdir(const char*);
}

void _AIL_shutdown_0();
void FUN_0041e310();
void FUN_00421840();

extern HWND DAT_0043de7c;
extern int DAT_0043d55c;


/* Function start: 0x4192F0 */
int ProcessMessages()
{
    MSG local_1c;
    int iVar1;

    do {
        iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
        while (iVar1 != 0) {
            TranslateMessage(&local_1c);
            if (local_1c.message == WM_KEYDOWN) {
                DAT_004373bc = local_1c.wParam;
                if (0x7f < local_1c.wParam) {
                    DAT_004373bc = local_1c.wParam & 0x2f;
                }
                if (local_1c.wParam == VK_F12) {
                    return 1;
                }
            }
            DispatchMessageA(&local_1c);
            iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
        }
        iVar1 = FUN_00422510();
        if (iVar1 != 0) {
            return 0;
        }
    } while (1);
}

extern "C" {
    int FUN_00423cd9();
    void FUN_00419170(char*);
    void FUN_00423aac();
    void FUN_00423a54();
    void FUN_00422d98(int);
}

/* Function start: 0x4223F0 */
int InitGraphics(void)
{
    if (FUN_00423cd9() != 8) {
        FUN_00419170("For Optimum performance, please set your display to 256 colors.");
    }
    FUN_00423aac();
    FUN_00423a54();
    FUN_00422d98(0);
    return 1;
}

/* Function start: 0x430310 */
int GetFileAttributes_Wrapper(const char* param_1, char param_2)
{
    int DVar1;

    DVar1 = GetFileAttributesA(param_1);
    if (DVar1 == -1) {
        DVar1 = GetLastError();
        SetErrorCode(DVar1);
        return -1;
    }
    if (((DVar1 & 1) != 0) && ((param_2 & 2) != 0)) {
        _DAT_0043bdf0 = 0xd;
        DAT_0043bdf4 = 5;
        return -1;
    }
    return 0;
}

/* Function start: 0x42B300 */
void SetErrorCode(unsigned int errorCode)
{
    int iVar1;
    unsigned int* puVar2;

    iVar1 = 0;
    puVar2 = &DAT_0043c760;
    DAT_0043bdf4 = errorCode;
    do {
        if (*puVar2 == errorCode) {
            _DAT_0043bdf0 = *(int*)(iVar1 * 8 + 0x43c764);
            return;
        }
        puVar2 = puVar2 + 2;
        iVar1 = iVar1 + 1;
    } while (puVar2 < &DAT_0043c8c8);
    if ((0x12 < errorCode) && (errorCode < 0x25)) {
        _DAT_0043bdf0 = 0xd;
        return;
    }
    if ((0xbb < errorCode) && (errorCode < 0xcb)) {
        _DAT_0043bdf0 = 8;
        return;
    }
    _DAT_0043bdf0 = 0x16;
    return;
}


/* Function start: 0x4195A0 */
int FileExists(const char* filename)
{
    return GetFileAttributesA(filename) != -1;
}

/* Function start: 0x421EB0 */
int CheckFileOnDrive(void* this_ptr, int drive_letter)
{
    char local_40[64];

    sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40, (char*)((int)this_ptr + 0x80), (char*)((int)this_ptr + 0x1c5));
    return FileExists(local_40);
}

/* Function start: 0x421F40 */
int ChangeToDriveDirectory(void* this_ptr, int drive_letter)
{
    char local_40[64];

    sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40, (char*)((int)this_ptr + 0x80), (char*)((int)this_ptr + 0x1c5));
    return 1 - (ChangeDirectory(this_ptr, (unsigned char*)local_40) == 0);
}

/* Function start: 0x421EF0 */
int ChangeDirectory(void* this_ptr, unsigned char* path)
{
    if (path != 0 && *path != 0) {
        if (_chdir((char*)path) != 0) {
            return 1;
        }
        ParsePath((char*)path, (char*)((int)this_ptr + 0xc0), 0, 0, 0);
    }
    return 0;
}

/* Function start: 0x4261C0 */
void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext)
{
    const char* pbVar3;
    const char* _Source = 0;
    const char* local_4 = 0;
    size_t sVar2;

    if (path[1] == ':') {
        if (drive) {
            _mbsnbcpy((unsigned char*)drive, (unsigned char*)path, 2);
            drive[2] = '\0';
        }
        path += 2;
    }
    else if (drive) {
        *drive = '\0';
    }

    pbVar3 = path;
    while (*pbVar3 != 0) {
        char bVar1 = *pbVar3;
        if ((__mbctype[(unsigned char)bVar1 + 1] & 4) == 0) {
            if (bVar1 == '\\' || bVar1 == '/') {
                _Source = pbVar3 + 1;
            }
            else if (bVar1 == '.') {
                local_4 = pbVar3;
            }
        }
        else {
            pbVar3++;
        }
        pbVar3++;
    }

    if (_Source) {
        if (dir) {
            sVar2 = _Source - path;
            if (sVar2 > 255) {
                sVar2 = 255;
            }
            _mbsnbcpy((unsigned char*)dir, (unsigned char*)path, sVar2);
            dir[sVar2] = '\0';
        }
        path = _Source;
    }
    else if (dir) {
        *dir = '\0';
    }

    if (local_4 && local_4 > path) {
        if (fname) {
            sVar2 = local_4 - path;
            if (sVar2 > 255) {
                sVar2 = 255;
            }
            _mbsnbcpy((unsigned char*)fname, (unsigned char*)path, sVar2);
            fname[sVar2] = '\0';
        }
        if (ext) {
            strcpy(ext, local_4);
        }
    }
    else {
        if (fname) {
            strcpy(fname, path);
        }
        if (ext) {
            *ext = '\0';
        }
    }
}

/* Function start: 0x41A670 */
void CheckDebug(void)
{
    char local_94[128];
    void* pvVar2;

    __try {
        pvVar2 = DAT_0043697c;
        if (DAT_0043697c == (char *)0x0) {
            void* local_14 = AllocateMemory(0x1e5);
            if (local_14 != (void *)0x0) {
                pvVar2 = FUN_00421e40(local_14, "cddata", &DAT_0043d568);
            }
        }
        DAT_0043697c = (char*)pvVar2;
        if (DAT_0043d568 == '\0') {
            if (!FileExists((char*)((int)pvVar2 + 0x1c5)) && !FileExists("Develop")) {
                int i = 3;
                for (; i < 0x1a; i++) {
                    if (CheckFileOnDrive(DAT_0043697c, i)) {
                        if (ChangeToDriveDirectory(DAT_0043697c, i)) {
                            ShowError("Invalid CD directory");
                        }
                        break;
                    }
                }
                if (0x18 < i) {
                    ShowError("Missing the Teacher CD-ROM");
                }
            }
            else {
                _DAT_0043d564 = 1;
                if (ChangeDirectory(DAT_0043697c, (unsigned char*)((int)DAT_0043697c + 0x1c5))) {
                    ShowError("Invalid Development directory");
                }
            }
        }
        else {
            sprintf(local_94, "%s\\%s", &DAT_0043d568, (char*)((int)pvVar2 + 0x1c5));
            if (!FileExists(local_94)) {
                ShowError("Invalid CD path specified on command line");
            }
            ChangeDirectory(DAT_0043697c, (unsigned char*)local_94);
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

/* Function start: 0x422E02 */
unsigned int CalculateBufferSize(unsigned int width, unsigned int height)
{
    return (((width + 3) & ~3U) * height) + DAT_00437f4c;
}

/* Function start: 0x41A3D0 */
void InitGameSystems(void)
{
    DAT_00436960 = (char*)AllocateMemory(0x100);
    DAT_00436964 = AllocateMemory(CalculateBufferSize(0x280,0x1e0));
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitWorkBuffer(0x280,0x1e0);
    void* pJoystickManager = AllocateMemory(0x1b8);
    if (pJoystickManager != (void *)0x0) {
        DAT_00436968 = JoystickManager_Constructor(pJoystickManager, *(char*)((int)DAT_00436970 + 0x44));
    }
    void* pSound = AllocateMemory(0x3c);
    if (pSound != (void *)0x0) {
        DAT_0043696c = Sound_Init(pSound,0x5622,8,1);
    }
    AnimatedAsset* pTextManager = (AnimatedAsset*)AllocateMemory(0x38);
    AnimatedAsset* pTextManagerInit = (AnimatedAsset*)0x0;
    if (pTextManager != (AnimatedAsset *)0x0) {
        pTextManagerInit = AnimatedAsset_Ctor(pTextManager);
    }
    g_TextManager_00436990 = pTextManagerInit;
    g_TextManager_00436990->LoadAnimatedAsset("elements\\barrel06.smk");
    SetStateFlag(0,1);
    SetCursorVisible(0);
}

/* Function start: 0x422520 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
  GameWindow gameWindow;
  gameWindow.CreateGameWindow(hInstance, nCmdShow, lpCmdLine, 0);
  if (InitGraphics() == 0) {
    return 0;
  }
  UpdateWindow(DAT_0043de7c);
  if (DAT_0043d55c == 0) {
    PlayIntroCinematic();
  }
  else {
    FUN_0040c5d0();
  }
  FUN_00422430(&gameWindow);
  return 1;
}

/* Function start: 0x41A550 */
void ShutdownGameSystems(void)
{
  if (g_TextManager_00436990 != 0) {
    FUN_00421010(g_TextManager_00436990);
    FreeMemory(g_TextManager_00436990);
    g_TextManager_00436990 = 0;
  }
  if (g_WorkBuffer != 0) {
    g_WorkBuffer->~VBuffer();
    FreeMemory(g_WorkBuffer);
    g_WorkBuffer = 0;
  }
  if (DAT_0043696c != 0) {
    _AIL_shutdown_0();
    FreeMemory(DAT_0043696c);
    DAT_0043696c = 0;
  }
  if (DAT_00436968 != 0) {
    FUN_00421840();
    FreeMemory(DAT_00436968);
    DAT_00436968 = 0;
  }
  if (DAT_00436970 != 0) {
    FUN_004227a0(DAT_00436970);
    FreeMemory(DAT_00436970);
    DAT_00436970 = 0;
  }
  if (DAT_0043697c != 0) {
    FUN_00421ea0(DAT_0043697c);
    FreeMemory(DAT_0043697c);
    DAT_0043697c = 0;
  }
  if (DAT_00436964 != 0) {
    FreeMemory(DAT_00436964);
    DAT_00436964 = 0;
  }
    if (DAT_00436960 != 0) {
    FreeMemory(DAT_00436960);
    DAT_00436960 = 0;
  }
}

/* Function start: 0x40d28c */
void CleanupCinematic(void)
{
}

/* Function start: 0x40d230 */
void PlayIntroCinematic(void)
{
    InitGameSystems();
    Animation anim;
    anim.Play("cine\\cine0001.smk", 0x20);
    ShutdownGameSystems();
}
