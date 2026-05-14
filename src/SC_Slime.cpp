#include "SC_Slime.h"
#include "stubs.h"
#include "globals.h"
#include "InputManager.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "SoundList.h"
#include "Sample.h"
#include "main.h"
#include "Memory.h"
#include "LinkedList.h"
#include <new.h>
#include <string.h>
#include "GameState.h"
#include "MouseControl.h"
#include "RockThrower.h"
#include "CombatWeapon.h"
#include "Projectile.h"
#include "SC_Question.h"
extern "C" void SetVideoRes(int, int);
#include "string.h"
extern "C" void WriteToLog(const char* format, ...);
// g_SlimeField_00468bbc — declared in globals.h

#include "SlimeTable.h"

// TimerWrapper = TimeOut (ctor 0x421920 wraps 0x421960, dtor 0x421930)
#include "TimeOut.h"

/* Function start: 0x40CF60 */
SC_Slime::SC_Slime() {
    int* p = &hitCount;
    p[0] = 0;
    p[1] = 0;
    p = &slimeRound;
    p[0] = 0;
    p[1] = 0;
    memset(&spriteAction, 0, 0xD8);
    handlerId = 0x3F;
}

/* Function start: 0x40D100 */
SC_Slime::~SC_Slime()
{
    ShutDown(0);
}

/* Function start: 0x40D1B0 */
void SC_Slime::Init(SC_MessageParser* msg)
{
    int savedId = handlerId;
    memset(&spriteAction, 0, 0xD8);
    handlerId = savedId;

    if (FileExists("CB_Slime") == 0) {
        ShowLoadingScreen();
    }

    CopyCommandData(msg);

    screenSize.x = 0x140;
    screenSize.y = 0xf0;
    gamePhase = 0;

    SetVideoRes(0x140, 0xf0);

    if (gameResult != 0) {
        delete gameResult;
        gameResult = 0;
    }

    gameResultSize = 7;
    gameResult = new int[7];
    memset(gameResult, 0, gameResultSize * sizeof(int));

    SlimeTable* table = new SlimeTable();
    slimeTable = table;
    table->Allocate(0xC);

    moduleParam = ((SpriteAction*)msg)->addressValue;
    ParseFile(this, "mis\\cb_slime.mis", (char*)0);

    if (spriteAction == 0) {
        SpriteAction* action = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        spriteAction = action;
    }

    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }

    gameResult[0] = 0;
    gameResult[1] = 0;
    gameResult[2] = 0;
    gameResult[3] = 1;
    gameResult[4] = 1;
    gameResult[5] = 1;
    gameResult[6] = 0;

    TimeOut* t = new TimeOut();
    timer = t;
}

/* Function start: 0x40D430 */
int SC_Slime::ShutDown(SC_MessageParser* msg)
{
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }

    if (g_SlimeField_00468bbc != 0) {
        int* obj = (int*)g_SlimeField_00468bbc;
        int* vtbl = (int*)*obj;
        void (__fastcall *fn)(int*, int, int) = (void (__fastcall *)(int*, int, int))vtbl[3];
        fn(obj, 0, 1);
        g_SlimeField_00468bbc = 0;
    }

    if (consoleSprite != 0) {
        delete consoleSprite;
        consoleSprite = 0;
    }
    if (timer != 0) {
        delete (TimeOut*)timer;
        timer = 0;
    }
    if (leftSwitchSprite != 0) {
        delete leftSwitchSprite;
        leftSwitchSprite = 0;
    }
    leftSwitchActive = 0;
    if (rightSwitchSprite != 0) {
        delete rightSwitchSprite;
        rightSwitchSprite = 0;
    }
    rightSwitchActive = 0;
    if (leftArmSprite != 0) {
        delete leftArmSprite;
        leftArmSprite = 0;
    }
    leftArmActive = 0;
    if (rightArmSprite != 0) {
        delete rightArmSprite;
        rightArmSprite = 0;
    }
    rightArmActive = 0;
    if (armMaskSprite != 0) {
        delete armMaskSprite;
        armMaskSprite = 0;
    }
    if (slimeMeterSprite != 0) {
        delete slimeMeterSprite;
        slimeMeterSprite = 0;
    }
    if (spriteAction != 0) {
        delete (SpriteAction*)spriteAction;
        spriteAction = 0;
    }
    if (slimeTable != 0) {
        delete (SlimeTable*)slimeTable;
        slimeTable = 0;
    }
    if (soundList != 0) {
        delete (SoundList*)soundList;
        soundList = 0;
    }
    sound5 = 0;
    sound4 = 0;
    sound3 = 0;
    sound2 = 0;
    sound1 = 0;
    if (gameResult != 0) {
        delete gameResult;
        gameResult = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x40DF30 */
void SC_Slime::ProcessSprite(Sprite* spr) {
    if (spr == 0) {
        goto render_timer;
    }
    if (spr->handle == -1) {
        goto render_timer;
    }

    if (spr == 0) {
        goto render_timer;
    }

    if (spr->Do(spr->loc_x, spr->loc_y, 1.0) == 0) {
        goto render_timer;
    }

    switch (spr->handle) {
    case 0:
        hitCount++;
        {
            int done;
            if (maxHits != 0 && hitCount >= maxHits) {
                done = 1;
            } else {
                done = 0;
            }
            if (done != 0) {
                gameResult[0] = 1;
                goto update_progress;
            }
        }

        if (sound5 != 0) {
            sound5->Play(100, 1);
        }
        consoleSprite->ResetAnimation(8, 0);

    update_progress:
        {
            int progress = (hitCount * 8) / maxHits;
            if (progress >= 7) {
                progress = 7;
            }
            slimeMeterSprite->ResetAnimation(progress, 0);
        }
        spr->ResetAnimation(1, 0);
        break;

    case 1:
    case 2:
        spr->ResetAnimation(-1, 0);
        break;

    default:
        break;
    }

render_timer:
    if (armMaskSprite == 0) {
        return;
    }
    if (armMaskSprite->Do(armMaskSprite->loc_x, armMaskSprite->loc_y, 1.0) == 0) {
        return;
    }
    armMaskSprite->ResetAnimation(-1, 0);
}

/* Function start: 0x40E070 */
void SC_Slime::UpdateArmSprites()
{
    Sprite* spr = leftArmActive;
    int status = spr->handle;
    if (status == 0 ||
        (spr = rightArmActive, status = spr->handle, status == 0)) {
        int height = spr->ranges[status].dim.y;
        height -= *(int*)((char*)spr->animation_data->smk + 0x374);
        spr->ResetAnimation(2, height);
    }
    armMaskSprite->ResetAnimation(-1, 0);
}

/* Function start: 0x40D6C0 */
void SC_Slime::Update(int param1, int param2) {
    if (handlerId == param2) {
        field_174 = 0;
        if (gameResultSize > 0) {
            do {
                int* ptr = gameResult + field_174;
                if (*ptr != 0) {
                    ProcessAction(field_174, ptr);
                }
                field_174++;
            } while (gameResultSize > field_174);
        }
    }
}

/* Function start: 0x40D720 */
int SC_Slime::AddMessage(SC_MessageParser* msg)
{
    WriteMessageAddress(msg);
    if (((SpriteAction*)msg)->lastKey == 0x1b) {
        gameResult[2] = 1;
    }
    return 1;
}

/* Function start: 0x40E0D0 */
void SC_Slime::OnProcessEnd()
{
    if (leftArmActive != 0) {
        leftArmActive->ResetAnimation(-1, 0);
    }
    if (rightArmActive != 0) {
        rightArmActive->ResetAnimation(-1, 0);
    }
    if (armMaskSprite != 0) {
        armMaskSprite->ResetAnimation(-1, 0);
    }

    invSlots[0].left = 0x40;
    invSlots[0].top = 0x18;
    invSlots[0].right = 0x60;
    invSlots[0].bottom = 0x48;
    invSlots[1].left = 0xdd;
    invSlots[1].top = 0x19;
    invSlots[1].right = 0xfd;
    invSlots[1].bottom = 0x49;
    targetRect.left = 0x4c;
    targetRect.top = 0x4d;
    targetRect.right = 0xfb;
    targetRect.bottom = 0xb9;

    extern InputManager* g_InputManager_0046aa08;
    if (g_InputManager_0046aa08 != 0) {
        (g_InputManager_0046aa08)->Refresh(1);
    }

    if (consoleSprite != 0) {
        extern InputManager* g_InputManager_0046aa08;
        int mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        }
        int div = screenSize.x / 5;
        consoleSprite->ResetAnimation(mouseX / div, 0);
    }
}

/* Function start: 0x40E1E0 */
int SC_Slime::LBLParse(char* line) {
    char buffer[128];
    char token[32];
    int index;
    int value;

    index = 0;
    value = 0;
    buffer[0] = 0;
    token[0] = 0;
    sscanf(line, " %s ", token);

    if (strcmp(token, "PALETTE") == 0) {
        sscanf(line, "%s %s", token, buffer);
        palette = new Palette();
        palette->Load(buffer);
    } else if (strcmp(token, "BACKGROUND_SPRITE") == 0) {
        bgSprite = new Sprite((char*)0);
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(token, "CONSOLE_SPRITE") == 0) {
        consoleSprite = new Sprite((char*)0);
        Parser::ProcessFile(consoleSprite, this, (char*)0);
    } else if (strcmp(token, "LEFT_ARM_SPRITE") == 0) {
        leftArmSprite = new Sprite((char*)0);
        leftArmActive = leftArmSprite;
        Parser::ProcessFile(leftArmSprite, this, (char*)0);
    } else if (strcmp(token, "RIGHT_ARM_SPRITE") == 0) {
        rightArmSprite = new Sprite((char*)0);
        rightArmActive = rightArmSprite;
        Parser::ProcessFile(rightArmSprite, this, (char*)0);
    } else if (strcmp(token, "LEFT_SWITCH_SPRITE") == 0) {
        leftSwitchSprite = new Sprite((char*)0);
        leftSwitchActive = (int)leftSwitchSprite;
        Parser::ProcessFile(leftSwitchSprite, this, (char*)0);
    } else if (strcmp(token, "RIGHT_SWITCH_SPRITE") == 0) {
        rightSwitchSprite = new Sprite((char*)0);
        rightSwitchActive = (int)rightSwitchSprite;
        Parser::ProcessFile(rightSwitchSprite, this, (char*)0);
    } else if (strcmp(token, "ARM_MASK_SPRITE") == 0) {
        armMaskSprite = new Sprite((char*)0);
        Parser::ProcessFile(armMaskSprite, this, (char*)0);
    } else if (strcmp(token, "SLIME_METER_SPRITE") == 0) {
        slimeMeterSprite = new Sprite((char*)0);
        Parser::ProcessFile(slimeMeterSprite, this, (char*)0);
    } else if (strcmp(token, "STUDENT_HITS_ALLOWED") == 0) {
        sscanf(line, " %s %d", token, &maxHits);
    } else if (strcmp(token, "CLOSED_SHOTS_NEEDED") == 0) {
        sscanf(line, " %s %d", token, &index);
        closedShots[1].max = index;
        closedShots[0].max = index;
    } else if (strcmp(token, "OPENED_SHOTS_NEEDED") == 0) {
        sscanf(line, " %s %d", token, &index);
        openedShots[1].max = index;
        openedShots[0].max = index;
    } else if (strcmp(token, "TENTACLE_SHOTS_NEEDED") == 0) {
        sscanf(line, " %s %d", token, &tentacleShotsNeeded);
    } else if (strcmp(token, "SOUND") == 0) {
        int fields = sscanf(line, " %s %d %s %d ", token, &index, buffer, &value);
        if (fields != 4) {
            ReportUnknownLabel("SC_Slime");
        }
        if (soundList == 0) {
            soundList = new SoundList(0xf);
        }
        if (index == 0) {
            SendGameMessage(5, atoi(buffer), handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        } else if (index >= 1 && index <= 5) {
            Sample* sample = (Sample*)soundList->Register(MakeAudioName(buffer));
            if (index == 1) {
                sound1 = (int)sample;
            } else if (index == 2) {
                sound2 = (int)sample;
            } else if (index == 3) {
                sound3 = (int)sample;
            } else if (index == 4) {
                sound4 = (int)sample;
            } else if (index == 5) {
                sound5 = sample;
            }
        } else if (index >= 6 && index <= 0xb) {
            slimeTable->LoadEntry(index, buffer, value);
        } else {
            ReportUnknownLabel("SC_Slime");
        }
    } else if (strcmp(token, "WEAPON") == 0) {
        if (sscanf(line, " %s %s ", token, buffer) == 2) {
            if (strcmp(buffer, "ROCKTHROWER2") == 0) {
                g_SlimeField_00468bbc = (int)new CombatWeapon(this);
            }
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_Slime");
    }
    return 0;
}

/* Function start: 0x40EB60 */
void SC_Slime::ProcessAction(int action, int* data) {
    Sprite* spr = bgSprite;
    int state = spr->handle;

    if ((unsigned int)action > 6) {
        ShowError("SC_Slime::Process_Action - invalid Action=%d, value=%d", action, *data);
        return;
    }

    switch (action) {
    case 0:
        gameResult[1] = 0;
        gameResult[2] = 0;
        gameResult[3] = 0;
        if (*data == 1) {
            ((SoundList*)slimeTable)->Play(8);
            (*data)++;
            gameResult[6] = 0;
        }
        if (((SoundList*)slimeTable)->IsSamplePlaying(8) == 0) {
            SendResultMessage();
        }
        return;

    case 1:
        gameResult[2] = 0;
        gameResult[3] = 0;
        if (*data == 1) {
            ((SoundList*)slimeTable)->Play(0xB);
            ((SoundList*)slimeTable)->Play(7);
            (*data)++;
            gameResult[6] = 0;
            gameResult[5] = 0;
        }
        if (*data == 2 && ((SoundList*)slimeTable)->IsSamplePlaying(7) == 0) {
            (*data)++;
            gamePhase |= 1;
        }
        if (gamePhase == 3) {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindLabel("COMBAT_SLIME_WON");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 1;
            SendResultMessage();
            return;
        }
        return;

    case 2:
        gameResult[3] = 0;
        if (*data == 1) {
            ((SoundList*)slimeTable)->Play(9);
            (*data)++;
            gameResult[6] = 0;
            UpdateArmSprites();
        }
        if (((SoundList*)slimeTable)->IsSamplePlaying(9) == 0) {
            SendResultMessage();
        }
        return;

    case 3:
        if (*data == 1) {
            ((SoundList*)slimeTable)->Play(6);
            (*data)++;
        }
        if (((SoundList*)slimeTable)->IsSamplePlaying(6) == 0) {
            *data = 0;
            gameResult[6] = 1;
        }
        return;

    case 4:
        if (spr->Do(spr->loc_x, spr->loc_y, 1.0) != 0) {
            if (state == 0) {
                spr->ResetAnimation(1, 0);
            } else if (state == 1) {
                spr->ResetAnimation(0, 0);
                int r = rand() & 1;
                if (r < 2 && gameResult[6] != 0) {
                    Sprite* arm = (&leftArmSprite)[r];
                    if (arm != 0) {
                        arm->ResetAnimation(0, 0);
                        if (armMaskSprite != 0) {
                            armMaskSprite->ResetAnimation(r, 0);
                        }
                    }
                }
            } else if (state == 2) {
                gamePhase |= 2;
                spr->ResetAnimation(3, 0);
            }
        }
        return;

    case 5:
        CheckTimerExpired((Sprite*)leftSwitchActive);
        CheckTimerExpired((Sprite*)rightSwitchActive);
        ProcessSprite(leftArmActive);
        ProcessSprite(rightArmActive);
        return;

    case 6:
        if (g_Mouse_0046aa18 != 0) {
            g_Mouse_0046aa18->DrawCursor();
        }
        {
            Weapon* weapon = (Weapon*)g_SlimeField_00468bbc;
            if (weapon != 0 && consoleSprite != 0) {
                weapon->UpdateProjectiles();
                if (((int*)weapon)[0x2A] != 0) {
                    int val = 0;
                    if (g_InputManager_0046aa08->pMouse != 0) {
                        val = g_InputManager_0046aa08->pMouse->x;
                    }
                    consoleSprite->ResetAnimation(val / (screenSize.x / 3) + 5, 0);
                }
                if (consoleSprite->Do(consoleSprite->loc_x, consoleSprite->loc_y, 1.0) != 0) {
                    int val = 0;
                    if (g_InputManager_0046aa08->pMouse != 0) {
                        val = g_InputManager_0046aa08->pMouse->x;
                    }
                    consoleSprite->ResetAnimation(val / (screenSize.x / 5), 0);
                }
            }
        }
        slimeMeterSprite->Do(slimeMeterSprite->loc_x, slimeMeterSprite->loc_y, 1.0);
        return;
    }
}

/* Function start: 0x40D750 */
int SC_Slime::Exit(SC_MessageParser* msg)
{
    int id = handlerId;
    SpriteAction* a = (SpriteAction*)msg;
    if (a->addressType != id) {
        return 0;
    }

    int action = a->instruction;
    switch (action) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, id, id, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x40D7C0 */
void SC_Slime::SendResultMessage() {
    int idx;
    int* gr;
    if (savedCommand != 0x2B) {
        // Non-practice mode
        gr = (int*)gameResult;
        if (gr[0] != 0) {
            // Won — send room switch + add 20 to NUM_ACTIONS
            ((int*)spriteAction)[0] = 0x2C;
            ((int*)spriteAction)[1] = 1;
            goto enqueue;
        }
        if (gr[2] != 0) {
            // Alternate win — send room switch + add 20 to NUM_ACTIONS
            ((int*)spriteAction)[0] = 0x20;
            ((int*)spriteAction)[1] = 2;
            {
                GameState* gs = g_GameState_0046aa30;
                idx = gs->FindState("NUM_ACTIONS");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] += 0x14;
            }
            goto enqueue;
        }
        // Lost — add 30 to NUM_ACTIONS
        {
            GameState* gs = g_GameState_0046aa30;
            idx = gs->FindState("NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x1E;
        }

    enqueue:
        EnqueueSpriteAction((void*)spriteAction);
        if (spriteAction != 0) {
            delete (SpriteAction*)spriteAction;
            spriteAction = 0;
        }
        return;
    }

    // Practice mode (savedCommand == 0x2B)
    gr = (int*)gameResult;
    if (gr[1] != 0) {
        // Lost in practice
        if (spriteAction != 0) {
            delete (SpriteAction*)spriteAction;
            spriteAction = 0;
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        spriteAction = action;
        SC_MessageParser temp;
        ParseFile(&temp, "mis\\cb_slime.mis", "[WIN_LBL_PR]");
        goto enqueue;
    }

    if (gr[0] != 0) {
        // Won in practice
        if (spriteAction != 0) {
            delete (SpriteAction*)spriteAction;
            spriteAction = 0;
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        spriteAction = action;
        SC_MessageParser temp;
        ParseFile(&temp, "mis\\cb_slime.mis", "[LOSE_LBL_PR]");
        goto enqueue;
    }

    goto enqueue;
}

/* Function start: 0x40DB00 */
void SC_Slime::ProcessHit() {
}

/* Function start: 0x40DB20 */
int SC_Slime::HandleInput(Sprite* spr) {
    int* piVar1;
    void* pvVar2;
    bool bVar3;
    unsigned int uVar4;
    int* piVar5;
    int iVar6;
    int iVar7;
    int iVar8;
    unsigned int uVar9;

    piVar1 = (int*)((char*)spr + 0x120);
    iVar6 = *(int*)((char*)spr->ranges + spr->handle * 0x10 + 4) - 1;
    iVar8 = piVar1[1];
    if ((iVar8 <= 0x4f) || (*(int*)((char*)this + 0x128) == 0)) {
        if (*(int*)((char*)spr + 0xf0) == 0) {
            if (iVar6 != 0) goto LAB_0040dba1;
        } else if (*(int*)(*(int*)(*(int*)((char*)spr + 0xf0) + 0xc) + 0x374) != iVar6) {
            goto LAB_0040dba1;
        }
        if (*(int*)((char*)this + 0xc4) != 0) {
            iVar7 = 0;
            piVar5 = (int*)((char*)this + 0xd8);
            do {
                if ((((*piVar1 < *piVar5) || (piVar5[2] < *piVar1)) || (iVar8 < piVar5[1])) ||
                    (bVar3 = true, piVar5[3] < iVar8)) {
                    bVar3 = false;
                }
                if (bVar3) {
                    iVar8 = *(int*)(*(int*)((char*)this + 0xc4) + 0xf0);
                    if (iVar8 == 0) {
                        iVar8 = *(int*)((char*)g_SlimeHitTable_00468bc0 + iVar7 * 0xf4);
                    } else {
                        iVar8 = *(int*)((char*)g_SlimeHitTable_00468bc0 +
                            (*(int*)(*(int*)(iVar8 + 0xc) + 0x374) + iVar7 * 0x3d) * 4);
                    }
                    if (iVar8 != 0) {
                        if (*(void**)((char*)this + 0x164) != 0) {
                            ((Sample*)*(void**)((char*)this + 0x164))->Play(100, 1);
                            return 1;
                        }
                        goto LAB_0040ddfc;
                    }
                    pvVar2 = *(void**)((char*)this + iVar7 * 4 + 0xc8);
                    if (pvVar2 != 0) {
                        iVar6 = *(int*)((char*)pvVar2 + 0x98);
                        switch (iVar6) {
                        case 0:
                            iVar8 = *(int*)((char*)this + iVar7 * 8 + 0xf8) + 1;
                            *(int*)((char*)this + iVar7 * 8 + 0xf8) = iVar8;
                            iVar6 = *(int*)((char*)this + iVar7 * 8 + 0xfc);
                            if ((iVar6 == 0) || (bVar3 = true, iVar8 < iVar6)) {
                                bVar3 = false;
                            }
                            if (!bVar3) goto LAB_0040ddcf;
                            iVar6 = 2;
                            goto LAB_0040ddd7;
                        case 1:
                        case 3:
                            break;
                        case 2:
                            iVar8 = *(int*)((char*)this + iVar7 * 8 + 0x108) + 1;
                            *(int*)((char*)this + iVar7 * 8 + 0x108) = iVar8;
                            iVar6 = *(int*)((char*)this + iVar7 * 8 + 0x10c);
                            if ((iVar6 == 0) || (bVar3 = true, iVar8 < iVar6)) {
                                bVar3 = false;
                            }
                            if (bVar3) {
                                ((Sprite*)pvVar2)->ResetAnimation(4, 0);
                                if (*(int*)(*(int*)((char*)this + iVar7 * -4 + 0xcc) + 0x98) == 4) {
                                    UpdateArmSprites();
                                }
                                goto switchD_0040dd45_default;
                            }
LAB_0040ddcf:
                            iVar6 = *(int*)((char*)pvVar2 + 0x98);
                            break;
                        default:
                            goto switchD_0040dd45_default;
                        }
                        iVar6 = iVar6 + 1;
LAB_0040ddd7:
                        ((Sprite*)pvVar2)->ResetAnimation(iVar6, 0);
switchD_0040dd45_default:
                        if (*(void**)((char*)this + 0x160) != 0) {
                            ((Sample*)*(void**)((char*)this + 0x160))->Play(100, 1);
                        }
                        ((SoundList*)*(void**)((char*)this + 0x16c))->Play(10);
LAB_0040ddfc:
                        return 1;
                    }
                    break;
                }
                piVar5 = piVar5 + 4;
                iVar7 = iVar7 + 1;
            } while (iVar7 < 2);
        }
    } else {
        uVar9 = 1;
        do {
            pvVar2 = *(void**)((char*)this + uVar9 * 4 + 0x114);
            if ((*(int*)((char*)pvVar2 + 0x98) == 0) &&
                (uVar4 = ((VBuffer*)*(void**)(*(int*)((char*)*(void**)((char*)this + 0x128) + 0xf0) + 0x18))->CheckHit(
                    *piVar1, piVar1[1] - 0x4f), uVar9 == uVar4)) {
                iVar6 = *(int*)((char*)this + 0x138) + 1;
                *(int*)((char*)this + 0x138) = iVar6;
                if ((*(int*)((char*)this + 0x13c) == 0) ||
                    (bVar3 = true, iVar6 < *(int*)((char*)this + 0x13c))) {
                    bVar3 = false;
                }
                if (bVar3) {
                    ((Sprite*)pvVar2)->ResetAnimation(2,
                        *(int*)((char*)((Sprite*)pvVar2)->ranges + (*(int*)((char*)pvVar2 + 0x98) * 0x10) + 4) -
                        *(int*)(*(int*)(*(int*)((char*)pvVar2 + 0xf0) + 0xc) + 0x374));
                    ((Sprite*)*(void**)((char*)this + 0x128))->ResetAnimation(-1, 0);
                }
                if (*(void**)((char*)this + 0x164) != 0) {
                    ((Sample*)*(void**)((char*)this + 0x164))->Play(100, 1);
                }
                return 1;
            }
            uVar9 = uVar9 + 1;
        } while ((int)uVar9 <= 2);
    }

LAB_0040dba1:
    if (*(int*)((char*)spr + 0xf0) == 0) {
        if (iVar6 != 0) {
            goto LAB_0040de8e;
        }
    } else if (*(int*)(*(int*)(*(int*)((char*)spr + 0xf0) + 0xc) + 0x374) != iVar6) {
        goto LAB_0040de8e;
    }
    if ((((*piVar1 < *(int*)((char*)this + 0x140)) || (*(int*)((char*)this + 0x148) < *piVar1)) ||
        (piVar1[1] < *(int*)((char*)this + 0x144))) ||
        (bVar3 = true, *(int*)((char*)this + 0x14c) < piVar1[1])) {
        bVar3 = false;
    }
    if (bVar3) {
        if (*(void**)((char*)this + 0x164) != 0) {
            ((Sample*)*(void**)((char*)this + 0x164))->Play(100, 1);
            return 1;
        }
        goto LAB_0040de7f;
    } else if (*(void**)((char*)this + 0x15c) != 0) {
        ((Sample*)*(void**)((char*)this + 0x15c))->Play(100, 1);
    }
LAB_0040de7f:
    return 1;
LAB_0040de8e:
    return 0;
}

/* Function start: 0x40DEB0 */
void SC_Slime::CheckTimerExpired(Sprite* spr)
{
    if (spr == 0) {
        return;
    }

    if (spr->Do(spr->loc_x, spr->loc_y, 1.0) == 0) {
        return;
    }

    if (spr->handle != 4) {
        return;
    }

    if (((Sprite*)leftSwitchActive)->handle != 4) {
        return;
    }

    if (((Sprite*)rightSwitchActive)->handle != 4) {
        return;
    }

    bgSprite->ResetAnimation(2, 0);
    gameResult[1] = 1;
}

/* Function start: 0x40D0D0 */
SlotPair::SlotPair() { count = 0; max = 0; }

// g_SlimeField_00468bbc — defined in globals.cpp

/* Function start: 0x425480 */
SlimeTable::SlimeTable()
{
    fields[0] = 0;
    fields[1] = 0;
    fields[2] = 0;
}

/* Function start: 0x425490 */
SlimeTable::~SlimeTable()
{
    Cleanup();
}

/* Function start: 0x425550 */
int SlimeTable::Play(int index)
{
    if (index < 0 || fields[0] - 1 < index) {
        return 0;
    }
    if (((int*)fields[2])[index] != 0) {
        int i = 0;
        if (fields[0] > 0) {
            do {
                if (((int*)fields[2])[i] == ((int*)fields[2])[index] && ((Sample**)fields[1])[i] != 0) {
                    ((Sample**)fields[1])[i]->Stop();
                }
                i++;
            } while (fields[0] > i);
        }
    }
    Sample* smp = ((Sample**)fields[1])[index];
    if (smp != 0) {
        smp->Play(100, 1);
    }
    return smp != 0;
}

/* Function start: 0x425620 */
void SlimeTable::LoadEntry(int index, char* filename, int value)
{
    if (index < 0 || fields[0] - 1 < index) {
        return;
    }

    Sample* sample = new Sample();
    ((void**)fields[1])[index] = sample;
    sample->Load(MakeAudioName(filename));
    ((int*)fields[2])[index] = value;
}

/* Function start: 0x4254F0 */
void SlimeTable::Cleanup()
{
    int i;

    for (i = 0; i < fields[0]; i++) {
        void* entry = ((void**)fields[1])[i];
        if (entry != 0) {
            ((Sample*)entry)->Unload();
            operator delete(entry);
            ((void**)fields[1])[i] = 0;
        }
    }

    if ((void*)fields[1] != 0) {
        operator delete((void*)fields[1]);
        fields[1] = 0;
    }

    fields[0] = 0;
}

/* Function start: 0x4254A0 */
void SlimeTable::Allocate(int count)
{
    Cleanup();
    fields[0] = count;
    fields[1] = (int)operator new(count * 4);
    memset((void*)fields[1], 0, fields[0] * 4);

    fields[2] = (int)operator new(fields[0] * 4);
    memset((void*)fields[2], 0, fields[0] * 4);
}
