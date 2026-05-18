#ifndef MAIN_H
#define MAIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function declarations from main.cpp */

int ProcessMessages(void);
int InitGraphics(void);
int FileExists(const char* filename);
void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext);
void CheckDebug(void);
unsigned int CalculateBufferSize(unsigned int width, unsigned int height);
void InitGameSystems(void);
void ShowLoadingScreen(void);
void SetVideoRes(int width, int height);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void ShutdownGameSystems(void);
void CleanupCinematic(void);
void PlayIntroCinematic(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
