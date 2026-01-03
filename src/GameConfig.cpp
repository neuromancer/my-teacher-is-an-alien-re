#include "GameConfig.h"
#include "Memory.h"
#include "Animation.h"
#include <string.h>
#include <stdlib.h>

extern "C" {
    FILE* __cdecl FUN_00425e50(const char* filename, const char* mode);
    unsigned int __cdecl FUN_004269e0(void* buffer, unsigned int size, unsigned int count, FILE* stream);
    extern char DAT_004371a8[];
    extern char* PTR_s_Setup_cfg_00437454;
    int FileExists(const char* filename);
}

/* Function start: 0x422750 */
ConfigData::~ConfigData() {
    // Destructor - UnknownClassAnimation destructor will be called automatically
}

/* Function start: 0x422690 */
GameConfig::GameConfig() {
    // Zero entire object (0x25 dwords = 148 bytes = 0x94)
    memset(this, 0, sizeof(GameConfig));
    // Zero data section again (0x14 dwords = 80 bytes at offset 0x44)
    memset(&this->data, 0, 80);
    
    if (FileExists(PTR_s_Setup_cfg_00437454) == 0) {
        CreateDefaultConfig();
    }
    SaveConfig();
    CheckWindir();
}

/* Function start: 0x4227A0 */
GameConfig::~GameConfig() {
    Close();
}

/* Function start: 0x422800 */
FILE* GameConfig::Open(char* mode) {
    this->fp = FUN_00425e50(PTR_s_Setup_cfg_00437454, mode);
    return this->fp;
}

/* Function start: 0x422820 */
void GameConfig::Close() {
    if (this->fp) {
        fclose(this->fp);
        this->fp = 0;
    }
}

/* Function start: 0x422840 */
void GameConfig::CreateDefaultConfig() {
    // Zero out data (20 dwords = 80 bytes at offset 0x44)
    memset(&this->data, 0, 80);
    // Set default values
    this->data.rawData[0] = 2;  // offset 0x44
    this->data.rawData[2] = 2;  // offset 0x46
    this->data.rawData[1] = 0;  // offset 0x45
    
    CheckWindir();
    LoadConfig();
}

/* Function start: 0x422870 */
void GameConfig::LoadConfig() {
    if (Open(DAT_004371a8)) {
        FUN_004269e0(&this->data, 80, 1, this->fp);
        Close();
    }
}

/* Function start: 0x4228A0 */
void GameConfig::SaveConfig() {
    if (Open("wb")) {
        fwrite(&this->data, 0x50, 1, this->fp);
        Close();
    }
    CheckWindir();
}
/* Function start: 0x4228E0 */
unsigned char GameConfig::CheckWindir() {
    char* result = getenv("windir");
    this->data.rawData[6] = (result != 0) ? 1 : 0;
    return this->data.rawData[6];
}

