#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <stdio.h>

class GameConfig {
public:
    FILE* fp; // 0x00
    int unused_04;
    int unused_08;
    int isSet; // 0x0c
    char value[32]; // 0x10
    char unused_30[20]; // 0x30
    unsigned char data[80]; // 0x44 -- This overlaps? 
    // Wait, 0x30 + 20 = 0x44. Correct.
    // data at 0x44.

    // Methods
    GameConfig(); // 0x422690
    ~GameConfig(); // 0x4227A0
    int Init(char* cmdLine);
    void LoadConfig(); // 0x422870
    void SaveConfig(); // 0x4228A0
    FILE* Open(char* mode); // 0x422800
    void Close(); // 0x422820
    void CreateDefaultConfig(); // 0x422840
    unsigned char CheckWindir(); // 0x4228E0
};

#endif // GAMECONFIG_H
