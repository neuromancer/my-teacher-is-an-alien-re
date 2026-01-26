#include "CombatSprite.h"
#include "string.h"
#include <stdio.h>
#include <string.h>

// Global variables
SpriteHashTable* g_CurrentSprite = 0;   // 0x436348
int g_CurrentSpriteIndex = 0;           // 0x43634c

CombatSprite::CombatSprite() : Parser() {
    CombatSprite::spriteTable = 0;
}

CombatSprite::~CombatSprite() {
    // TODO: cleanup spriteTable if needed
}

/* Function start: 0x41569E */
int CombatSprite::LBLParse(char* line) {
    char token[52];
    int slot;
    void* entry;
    SpriteHashTable* table;
    SpriteHashTable* currentSprite;
    int currentIndex;

    sscanf(line, "%s", token);

    if (token[0] == 'S') {
        // Create sprite table if it doesn't exist
        if (CombatSprite::spriteTable == 0) {
            CombatSprite::spriteTable = new SpriteHashTable(0x11);
        }

        // Commit current sprite to table if exists
        if (g_CurrentSprite != 0) {
            currentSprite = g_CurrentSprite;
            currentIndex = g_CurrentSpriteIndex;
            table = CombatSprite::spriteTable;

            entry = table->Lookup(currentIndex, &slot);
            if (entry == 0) {
                if (table->buckets == 0) {
                    table->Resize(table->maxSize, 1);
                }
                entry = table->AllocEntry();
                ((int*)entry)[1] = slot;
                ((int*)entry)[2] = currentIndex;
                ((int*)entry)[0] = ((int*)table->buckets)[slot];
                ((int*)table->buckets)[slot] = (int)entry;
            }
            ((int*)entry)[3] = (int)currentSprite;
            g_CurrentSprite = 0;
        }

        // Create new sprite entry
        g_CurrentSprite = new SpriteHashTable();

        // Parse " %s %d " format to get label and index
        sscanf(line, " %s %d ", token, &g_CurrentSpriteIndex);

        // Parse sprite data starting after "Sxx"
        CombatSprite::ParseSpriteData(line + 3);

        return 0;
    }
    else {
        // Check for END marker
        if (_stricmp(token, "END") == 0) {
            // Commit current sprite if exists
            if (g_CurrentSprite != 0) {
                currentSprite = g_CurrentSprite;
                currentIndex = g_CurrentSpriteIndex;
                table = CombatSprite::spriteTable;

                entry = table->Lookup(currentIndex, &slot);
                if (entry == 0) {
                    if (table->buckets == 0) {
                        table->Resize(table->maxSize, 1);
                    }
                    entry = table->AllocEntry();
                    ((int*)entry)[1] = slot;
                    ((int*)entry)[2] = currentIndex;
                    ((int*)entry)[0] = ((int*)table->buckets)[slot];
                    ((int*)table->buckets)[slot] = (int)entry;
                }
                ((int*)entry)[3] = (int)currentSprite;
                g_CurrentSprite = 0;
            }
            return 1;
        }

        // Check for continuation marker
        if (_stricmp(token, "S") == 0) {
            // Commit current sprite if exists
            if (g_CurrentSprite != 0) {
                currentSprite = g_CurrentSprite;
                currentIndex = g_CurrentSpriteIndex;
                table = CombatSprite::spriteTable;

                entry = table->Lookup(currentIndex, &slot);
                if (entry == 0) {
                    if (table->buckets == 0) {
                        table->Resize(table->maxSize, 1);
                    }
                    entry = table->AllocEntry();
                    ((int*)entry)[1] = slot;
                    ((int*)entry)[2] = currentIndex;
                    ((int*)entry)[0] = ((int*)table->buckets)[slot];
                    ((int*)table->buckets)[slot] = (int)entry;
                }
                ((int*)entry)[3] = (int)currentSprite;
                g_CurrentSprite = 0;
            }
            return 0;
        }

        // Continue parsing sprite data or error
        if (g_CurrentSprite != 0) {
            CombatSprite::ParseSpriteData(line);
        }
        else {
            ShowError("CombatSprite");
        }

        return 0;
    }
}
