#include "GameConfig.h"
#include "Memory.h"
#include <string.h>
#include <stdlib.h>

extern "C" {
    extern char DAT_004371a8[];
    extern char* PTR_s_Setup_cfg_00437454;
    int FileExists(const char* filename);
}

#include <stdio.h>

/* Function start: 0x422690 */
GameConfig::GameConfig() {
    // Zero entire object (0x25 dwords = 148 bytes = 0x94)
    memset(this, 0, sizeof(GameConfig));
    // Zero data section again (0x14 dwords = 80 bytes at offset 0x44)
    memset(&data, 0, 80);
    
    if (FileExists(PTR_s_Setup_cfg_00437454) == 0) {
        CreateDefaultConfig();
    }
    ReloadConfig();
    CheckWindir();
}

/* Function start: 0x422750 */
ConfigData::~ConfigData() {
    // Empty destructor
}

/* Function start: 0x4227A0 */
GameConfig::~GameConfig() {
    Close();
}

/* Function start: 0x422800 */
FILE* GameConfig::Open(char* mode) {
    fp = fopen(PTR_s_Setup_cfg_00437454, mode);
    return fp;
}

/* Function start: 0x422820 */
void GameConfig::Close() {
    if (fp) {
        fclose(fp);
        fp = 0;
    }
}

/* Function start: 0x422840 */
void GameConfig::CreateDefaultConfig() {
    // Zero out data (20 dwords = 80 bytes at offset 0x44)
    memset(&data, 0, 80);
    // Set default values
    data.rawData[0] = 2;  // offset 0x44
    data.rawData[2] = 2;  // offset 0x46
    data.rawData[1] = 0;  // offset 0x45
    
    CheckWindir();
    LoadConfig();
}

/* Function start: 0x422870 */
void GameConfig::LoadConfig() {
    if (Open(DAT_004371a8)) {
        fwrite(&data, 80, 1, fp);
        Close();
    }
}

/* Function start: 0x4228A0 */
void GameConfig::ReloadConfig() {
    if (Open(DAT_004371a8)) {
        fread(&data, 0x50, 1, fp);
        Close();
    }
    CheckWindir();
}
/* Function start: 0x4228E0 */
unsigned char GameConfig::CheckWindir() {
    char* result = getenv("windir");
    data.rawData[6] = (result != 0) ? 1 : 0;
    return data.rawData[6];
}

