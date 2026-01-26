#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

class VBuffer;

class Parser
{
public:
    Parser();
    ~Parser(); // DO NOT make this `virtual` or you will break subclasses and have strange crashes!
    virtual int LBLParse(char*);
    virtual void OnProcessStart();
    virtual void OnProcessEnd();

    void CloseFile();
    void Open(char *param_1);
    void Copy(Parser* other);
    Parser* CopyParserFields(Parser* src);  // 0x406610
    void SaveFilePosition();
    void RestoreFilePosition();
    void FindKey(unsigned char*);
    int GetKey(char* line);
    static Parser* ProcessFile(Parser* self, Parser* dst, char* key_format, ...);


    // 0x00: vtable (implicit)
    VBuffer* m_vbuffer;     // 0x04 - used in ZBuffer::CleanUpVBuffer
    void* m_subObject;      // 0x08 - object with virtual destructor, or int flag
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
