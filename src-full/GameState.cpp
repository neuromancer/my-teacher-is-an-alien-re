#include "GameState.h"
#include "ObjectPool.h"
#include "string.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>

extern char* g_Buffer_0046aa00;

// Hash table entry for GameState label storage
struct GSHashEntry {
    GSHashEntry* next;  // 0x00
    int bucketIndex;    // 0x04
    char* label;        // 0x08 (value)
    int stateIndex;     // 0x0C (key)
};

// Hash function for label strings (from original binary)
static unsigned int HashLabel(char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = hash * 33 + (unsigned char)*str;
        str++;
    }
    return hash;
}

char s_COMBATS[] = "COMBATS";
char s_duncan[] = "duncan";
char s_DIALOG[] = "DIALOG";
char s_COMBAT[] = "COMBAT";
char s_susan[] = "susan";
char s_MEET_MY[] = "MEET MY";
char s_TESTPUZZLE[] = "TESTPUZZLE";


/* Function start: 0x409F20 */
void GameState::ValidateIndex(int idx) {
    if (idx < 0 || maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
}

/* Function start: 0x433CB0 */
void GameState::SetFromAction(int* action) {
    int idx;
    int val;

    idx = action[1];
    if (idx < 0 || maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }

    switch (action[4]) {
    case 0:
        return;
    case 0xE:
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        if (stateValues[idx] == 0) {
            ValidateIndex(idx);
            stateValues[idx] = 1;
        } else {
            ValidateIndex(idx);
            stateValues[idx] = 0;
        }
        return;
    case 0xF:
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateValues[idx] = 1;
        return;
    case 0x10:
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateValues[idx] = 0;
        return;
    case 0x11:
        val = action[5];
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateValues[idx] += val;
        return;
    case 0x12:
        val = action[5];
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateValues[idx] -= val;
        return;
    case 0x13:
        val = action[5];
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateValues[idx] = val;
        return;
    case 0x14:
        if (idx < 0 || maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        val = stateValues[idx];
        {
            int mask = action[5];
            if (idx < 0 || maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateValues[idx] = val | mask;
        }
        return;
    default:
        ShowError("GameState::SetState() - invalid instruction in action");
        return;
    }
}

/* Function start: 0x432E20 */
int GameState::FUN_00432e20(char* name) {
    int i;

    i = 0;
    while (i < maxStates) {
        if (stateLabels != 0 && stateLabels[i] != 0) {
            if (strstr((char*)stateLabels[i], name) != 0) {
                if (strlen((char*)stateLabels[i]) == strlen(name)) {
                    return i;
                }
            }
        }
        i++;
    }
    ShowError("GameState::FindState unable to find %s", name);
    return -1;
}

/* Function start: 0x4333D0 */
GameState::~GameState()
{
    if (stateValues != 0) {
        FreeMemory(stateValues);
        stateValues = 0;
    }

    if (stateLabels != 0) {
        ((ObjectPool*)stateLabels)->~ObjectPool();
        FreeMemory(stateLabels);
        stateLabels = 0;
    }
}

/* Function start: 0x433480 */
void GameState::Serialize(void* param)
{
    int* header;
    FILE* file;

    file = *(FILE**)((char*)param + 0x44);
    header = (int*)operator new(0x110);
    int len = strlen("GAMESTATE_INFO") + 1;

    if (*(int*)param != 0) {
        fwrite("GAMESTATE_INFO", len, 1, file);
        header[0] = maxStates;
        header[1] = 0x110;
        header[2] = 1;
        header[3] = maxStates * 4 + 0x110;
        fwrite(header, 0x110, 1, file);
        fwrite(stateValues, 4, header[0], file);
    } else {
        *g_Buffer_0046aa00 = 0;
        fread(g_Buffer_0046aa00, len, 1, file);
        if (strcmp(g_Buffer_0046aa00, "GAMESTATE_INFO") != 0) {
            ShowError("GameState::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
        }
        fread(header, 0x110, 1, file);
        if (maxStates != (unsigned int)header[0] || header[2] != 1) {
            ShowError("GameState::Serialize incompatible file");
        }
        fread(stateValues, 4, header[0], file);
    }

    if (header != 0) {
        FreeMemory(header);
    }
}

/* Function start: 0x4335D0 */
void GameState::SetMaxStates(int count)
{
    if (stateValues != 0) {
        ShowError("GameState::SetMaxStates - Already Called");
    }
    maxStates = count;
    stateValues = new int[count];
    ClearStates();

    ObjectPool* pool = new ObjectPool(0x11, count);

    if (count != 0xa) {
        if (pool->memory != 0) {
            FreeMemory(pool->memory);
            pool->memory = 0;
        }
        int numBuckets = count + (int)((double)count * 0.3);
        int* buckets = (int*)AllocateMemory(numBuckets * 4);
        memset(buckets, 0, numBuckets * 4);
        pool->memory = buckets;
        pool->size = numBuckets;
    }

    stateLabels = (char**)pool;
}

/* Function start: 0x433710 */
int GameState::LBLParse(char* line)
{
    int index;
    int defaultValue;
    int numParsed;
    char keyword[32];
    char labelName[32];

    labelName[0] = '\0';
    keyword[0] = '\0';
    sscanf(line, " %s ", keyword);

    if (strcmp(keyword, "MAXSTATES") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        SetMaxStates(index);
    } else if (strcmp(keyword, "LABEL") == 0) {
        numParsed = sscanf(line, "%s %d %s %d", keyword, &index, labelName, &defaultValue);

        // Allocate and copy label string
        char* newLabel = new char[strlen(labelName) + 1];
        strcpy(newLabel, labelName);

        // Hash the label string and insert into ObjectPool hash table
        ObjectPool* pool = (ObjectPool*)stateLabels;
        unsigned int h = HashLabel(newLabel) % pool->size;

        // Search for existing entry with same label
        GSHashEntry* entry = 0;
        if (pool->memory != 0) {
            entry = ((GSHashEntry**)pool->memory)[h];
            while (entry != 0) {
                if (strcmp(entry->label, newLabel) == 0) break;
                entry = entry->next;
            }
        }

        if (entry == 0) {
            // Not found - allocate new entry
            if (pool->memory == 0) {
                pool->MemoryPool_Allocate(pool->size, 1);
            }
            entry = (GSHashEntry*)pool->Allocate_2();
            entry->bucketIndex = h;
            entry->next = ((GSHashEntry**)pool->memory)[h];
            ((GSHashEntry**)pool->memory)[h] = entry;
        }
        entry->label = newLabel;
        entry->stateIndex = index;

        if (numParsed > 3) {
            if (index < 0 || maxStates - 1 < index) {
                ShowError("Invalid gamestate %d", index);
            }
            stateValues[index] = defaultValue;
        }
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("GameState");
    }

    return 0;
}

/* Function start: 0x433A20 */
// Full game stateLabels (0x94) is a hash table:
//   [0]=buckets, [4]=numBuckets, [8]=count
// Node: [0]=next, [4]=bucketIndex, [8]=value(char*), [C]=key
char *GameState::GetState(int stateIndex)
{
    if (stateIndex < 0 || maxStates - 1 < stateIndex) {
        ShowError("Invalid gamestate %d", stateIndex);
    }

    int* hashTable = (int*)stateLabels;
    int* cursor;
    int* node;

    // ADC pattern: cursor = (count < 1) ? 0 : -1
    cursor = 0;
    if (hashTable[2] >= 1) {
        cursor = (int*)-1;
    }

loop:
    if (cursor == 0) {
        return 0;
    }
    node = (int*)cursor;
    if (cursor == (int*)-1) {
        // Find first non-empty bucket
        int i = 0;
        int numBuckets = hashTable[1];
        if (numBuckets == 0) goto found_node;
        int* buckets = (int*)hashTable[0];
    scan_buckets:
        node = (int*)buckets[i];
        if (node != 0) goto found_node;
        i++;
        if ((unsigned int)i < (unsigned int)numBuckets) goto scan_buckets;
    }

found_node:
    {
        int bIdx;
        int numBuckets;
        int* next = (int*)node[0];
        if (next != 0) goto advance;
        bIdx = node[1];
        numBuckets = hashTable[1];
        bIdx++;
        if ((unsigned int)bIdx >= (unsigned int)numBuckets) goto advance;
        { int* ptr = (int*)hashTable[0] + bIdx;
    scan_next:
        next = (int*)*ptr;
        if (next != 0) goto advance;
        ptr++;
        bIdx++;
        if ((unsigned int)bIdx < (unsigned int)numBuckets) goto scan_next;
        }

    advance:
        ;
        char* value = (char*)node[2];
        cursor = next;
        if (node[3] == stateIndex) {
            return value;
        }
    }
    goto loop;
}

// FindState delegates to FindLabel (hash lookup)
int GameState::FindState(char* stateName)
{
    return FindLabel(stateName);
}

/* Function start: 0x433AE0 */
int GameState::FindLabel(char* name) {
    ObjectPool* pool = (ObjectPool*)stateLabels;

    // Inline hash: hash = hash * 33 + char
    unsigned int hash = 0;
    char* p = name;
    while (*p) {
        hash = hash * 33 + (unsigned char)*p;
        p++;
    }

    unsigned int h = hash % pool->size;

    GSHashEntry* entry = 0;
    if (pool->memory != 0) {
        entry = ((GSHashEntry**)pool->memory)[h];
        while (entry != 0) {
            if (strcmp(entry->label, name) == 0) break;
            entry = entry->next;
        }
    }

    if (entry == 0) {
        ShowError("GameState::StateIndex()-Invalid gamestate = '%s'", name);
    } else {
        h = entry->stateIndex;
    }
    return h;
}

/* Function start: 0x433B90 */
void GameState::ClearStates()
{
    memset(stateValues, 0, maxStates * 4);
}

/* Function start: 0x433BB0 */
int GameState::FUN_00433bb0(int* param_1) {
    int idx;
    unsigned int val;

    if (param_1 == 0) {
        ShowError("illegal message13");
    }
    if (param_1[0] != 2) {
        ShowError("illegal message12");
    }
    idx = param_1[1];
    if (idx < 0 || maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    val = (unsigned int)stateValues[idx];
    switch (param_1[4]) {
    case 8:
        return val != 0;
    case 9:
        return val == 0;
    case 10:
        return val < (unsigned int)param_1[5];
    case 11:
        return (unsigned int)param_1[5] < val;
    case 12:
        return (unsigned int)param_1[5] == val;
    case 13:
        return (unsigned int)param_1[5] != val;
    default:
        ShowError("illegal message12b");
    }
    return 0;
}
