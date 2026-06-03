#include "RockThrower.h"
#include "stubs.h"
#include "globals.h"
#include "Projectile.h"
#include "GameState.h"
#include "Target.h"
#include "HashTable.h"
#include "MouseControl.h"
#include "InputManager.h"
#include <string.h>
#include <stdio.h>

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);

/* Function start: 0x4274C0 */
RockThrower::RockThrower(Parser* parent) {
    memset(&m_itemCount, 0, 6 * sizeof(int));
    m_itemCount = 3;
    m_posY = 0xa0;
    m_posX = 0xa0;
    m_posZ = 0xa6;
    Parser::ProcessFile(this, parent, 0);
}

// 0x427710 = ZBuffer::~ZBuffer (COMDAT)
RockThrower::~RockThrower() {
    while (m_itemCount != 0) {
        m_itemCount = m_itemCount - 1;
        Projectile* p = m_items[m_itemCount];
        if (p != 0) {
            delete p;
            m_items[m_itemCount] = 0;
        }
    }
    if (m_items != 0) {
        FreeMemory(m_items);
        m_items = 0;
    }
}

// 0x4279A0 = ZBuffer::ResetItems (COMDAT)
void RockThrower::ResetProjectiles() {
    int i = 0;
    if (m_itemCount > 0) {
        int offset = 0;
        do {
            offset = offset + 4;
            i = i + 1;
            m_items[i - 1]->active = 0;
        } while (i < m_itemCount);
    }
}

/* Function start: 0x4279D0 */
void RockThrower::DrawCrosshairs() {
    if (g_Mouse_0046aa18 != 0) {
        g_Mouse_0046aa18->DrawCursor();
        return;
    }
    SetFillColor(0xfa);
    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    int mouseY = 0;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }
    int mouseX;
    if (pMouse != 0) {
        mouseX = pMouse->x;
    } else {
        mouseX = 0;
    }
    SetDrawPosition(mouseX, mouseY);
    DrawCircle(7);
}

/* Function start: 0x427A30 */
int RockThrower::CheckTargetHit(int param_1) {
    int entry;
    int target;

    if (g_TargetList_0046ae58 != 0) {
        HashTable* ht = g_TargetList_0046ae58->hashTable;
        if (ht == 0) {
            return 0;
        }

        entry = ((unsigned int)ht->count < 1) - 1;

        while (entry != 0) {
            int* edx = (int*)entry;
            if (entry == -1) {
                unsigned int idx = 0;
                unsigned int numBuckets = ht->numBuckets;
                if (numBuckets != 0) {
                    HashNode** buckets = ht->buckets;
                    do {
                        edx = (int*)buckets[idx];
                        if (edx != 0) break;
                        idx++;
                    } while (numBuckets > idx);
                }
            }

            // Original bug at 0x427AA0: the bucket scan result is dereferenced without rechecking the sentinel/null case.
            int* nextEntry = (int*)edx[0];
            if (nextEntry == 0) {
                unsigned int idx = edx[1] + 1;
                unsigned int numBuckets = ht->numBuckets;
                if (idx < numBuckets) {
                    HashNode** cursor = &ht->buckets[idx];
                    do {
                        nextEntry = (int*)*cursor;
                        if (nextEntry != 0) break;
                        cursor++;
                        idx++;
                    } while (idx < numBuckets);
                }
            }

            target = edx[3];
            entry = (int)nextEntry;

            if (target != 0) {
                if (((Target*)target)->CheckTimeInRangeParam((int*)(param_1 + 0x120)) != 0) {
                    ((Target*)target)->UpdateProgress(1);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* Function start: 0x427B20 */
/* Function start: 0x427B43 */
int RockThrower::LBLParse(char* line) {
    char token[32];
    int count = 0;
    GameState* gs;

    token[0] = 0;
    sscanf(line, " %s", token);

    if (strcmp(token, "MAXROCKS") == 0) {
        sscanf(line, "%s %d", token, &m_itemCount);
        count = 0;
        m_items = new Projectile*[m_itemCount];
        if (m_itemCount > 0) {
            int offset = 0;
            do {
                Projectile* p = new Projectile((int)this);
                offset = offset + 4;
                count = count + 1;
                m_items[count - 1] = p;
            } while (count < m_itemCount);
        }
    } else if (strcmp(token, "SPRITE") == 0) {
        SaveFilePosition();
        count = 0;
        if (m_itemCount > 0) {
            int offset = 0;
            do {
                RestoreFilePosition();
                Parser::ProcessFile(m_items[count], this, 0);
                int periodIdx = g_PeriodStateIdx_0046cb90;
                gs = g_GameState_0046aa30;
                if (periodIdx < 0 || gs->maxStates - 1 < periodIdx) {
                    ShowError("Invalid gamestate %d", periodIdx);
                }
                sprintf(token, "AMMO_SPEED_%c",
                        (char)g_PeriodCharTable_0046cb94[gs->stateValues[periodIdx]]);
                gs = g_GameState_0046aa30;
                int speedIdx = gs->FindLabel(token);
                if (speedIdx < 0 || gs->maxStates - 1 < speedIdx) {
                    ShowError("Invalid gamestate %d", speedIdx);
                }
                offset = offset + 4;
                m_items[count]->ConfigRange(0, 1, gs->stateValues[speedIdx], 1);
                count = count + 1;
            } while (count < m_itemCount);
        }
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("RockThrower");
    }
    return 0;
}
