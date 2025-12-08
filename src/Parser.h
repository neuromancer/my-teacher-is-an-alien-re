#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

class Parser
{
public:
    void** vtable;
public:
    Parser();
    virtual ~Parser();
    void Init();
    void CloseFile();
    virtual int LBLParse(char*);
    void Open(char *param_1);
    void Copy(Parser* other);
    void SaveFilePosition();
    void RestoreFilePosition();
    void FindKey(unsigned char*);
    int GetKey(char* line);
    static void ProcessFile(Parser* self, Parser* dst, char* key);
public:
private:
    int field_0x4;
    int isFileOpen;
    int isProcessingKey;
    char currentKey[32];
    int field_0x14;
    int field_0x18;
    int field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
    int lineNumber;
    int field_0x34;
    fpos_t savedFilePos;
    int field_0x3c;
    char filename[20];
    FILE* pFile;
    int field_0x84;
public:
};

Parser* ParseFile(Parser* parser, char* filename, char* key_format, ...);

#endif // PARSER_H
