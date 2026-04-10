#ifndef MAIN_H
#define MAIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function declarations from main.cpp */

int ProcessMessages(void);
int InitGraphics(void);
int GetFileAttributes_Wrapper(const char* filename, char param_2);
void SetErrorCode(unsigned int errorCode);
int FileExists(const char* filename);
int CheckFileOnDrive(void* this_ptr, int drive_letter);
int ChangeToDriveDirectory(void* this_ptr, int drive_letter);
int ChangeDirectory(void* this_ptr, unsigned char* path);
void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext);
void CheckDebug(void);
unsigned int CalculateBufferSize(unsigned int width, unsigned int height);
void InitGameSystems(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void ShutdownGameSystems(void);
void CleanupCinematic(void);
void PlayIntroCinematic(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
