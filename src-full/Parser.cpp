#include "globals.h"
#include "Parser.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function start: 0x4127C0 */ /* ~86% match */
Parser::Parser() { memset(&m_vbuffer, 0, sizeof(Parser) - 4); }

/* Function start: 0x418A10 */ /* DEMO ONLY - no full game match */
Parser::~Parser() { CloseFile(); }

/* Function start: 0x412940 */ /* ~89% match */
void Parser::Open(char *param_1) {
  char* fn;
  CloseFile();
  fn = filename;
  strcpy(fn, param_1);
  pFile = fsopen(fn, "r");
  m_subObject = (void*)1;
  if (pFile == NULL) {
    ShowError("Parser::Open - Unable to open file '%s' ", fn);
  }
}

/* Function start: 0x4129B0 */ /* ~82% match */
void Parser::Copy(Parser *other) {
  if (pFile != other->pFile) {
    CloseFile();
  }
  strcpy(filename, other->filename);
  pFile = other->pFile;
  if (pFile == NULL) {
    ShowError("Parser::Open - File '%s' not open", filename);
  }
}

/* Function start: 0x418B00 */ /* DEMO ONLY - no full game match */
void Parser::CloseFile() {
  if (m_subObject != 0 && pFile != NULL) {
    fclose(pFile);
    pFile = NULL;
    m_subObject = 0;
  }
}

/* Function start: 0x418B30 */ /* DEMO ONLY - no full game match */
int Parser::LBLParse(char *param_1) {
  return ((int (*)(const char*, ...))ShowError)("%s::LBLParse - Uknown Label\n'%s'\nfound in file %s", param_1,
            lineNumber, filename);
}

void Parser::OnProcessEnd() {}

void Parser::OnProcessStart() {}

/* Function start: 0x418B50 */ /* DEMO ONLY - no full game match */
void Parser::SaveFilePosition() {
  if (fgetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError("Parser::SaveFilePosition() - unable to save file pos in file %s",
              filename);
  }
}

/* Function start: 0x418B80 */ /* DEMO ONLY - no full game match */
void Parser::RestoreFilePosition() {
  if (fsetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError(
        "Parser::RestoreFilePosition() - unable to restore file pos in file %s",
        filename);
  }
}

/* Function start: 0x418BB0 */ /* DEMO ONLY - no full game match */
void Parser::FindKey(unsigned char *param_1) {
  char local_100[256];

  if (param_1 == NULL) {
    return;
  }

  fseek(pFile, 0, 0);

  while (1) {
    if (internal_ReadLine(local_100, 255, pFile) == NULL) {
      break;
    }
    sscanf(local_100, " %s ", g_Buffer_00436960);
    if (strcmp(g_Buffer_00436960, (char *)param_1) == 0) {
      return;
    }
  }
  ShowError("Parser::FindKey - Unable to find key '%s' in file '%s'", param_1,
            filename);
}

/* Function start: 0x418C70 */ /* DEMO ONLY - no full game match */
int Parser::GetKey(char *line) {
  char key_buffer[64];
  int result = isProcessingKey;
  char *start_key = strstr(line, "<<");
  if (start_key) {
    start_key = start_key + 2;
    char *end_key = strstr(line, ">>");
    int key_len = end_key - start_key;
    if (end_key) {
      if (key_len > 0) {
        strncpy(key_buffer, start_key, key_len);
        key_buffer[key_len] = 0;

        if (isProcessingKey == 0) {
          strcpy(currentKey, key_buffer);
          isProcessingKey = 1;
          result = 1;
        } else {
          if (strcmp(currentKey, key_buffer) == 0) {
            memset(currentKey, 0, sizeof(currentKey));
            isProcessingKey = 0;
          }
        }
      }
    }
  }
  return result;
}

/* Function start: 0x413E10 */
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
  Parser *result = Parser::ProcessFile(parser, parser, key_buffer);
  parser->CloseFile();
  return result;
}

/* Function start: 0x418DC0 */ /* DEMO ONLY - no full game match */
Parser *Parser::ProcessFile(Parser *self, Parser *dst, char *key_format, ...) {
  char key_buffer[64];
  char line_buffer[256];
  int result;
  va_list args;

  self->Copy(dst);

  if (key_format != NULL && *key_format != 0) {
    va_start(args, key_format);
    vsprintf(key_buffer, key_format, args);
    va_end(args);
    self->FindKey((unsigned char *)key_buffer);
  }

  self->OnProcessStart();

  do {
    while (1) {
      if (self->pFile == NULL || self->pFile->_flag == 0) {
        ShowError("Parser::Parser - premature EOF in \'%s\' - Invalid File Pointer",
                  self->filename);
      }

      if (internal_ReadLine(line_buffer, 255, self->pFile) == NULL) {
        ShowError("Parser::Parser - premature EOF in \'%s\'- probably missing END label",
                  self->filename);
      }

      if (self->GetKey(line_buffer) == 0) break;
    }

    self->lineNumber = (int)line_buffer;
    result = self->LBLParse(line_buffer);
  } while (result == 0);

  self->OnProcessEnd();
  return (Parser *)result;
}
