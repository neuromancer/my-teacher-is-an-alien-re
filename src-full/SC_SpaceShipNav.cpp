#include "SC_SpaceShipNav.h"
#include "SC_CombatBase.h"
#include "Memory.h"
#include "Sprite.h"
#include "Animation.h"
#include "mCNavigator.h"
#include "MouseControl.h"
#include "InputManager.h"
#include "SoundList.h"

extern MouseControl* g_Mouse_0046aa18;
extern InputManager* g_InputManager_0046aa08;

extern int g_CombatEngine_0046ae78;

/* Function start: 0x447520 */
SC_SpaceShipNav::SC_SpaceShipNav() {
}

/* Function start: 0x4475B0 */
SC_SpaceShipNav::~SC_SpaceShipNav() {
    ShutDown(0);

    if (field_118[0] != 0) {
        int* obj = (int*)field_118[0];
        int* vtbl = (int*)*obj;
        ((void (__fastcall *)(int*, int, int))vtbl[3])(obj, 0, 1);
        field_118[0] = 0;
    }
}

/* Function start: 0x447790 */
int SC_SpaceShipNav::ShutDown(SC_Message* msg) {
    if (g_CombatEngine_0046ae78 != 0) {
        if (g_Navigator_0046ae70 != 0) {
            Sprite* spr = ((mCNavigator*)g_Navigator_0046ae70)->sprite;
            if (spr != 0) {
                spr->StopAnimationSound();
            }
        }
        ((SC_CombatBase*)g_CombatEngine_0046ae78)->RenderState();
        g_CombatEngine_0046ae78 = 0;
    }
    return SC_Combat::ShutDown(msg);
}

extern int FUN_0044be70(void*);
extern int DAT_0046c3f0[];

/* Function start: 0x447950 */
void SC_SpaceShipNav::UpdateCursor() {
    int iVar1 = FUN_0044be70(g_Navigator_0046ae70);
    if (iVar1 == 0 && g_Mouse_0046aa18->m_sprite != 0 &&
        g_Mouse_0046aa18->m_sprite->handle != -1) {
        int mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x * 3;
        }
        iVar1 = mouseX / *(int*)((int)this + 0xB8);
        if (iVar1 < 0) iVar1 = 0;
        else if (iVar1 > 2) iVar1 = 2;
        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(DAT_0046c3f0[iVar1], 0);
        }
        g_Mouse_0046aa18->DrawCursor();
    }
    if (*(int*)((int)g_Navigator_0046ae70 + 0xA0) != 0) {
        Animation* anim = *(Animation**)((int)g_Navigator_0046ae70 + 0xA0 + 0x50);
        int frameNum = 0;
        if (anim != 0) {
            frameNum = anim->smk->FrameNum;
        }
        switch (frameNum) {
        case 0xA3:
        case 0xC6:
        case 0xE9:
        case 299:
            bgSound->Play(0);
            return;
        case 0x122:
            bgSound->Play(1);
            return;
        }
    }
}

/* Function start: 0x447B60 */
int SC_SpaceShipNav::LBLParse(char* line) {
    char keyword[32];
    keyword[0] = 0;
    sscanf(line, "%s", keyword);

    if (strcmp(keyword, "NAVIGATION") == 0) {
        Parser::ProcessFile((Parser*)g_Navigator_0046ae70, this, 0);
        return 0;
    }
    if (strcmp(keyword, "END") == 0) {
        return 1;
    }
    SC_Combat::LBLParse(line);
    return 0;
}
