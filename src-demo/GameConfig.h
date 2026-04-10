#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <stdio.h>
#include "Range.h"
// Embedded object at offset 0x44 of GameConfig (80 bytes = 0x50)
class ConfigData {
public:
    unsigned char rawData[0x34];  // 0x00-0x33 (52 bytes)
    Range range;                  // 0x34 (8 bytes)
    unsigned char padding[0x14];  // 0x3C-0x4F (20 bytes)

    ~ConfigData();
};

class GameConfig {
public:
    FILE* fp; // 0x00
    int unused_04;
    int unused_08;
    int isSet; // 0x0c
    char value[32]; // 0x10
    char unused_30[20]; // 0x30
    ConfigData data; // 0x44 (80 bytes = 0x50)

    // Methods
    GameConfig(); // 0x422690
    ~GameConfig(); // 0x4227A0
    int Init(char* cmdLine);
    void LoadConfig(); // 0x422870
    void ReloadConfig(); // 0x4228A0
    FILE* Open(char* mode); // 0x422800
    void Close(); // 0x422820
    void CreateDefaultConfig(); // 0x422840
    unsigned char CheckWindir(); // 0x4228E0
};

#endif // GAMECONFIG_H
