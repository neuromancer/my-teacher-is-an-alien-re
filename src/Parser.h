#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

class Parser
{
public:
    Parser();
    ~Parser();
    virtual int LBLParse(char*);
    virtual void OnProcessStart();
    virtual void OnProcessEnd();
    void Init();
    void CloseFile();
    void Open(char *param_1);
    void Copy(Parser* other);
    void SaveFilePosition();
    void RestoreFilePosition();
    void FindKey(unsigned char*);
    int GetKey(char* line);
    static Parser* ProcessFile(Parser* self, Parser* dst, char* key_format, ...);
    void* FUN_004209e0(const char*, int);
    void FUN_00420a50();

    // 0x00: vtable (implicit)
    int field_0x4;          // 0x04
    int isFileOpen;         // 0x08
    int isProcessingKey;    // 0x0c
    char currentKey[32];    // 0x10 - 0x2f
    int lineNumber;         // 0x30
    int field_0x34;         // 0x34
    int savedFilePos;       // 0x38 (fpos_t is int in MSVC 4.2)
    int field_0x3c;         // 0x3c
    char filename[64];      // 0x40 - 0x7f
    FILE* pFile;            // 0x80
    int field_0x84;         // 0x84
};

Parser* ParseFile(Parser* parser, char* filename, char* key_format, ...);

#endif // PARSER_H
