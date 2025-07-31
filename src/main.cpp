#include <windows.h>
#include "GameState.h"
#include "Sound.h"

Sound* g_sound;

GameState* g_GameState = (GameState*)0x00436998;
extern int DAT_004373bc;
extern int FUN_00422510();

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
