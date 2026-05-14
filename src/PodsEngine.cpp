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
#include "SlimeTable.h"
#include "globals.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void StartScheduleTimer();
extern char* MakeSoundName(char*);

/* Function start: 0x440860 */
PodsEngine::PodsEngine()
{
    reserved_0xFC = 0;
    reserved_0x100 = 0;
    memset(&podsBgSprite, 0, 10 * sizeof(int));
    podState.x = 0;
    podState.y = 3;
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
        if (((SoundList*)g_SlimeTable_0046bf28)->IsSamplePlaying((~flags) & 1) == 0) {
            field_0x110 &= ~4;
            field_0x110 |= 2;
            if (g_Navigator_0046ae70->startingNode != 2) {
                g_Navigator_0046ae70->SetNavParams(2, 0);
            }
        }
    } else {
        if ((flags & 8) != 0) {
            value = ((flags & 1) == 0) ? 3 : 2;
            if (((SoundList*)g_SlimeTable_0046bf28)->IsSamplePlaying(value) == 0) {
                StartScheduleTimer();
            }
        }
    }

    if (podsBgSprite != 0) {
        state = podsBgSprite->handle;
        if (reserved_0x104 != g_ScoreDisplay_0046ae6c->hitCount && state != 2) {
            reserved_0x104 = g_ScoreDisplay_0046ae6c->hitCount;
            podsBgSprite->ResetAnimation(1, 0);
            podState.x++;
            if (podState.y == 0) {
                ready = 0;
            } else {
                ready = 1;
                if (podState.x >= podState.y) {
                } else {
                    ready = 0;
                }
            }
            if (ready) {
                field_0x110 |= 0x20;
                if (g_Palette_0046ae64 != 0) {
                    g_Palette_0046ae64->SetPalette(
                        g_WeaponParser_0046ae4c->paletteStart.x,
                        (g_WeaponParser_0046ae4c->paletteEnd.x - g_WeaponParser_0046ae4c->paletteStart.x) + 1);
                }
                if (targetList != 0) {
                    delete targetList;
                    targetList = 0;
                }
                g_TargetList_0046ae58 = 0;
            }
        }

        if (podsBgSprite->Do(podsBgSprite->loc_x, podsBgSprite->loc_y, 1.0) != 0) {
            if (state == 1) {
                int start = reserved_0xFC;
                int count = (reserved_0x100 - start) + 1;
                if (g_PodsPalette_0046bf30 == 0) {
                    g_PodsPalette_0046bf30 = new Palette();
                    g_PodsPalette_0046bf30->CopyEntries(0, 0x100);
                }
                {
                    Palette temp;
                    temp.CopyRGBData((int)g_PodsPalette_0046bf30->m_data, start, count);
                    temp.FadeTo(0xff, 0xff, 0, (float)podState.x * 0.1f, start, count);
                    temp.SetPalette(start, count);
                }
                if (podState.y != 0 && podState.x >= podState.y) {
                    podsBgSprite->ResetAnimation(2, 0);
                    ((SoundList*)g_SlimeTable_0046bf28)->Play(4);
                } else {
                    podsBgSprite->ResetAnimation(0, 0);
                }
            } else if (state == 2) {
                if ((field_0x110 & 1) == 0) {
                    value = 3;
                } else {
                    value = 2;
                }
                ((SoundList*)g_SlimeTable_0046bf28)->Play(value);
                field_0x110 |= 8;
                podsBgSprite->ResetAnimation(-1, 0);
            }
        }
    }

    if ((field_0x110 & 0x70) == 0) {
        if (g_TargetList_0046ae58 != 0 && g_CombatSprite_0046ae5c != 0) {
            g_CombatSprite_0046ae5c->ProcessFrame(spriteFrameCount);
        }
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
            if (g_BgSprite_0046ae50->Do(g_BgSprite_0046ae50->loc_x, g_BgSprite_0046ae50->loc_y, 1.0) != 0) {
                mouse = g_InputManager_0046aa08->pMouse;
                mouseX = (mouse == 0) ? 0 : mouse->x;
                g_BgSprite_0046ae50->ResetAnimation(
                    mouseX / (g_WeaponParser_0046ae4c->dimensions.x / 5), 0);
            }
        }
    }
}

extern void __stdcall EnqueueHotspotAction(SpriteAction*);

/* Function start: 0x440F30 */
void StartScheduleTimer() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    EnqueueHotspotAction(&action);
}

/* Function start: 0x440FC0 */
int PodsEngine::HandleAction(int* param) {
    TODO("PodsEngine::HandleAction");
    return 0;
}

/* Function start: 0x441000 */
void PodsEngine::OnProcessEnd() {
    TODO("PodsEngine::OnProcessEnd");
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
        sscanf(line, " %s %d %d", token, &reserved_0xFC, &reserved_0x100);
    } else if (strcmp(token, "STUDENT_HITS_ALLOWED") == 0) {
        sscanf(line, " %s %d", token, &podState.y);
    } else if (strcmp(token, "CINEMATIC") == 0) {
        int fields = sscanf(line, " %s %d %s", token, &key, buffer);
        if (hashTable == 0) {
            hashTable = new HashTable(10);
        }
        if (fields == 3) {
            HashTable* table = hashTable;
            HashNode* node = 0;
            name = MakeSoundName(buffer);
            copy = (char*)operator new(strlen(name) + 1);
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
                    table->buckets = (HashNode**)AllocateMemory(table->numBuckets * sizeof(HashNode*));
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
    } else if (strcmp(token, "") == 0) {
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
