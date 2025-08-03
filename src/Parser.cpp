#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern "C" FILE* FUN_00425e50(const char*, const char*);
extern "C" void ShowError(const char*, ...);
extern "C" int FUN_00425f30(FILE*, int, int);
extern "C" char *internal_ReadLine(char *s, int n, FILE *stream);
extern "C" char *strstr_wrapper(const char*, const char*);
extern "C" void FUN_00425fd0(char*, char*, int);

/* Function start: 0x4189f0 */
Parser::Parser()
{
    memset(&this->field_0x4, 0, sizeof(Parser) - 4);
}

/* Function start: 0x418A10 */
Parser::~Parser()
{
    CloseFile();
}

/* Function start: 0x418A20 */
void Parser::Open(char *param_1)
{
    CloseFile();
    strcpy(this->filename, param_1);
    this->pFile = FUN_00425e50(this->filename, "r");
    this->isFileOpen = 1;
    if (this->pFile == NULL) {
        ShowError("Parser::Open - Unable to open file '%s' ", this->filename);
    }
}

/* Function start: 0x418A90 */
void Parser::Copy(Parser* other)
{
    if (this->pFile != other->pFile) {
        CloseFile();
    }
    strcpy(this->filename, other->filename);
    this->pFile = other->pFile;
    if (this->pFile == NULL) {
        ShowError("Parser::Open - File '%s' not open", this->filename);
    }
}

/* Function start: 0x418B00 */
void Parser::CloseFile()
{
    if (this->isFileOpen != 0 && this->pFile != NULL) {
        fclose(this->pFile);
        this->pFile = NULL;
        this->isFileOpen = 0;
    }
}

/* Function start: 0x418B30 */
int Parser::LBLParse(char* param_1)
{
    ShowError("%s::LBLParse - Uknown Label\\n'%s'\\nfound in file %s", param_1, this->lineNumber, this->filename);
    return 0;
}

/* Function start: 0x418B50 */
void Parser::SaveFilePosition()
{
    if (fgetpos(this->pFile, &this->savedFilePos) != 0) {
        ShowError("Parser::SaveFilePosition() - unable to save file pos in file %s", this->filename);
    }
}

/* Function start: 0x418B80 */
void Parser::RestoreFilePosition()
{
    if (fsetpos(this->pFile, &this->savedFilePos) != 0) {
        ShowError("Parser::RestoreFilePosition() - unable to restore file pos in file %s", this->filename);
    }
}

/* Function start: 0x418BB0 */
void Parser::FindKey(unsigned char* param_1)
{
    char local_100[256];
    char temp_buffer[256];

    if (param_1 == NULL) {
        return;
    }

    FUN_00425f30(this->pFile, 0, 0);

    do {
        if (internal_ReadLine(local_100, 255, this->pFile) == NULL) {
            ShowError("Parser::FindKey - Unable to find key '%s' in file '%s'", param_1, this->filename);
        }
        sscanf(local_100, "%s", temp_buffer);
        if (strcmp(temp_buffer, (char*)param_1) == 0) {
            return;
        }
    } while (1);
}

/* Function start: 0x418C70 */
int Parser::GetKey(char* line)
{
    char key_buffer[64];
    char* start_key = strstr_wrapper(line, "<<");
    if (start_key) {
        char* end_key = strstr_wrapper(line, ">>");
        if (end_key) {
            int key_len = end_key - (start_key + 2);
            if (key_len > 0) {
                FUN_00425fd0(key_buffer, start_key + 2, key_len);
                key_buffer[key_len] = 0;

                if (this->isProcessingKey == 0) {
                    strcpy(this->currentKey, key_buffer);
                    this->isProcessingKey = 1;
                } else {
                    if (strcmp(this->currentKey, key_buffer) == 0) {
                        memset(this->currentKey, 0, sizeof(this->currentKey));
                        this->isProcessingKey = 0;
                    }
                }
            }
        }
    }
    return this->isProcessingKey;
}

/* Function start: 0x418D60 */
Parser* ParseFile(Parser* parser, char* filename, char* key_format, ...)
{
    char key_buffer[64];
    key_buffer[0] = 0;

    if (key_format != NULL) {
        va_list args;
        va_start(args, key_format);
        vsprintf(key_buffer, key_format, args);
        va_end(args);
    }

    parser->Open(filename);
    parser->ProcessFile(parser, key_buffer);
    parser->CloseFile();
    return parser;
}

/* Function start: 0x418DC0 */
void Parser::ProcessFile(Parser* dst, char* key)
{
    char line_buffer[256];

    this->Copy(dst);

    if (key != NULL && *key != 0) {
        this->FindKey((unsigned char*)key);
    }

    while (this->pFile != NULL && (this->pFile->_flag != 0)) {
        char* line = internal_ReadLine(line_buffer, 255, this->pFile);
        if (line == NULL) {
            ShowError("Parser::Parser - premature EOF in '%s'- probably missing END label", this->filename);
        }

        if (this->GetKey(line) == 0) {
            this->lineNumber = (int)line;
        }
    }
    ShowError("Parser::Parser - premature EOF in '%s' - Invalid File Pointer", this->filename);
}
