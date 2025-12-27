#include "globals.h"
#include "Parser.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function start: 0x4189f0 */
Parser::Parser() { memset(&this->m_vbuffer, 0, sizeof(Parser) - 4); }

/* Function start: 0x418A10 */
Parser::~Parser() { CloseFile(); }

/* Function start: 0x418A20 */
void Parser::Open(char *param_1) {
  CloseFile();
  strcpy(this->filename, param_1);
  this->pFile = fsopen(this->filename, "r");
  this->m_subObject = (void*)1;
  if (this->pFile == NULL) {
    ShowError("Parser::Open - Unable to open file '%s' ", this->filename);
  }
}

/* Function start: 0x418A90 */
void Parser::Copy(Parser *other) {
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
void Parser::CloseFile() {
  if (this->m_subObject != 0 && this->pFile != NULL) {
    fclose(this->pFile);
    this->pFile = NULL;
    this->m_subObject = 0;
  }
}

/* Function start: 0x418B30 */
int Parser::LBLParse(char *param_1) {
  ShowError("%s::LBLParse - Uknown Label\\n'%s'\\nfound in file %s", param_1,
            this->lineNumber, this->filename);
  return 0;
}

void Parser::OnProcessEnd() {}

void Parser::OnProcessStart() {}

/* Function start: 0x418B50 */
void Parser::SaveFilePosition() {
  if (fgetpos(this->pFile, (fpos_t *)&this->savedFilePos) != 0) {
    ShowError("Parser::SaveFilePosition() - unable to save file pos in file %s",
              this->filename);
  }
}

/* Function start: 0x418B80 */
void Parser::RestoreFilePosition() {
  if (fsetpos(this->pFile, (fpos_t *)&this->savedFilePos) != 0) {
    ShowError(
        "Parser::RestoreFilePosition() - unable to restore file pos in file %s",
        this->filename);
  }
}

/* Function start: 0x418BB0 */
void Parser::FindKey(unsigned char *param_1) {
  char local_100[256];

  if (param_1 == NULL) {
    return;
  }

  fseek(this->pFile, 0, 0);

  while (1) {
    if (internal_ReadLine(local_100, 255, this->pFile) == NULL) {
      break;
    }
    sscanf(local_100, "%s", g_Buffer_00436960);
    if (strcmp(g_Buffer_00436960, (char *)param_1) == 0) {
      return;
    }
  }
  ShowError("Parser::FindKey - Unable to find key '%s' in file '%s'", param_1,
            this->filename);
}

/* Function start: 0x418C70 */
int Parser::GetKey(char *line) {
  char key_buffer[64];
  int result = this->isProcessingKey;
  char *start_key = strstr(line, "<<");
  if (start_key) {
    start_key = start_key + 2;
    char *end_key = strstr(line, ">>");
    int key_len = end_key - start_key;
    if (end_key) {
      if (key_len > 0) {
        strncpy(key_buffer, start_key, key_len);
        key_buffer[key_len] = 0;

        if (this->isProcessingKey == 0) {
          strcpy(this->currentKey, key_buffer);
          this->isProcessingKey = 1;
          result = 1;
        } else {
          if (strcmp(this->currentKey, key_buffer) == 0) {
            memset(this->currentKey, 0, sizeof(this->currentKey));
            this->isProcessingKey = 0;
          }
        }
      }
    }
  }
  return result;
}

/* Function start: 0x418D60 */
Parser *ParseFile(Parser *parser, char *filename, char *key_format, ...) {
  char key_buffer[64];
  key_buffer[0] = 0;

  if (key_format != NULL) {
    va_list args;
    va_start(args, key_format);
    vsprintf(key_buffer, key_format, args);
    va_end(args);
  }

  parser->Open(filename);
  Parser::ProcessFile(parser, parser, key_buffer);
  parser->CloseFile();
  return parser;
}

/* Function start: 0x418DC0 */
Parser *Parser::ProcessFile(Parser *self, Parser *dst, char *key_format, ...) {
  char key_buffer[64];
  char line_buffer[256];
  Parser *result;

  self->Copy(dst);

  if (key_format != NULL && *key_format != 0) {
    va_list args;
    va_start(args, key_format);
    vsprintf(key_buffer, key_format, args);
    self->FindKey((unsigned char *)key_buffer);
  }

  self->OnProcessStart();

  while (self->pFile != NULL && (self->pFile->_flag != 0)) {
    char *line = internal_ReadLine(line_buffer, 255, self->pFile);
    if (line == NULL) {
      ShowError(
          "Parser::Parser - premature EOF in '%s'- probably missing END label",
          self->filename);
    }

    if (self->GetKey(line_buffer) == 0) {
      self->lineNumber = (int)line_buffer;
      result = (Parser *)self->LBLParse(line_buffer);
      if (result != NULL) {
        self->OnProcessEnd();
        return result;
      }
    }
  }
  ShowError("Parser::Parser - premature EOF in '%s' - Invalid File Pointer",
            self->filename);
  return NULL;
}
