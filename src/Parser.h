#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

class Parser
{
public:
    ~Parser();
    void Init();
    void CloseFile();
    void Open(char *param_1);
    void Copy(Parser* other);
private:
    int vtable;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    int field_0x10;
    int field_0x14;
    int field_0x18;
    int field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
    int field_0x30;
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    char filename[64];
    FILE* pFile;
    int field_0x84;
public:
};

#endif // PARSER_H
