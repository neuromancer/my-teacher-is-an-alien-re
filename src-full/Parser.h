#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

class VBuffer;

class Parser
{
public:
    Parser();
    ~Parser();
    virtual int LBLParse(char*);
    virtual void OnProcessStart();
    virtual void OnProcessEnd();

    void ReportUnknownLabel(char* name);  // 0x412A50
    void CloseFile();
    void Open(char *param_1);
    void Copy(Parser* other);
    void SaveFilePosition();
    void RestoreFilePosition();
    void FindKey(unsigned char*);
    int GetKey(char* line);
    int GetTokenType(char* line);              // 0x413810
    void HandleToken(int tokenType, char* line);  // 0x413120
    void BeginComment(char* line);   // 0x412C00
    int EndComment();                          // 0x412FE0
    int DoCommentsMatch(char* line);           // 0x412C60
    void SubstituteVars(char* src, char* dst); // 0x4140F0
    void UpdateProcessingState();              // 0x4130E0
    void PushConditionalState(int value);      // 0x412EB0
    void HandleToken_IF(char* line, int prevResult); // 0x412D00
    void ParseGosubParams(char* line); // 0x414040
    static Parser* ProcessFile(Parser* self, Parser* dst, char* key_format, ...);


    // 0x00: vtable (implicit)
    VBuffer* m_vbuffer;     // 0x04 - used in ZBuffer::CleanUpVBuffer
    int field_0x08;         // 0x08
    int isProcessingKey;    // 0x0C
    char currentKey[32];    // 0x10 - 0x2F
    int savedFilePos;       // 0x30 (fpos_t is int in MSVC 4.2)
    int field_0x34;         // 0x34
    int field_0x38;         // 0x38 - flags (bit 0 = has file position cache)
    void* field_0x3c;       // 0x3C - object pointer (destroyed in dtor)
    int m_subObject;        // 0x40 - open-file flag (was at 0x08 in demo)
    int lineNumber;         // 0x44 (was at 0x30 in demo)
    char filename[64];      // 0x48 - 0x87 (was at 0x40 in demo)
    FILE* pFile;            // 0x88 (was at 0x80 in demo)
    int field_0x8C;         // 0x8C
};

Parser* ParseFile(Parser* parser, char* filename, char* key_format, ...);

#endif // PARSER_H
