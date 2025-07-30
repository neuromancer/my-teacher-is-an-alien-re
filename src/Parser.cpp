#include "Parser.h"
#include <string.h>
#include <stdio.h>

extern "C" int _fclose(FILE*);
extern "C" FILE* FUN_00425e50(const char*, const char*);
extern "C" void ShowError(const char*, ...);

/* Function start: 0x4189f0 */
void Parser::Init()
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
    this->pFile = FUN_00425e50(this->filename, (char*)0x4364dc);
    this->field_0x8 = 1;
    if (this->pFile == NULL) {
        ShowError((char*)0x4364b0, this->filename);
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
        ShowError((char*)0x4364e0, this->filename);
    }
}

/* Function start: 0x418B00 */
void Parser::CloseFile()
{
    if (this->field_0x8 != 0 && this->pFile != NULL) {
        _fclose(this->pFile);
        this->pFile = NULL;
        this->field_0x8 = 0;
    }
}
