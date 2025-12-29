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
    int Init(char* cmdLine);
    void Constructor(); // 0x422690
    void LoadConfig(); // 0x422870
};

#endif // GAMECONFIG_H
