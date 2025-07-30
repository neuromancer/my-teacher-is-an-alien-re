#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" FILE* FUN_00425e50(const char*, const char*);
extern "C" void ShowError(const char*, ...);
extern "C" int FUN_00425f30(FILE*, int, int);
extern "C" char *internal_ReadLine(char *s, int n, FILE *stream);

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
        fclose(this->pFile);
        this->pFile = NULL;
        this->field_0x8 = 0;
    }
}

/* Function start: 0x418B30 */
void Parser::LBLParse(char* param_1)
{
    ShowError((char*)0x436504, param_1, this->field_0x30, this->filename);
}

/* Function start: 0x418B50 */
void Parser::SaveFilePosition()
{
    if (fgetpos(this->pFile, (fpos_t*)&this->field_0x38) != 0) {
        ShowError((char*)0x436538, this->filename);
    }
}

/* Function start: 0x418B80 */
void Parser::RestoreFilePosition()
{
    if (fsetpos(this->pFile, (fpos_t*)&this->field_0x38) != 0) {
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
