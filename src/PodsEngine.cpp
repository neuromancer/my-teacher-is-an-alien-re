#include "PodsEngine.h"
#include "stubs.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include "HashTable.h"
#include "SoundList.h"
#include "ScoreDisplay.h"
#include "EngineSubsystems.h"
#include "VBuffer.h"
#include "Animation.h"
#include "Target.h"
#include "CombatSprite.h"
#include "mCNavigator.h"
#include "RockThrower.h"
#include "InputManager.h"
#include "GameState.h"
#include "SlimeTable.h"
#include "globals.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SC_CombatBase.h"
void StartScheduleTimer();
extern char* MakeSoundName(char*);

/* Function start: 0x440860 */
PodsEngine::PodsEngine()
{
    memset(&podsBgSprite, 0, 10 * sizeof(int));
    counter.x = 0;
    counter.y = 3;
}

/* Function start: 0x440950 */
PodsEngine::~PodsEngine()
{
    if (g_PodsPalette_0046bf30 != 0) {
        delete g_PodsPalette_0046bf30;
        g_PodsPalette_0046bf30 = 0;
    }
    if (podsBgSprite != 0) {
        delete podsBgSprite;
        podsBgSprite = 0;
    }
    if (hashTable != 0) {
        delete hashTable;
        hashTable = 0;
    }
}

/* Function start: 0x440920 */
int PodsEngine::UpdateSprites()
{
    return 0;
}

/* Function start: 0x440A50 */
void PodsEngine::ProcessFrame() {
    unsigned int flags;
    int state;
    int value;
    int mouseX;
    int mouseDown;
    int ready;
    InputState* mouse;
    Target* target;

    flags = field_0x110;
    if ((flags & 4) != 0) {
        if (g_SlimeTable_0046bf28->IsSamplePlaying((~flags) & 1) == 0) {
            field_0x110 &= ~4;
            field_0x110 |= 2;
            if (g_Navigator_0046ae70->startingNode != 2) {
                g_Navigator_0046ae70->SetNavParams(2, 0);
            }
        }
    } else {
        if ((flags & 8) != 0) {
            value = ((flags & 1) == 0) ? 3 : 2;
            if (g_SlimeTable_0046bf28->IsSamplePlaying(value) == 0) {
                StartScheduleTimer();
            }
        }
    }

    if (podsBgSprite != 0) {
        state = podsBgSprite->handle;
        if (reserved_0x104 != g_ScoreDisplay_0046ae6c->hitCount && state != 2) {
            reserved_0x104 = g_ScoreDisplay_0046ae6c->hitCount;
            podsBgSprite->ResetAnimation(1, 0);
            counter.x++;
            if (counter.y == 0) {
                ready = 0;
            } else {
                ready = 1;
                if (counter.x >= counter.y) {
                } else {
                    ready = 0;
                }
            }
            if (ready) {
                field_0x110 |= 0x20;
                if (g_Palette_0046ae64 != 0) {
                    g_Palette_0046ae64->SetPalette(
                        g_WeaponParser_0046ae4c->paletteStart.x,
                        (g_WeaponParser_0046ae4c->paletteStart.y - g_WeaponParser_0046ae4c->paletteStart.x) + 1);
                }
                if (targetList != 0) {
                    delete targetList;
                    targetList = 0;
                }
                g_TargetList_0046ae58 = 0;
            }
        }

        if (podsBgSprite->Do(podsBgSprite->loc.x, podsBgSprite->loc.y, 1.0) != 0) {
            switch (state) {
            case 1: {
                unsigned int* p = (unsigned int*)&podRange.x;
                int count = (podRange.y - *p) + 1;
                if (g_PodsPalette_0046bf30 == 0) {
                    g_PodsPalette_0046bf30 = new Palette();
                    g_PodsPalette_0046bf30->CopyEntries(0, 0x100);
                }
                {
                    Palette temp;
                    temp.CopyRGBData((int)g_PodsPalette_0046bf30->m_data, *p, count);
                    temp.FadeTo(0xff, 0xff, 0, (float)counter.x * 0.1f, *p, count);
                    temp.SetPalette(*p, count);
                }
                if (counter.y != 0 && counter.x >= counter.y) {
                    podsBgSprite->ResetAnimation(2, 0);
                    g_SlimeTable_0046bf28->Play(4);
                    goto skip;
                }
                value = 0;
                break;
            }
            case 2:
                if ((field_0x110 & 1) != 0) {
                    g_SlimeTable_0046bf28->Play(2);
                } else {
                    g_SlimeTable_0046bf28->Play(3);
                }
                value = -1;
                field_0x110 |= 8;
                break;
            default:
                goto skip;
            }
            podsBgSprite->ResetAnimation(value, 0);
        }
    }

skip:
    if ((field_0x110 & 0x70) == 0) {
        if (g_TargetList_0046ae58 != 0 && g_CombatSprite_0046ae5c != 0) {
            g_CombatSprite_0046ae5c->ProcessFrame(spriteFrameCount);
        }
        // Original bug at 0x440A50: target list can be null before this call.
        target = g_TargetList_0046ae58->ProcessTargets();
        if ((field_0x110 & 2) != 0 && (field_0x110 & 1) != 0) {
            g_CombatWeapon_0046ae60->DrawCrosshairs();
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseDown = g_InputManager_0046aa08->pMouse->buttons & 1;
            } else {
                mouseDown = 0;
            }
            field_0x10C |= (mouseDown == 0);
            g_CombatWeapon_0046ae60->m_clicked = 0;
            if (g_BgSprite_0046ae50->handle <= 4) {
                if (g_InputManager_0046aa08->pMouse != 0) {
                    mouseDown = g_InputManager_0046aa08->pMouse->buttons & 1;
                } else {
                    mouseDown = 0;
                }
                if (mouseDown != 0 && field_0x10C != 0) {
                    g_CombatWeapon_0046ae60->m_clicked = 1;
                    field_0x10C = 0;
                }
            }
            if (g_CombatWeapon_0046ae60->m_clicked != 0) {
                g_CombatWeapon_0046ae60->OnHit();
                if (target != 0) {
                    target->UpdateProgress(1);
                }
            }
            if (g_CombatWeapon_0046ae60->m_clicked != 0) {
                mouse = g_InputManager_0046aa08->pMouse;
                mouseX = (mouse == 0) ? 0 : mouse->x;
                g_BgSprite_0046ae50->ResetAnimation(
                    mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5) + 5, 0);
            }
            if (g_BgSprite_0046ae50->Do(g_BgSprite_0046ae50->loc.x, g_BgSprite_0046ae50->loc.y, 1.0) != 0) {
                mouse = g_InputManager_0046aa08->pMouse;
                mouseX = (mouse == 0) ? 0 : mouse->x;
                g_BgSprite_0046ae50->ResetAnimation(
                    mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
            }
        }
    }
}


/* Function start: 0x440F30 */
void StartScheduleTimer() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    g_CombatEngine_0046ae78->EnqueueHotspotAction(&action);
}

/* Function start: 0x440FC0 */
int PodsEngine::HandleAction(SpriteAction* param) {
    int result = 0;
    switch (param->extra1) {
    case 1:
        g_CombatEngine_0046ae78->combatFlags |= 2;
        return 1;

    case 2:
        g_CombatEngine_0046ae78->combatFlags |= 1;
        return 1;
    }
    return result;
}

/* Function start: 0x441000 */
void PodsEngine::OnProcessEnd() {
    int mouseX;
    int idx;
    GameState* gs;
    InputState* mouse;

    if (g_InputManager_0046aa08 != 0) {
        g_InputManager_0046aa08->Refresh(1);
    }

    if (g_BgSprite_0046ae50 != 0) {
        // Original bug at 0x441000: DAT_0046aa08 is dereferenced after only the earlier Refresh guard.
        mouse = g_InputManager_0046aa08->pMouse;
        mouseX = (mouse == 0) ? 0 : mouse->x;
        g_BgSprite_0046ae50->ResetAnimation(
            mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
    }

    reserved_0x104 = 0;
    field_0x10C = 1;
    field_0x110 = 0;

    gs = g_GameState_0046aa30;
    idx = gs->FindLabel("OBJ011");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }

    if (gs->stateValues[idx] != 0) {
        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("PERIOD");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }

        if (gs->stateValues[idx] != 2) {
            field_0x110 |= 1;
            if (g_SlimeTable_0046bf28->Play(0) != 0) {
                field_0x110 |= 4;
            }
            return;
        }
    }

    field_0x110 &= ~1;
    if (g_SlimeTable_0046bf28->Play(1) != 0) {
        field_0x110 |= 4;
    }
}

/* Function start: 0x441140 */
int PodsEngine::LBLParse(char* line) {
    char buffer[128];
    char token[32];
    char* name;
    char* copy;
    unsigned int key;
    int value;

    key = 0;
    value = 0;
    buffer[0] = 0;
    token[0] = 0;
    sscanf(line, " %s ", token);

    if (strcmp(token, "STUDENT_SPRITE") == 0) {
        podsBgSprite = new Sprite(0);
        Parser::ProcessFile(podsBgSprite, this, 0);
    } else if (strcmp(token, "STUDENT_PALETTE") == 0) {
        sscanf(line, " %s %d %d", token, &podRange.x, &podRange.y);
    } else if (strcmp(token, "STUDENT_HITS_ALLOWED") == 0) {
        sscanf(line, " %s %d", token, &counter.y);
    } else if (strcmp(token, "CINEMATIC") == 0) {
        int fields = sscanf(line, " %s %d %s", token, &key, buffer);
        if (hashTable == 0) {
            hashTable = new HashTable(10);
        }
        if (fields == 3) {
            HashTable* table = hashTable;
            HashNode* node = 0;
            name = MakeSoundName(buffer);
            copy = new char[strlen(name) + 1];
            strcpy(copy, name);
            unsigned int bucket = (key >> 4) % (unsigned int)table->numBuckets;
            if (table->buckets != 0) {
                node = table->buckets[bucket];
                while (node != 0) {
                    if (node->key == key) {
                        break;
                    }
                    node = node->next;
                }
            }
            if (node == 0) {
                if (table->buckets == 0) {
                    table->buckets = (HashNode**)operator new(table->numBuckets * sizeof(HashNode*));
                    memset(table->buckets, 0, table->numBuckets * sizeof(int));
                }
                node = table->AllocateNode();
                node->bucketIndex = bucket;
                node->key = key;
                node->next = table->buckets[bucket];
                table->buckets[bucket] = node;
            }
            node->reserved = (int)copy;
        }
    } else if (strcmp(token, "SOUND") == 0) {
        int fields = sscanf(line, " %s %d %s %d ", token, &key, buffer, &value);
        if (fields != 4 || (int)key < 0 || (int)key > 4) {
            ReportUnknownLabel("EnginePodCombat");
        }
        g_SlimeTable_0046bf28->LoadEntry(key, buffer, value);
    } else if (strcmp(token, "ANIM") == 0) {
        int fields = sscanf(line, " %s %d %d", token, &key, &value);
        if (fields != 3 || (int)key < 0 || (int)key > 2) {
            ReportUnknownLabel("SC_Pods");
        }
        g_PodResults_004734a0[key] = value;
    } else if (strcmp(token, "END_DERIVED_ENGINE_INFO") == 0) {
        return 1;
    } else {
        SC_CombatBase::LBLParse(line);
    }
    return 0;
}
