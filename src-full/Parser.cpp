#include "globals.h"
#include "Parser.h"
#include "GameConfig.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int g_ParserCount;          // DAT_00469288
extern void* g_FilePosCache;       // DAT_00469144
extern char* DAT_0046aa00;        // global string buffer

extern void __fastcall FUN_004128f0(void*); // destructor for g_FilePosCache

extern int DAT_00469160;  // preprocessor variable substitution flag

extern int DAT_00469148;  // process file count
extern int DAT_0046914c;  // LBLParse call count
extern int DAT_00469150;  // LBLParse time
extern int DAT_00469154;  // ReadLine time
extern int DAT_00469158;  // FindKey time
extern int DAT_0046915c;  // total process time

/* Function start: 0x4127C0 */
Parser::Parser() {
  memset(&m_vbuffer, 0, sizeof(Parser) - 4);
  g_ParserCount++;
}

/* Function start: 0x4127E0 */
Parser::~Parser() {
  CloseFile();
  if (field_0x3c) {
    ConfigData* p = (ConfigData*)field_0x3c;
    p->~ConfigData();
    FreeMemory((void*)p);
    field_0x3c = 0;
  }
  g_ParserCount--;
  if (g_ParserCount == 0 && g_FilePosCache != 0) {
    void* p = g_FilePosCache;
    FUN_004128f0(p);
    FreeMemory(p);
    g_FilePosCache = 0;
  }
}

/* Function start: 0x412940 */
void Parser::Open(char *param_1) {
  CloseFile();
  strcpy(filename, param_1);
  pFile = fsopen(filename, "r");
  m_subObject = 1;
  if (pFile == NULL) {
    ShowError("Parser::Open - Unable to open file '%s' ", filename);
  }
}

/* Function start: 0x4129B0 */
void Parser::Copy(Parser *other) {
  if (other->pFile != pFile) {
    CloseFile();
  }
  strcpy(filename, other->filename);
  pFile = other->pFile;
  if (pFile == NULL) {
    ShowError("Parser::Open - File '%s' not open", filename);
  }
}

/* Function start: 0x412A20 */
void Parser::CloseFile() {
  if (m_subObject != 0 && pFile != NULL) {
    fclose(pFile);
    pFile = NULL;
    m_subObject = 0;
  }
}

int Parser::LBLParse(char *param_1) {
  ReportUnknownLabel(param_1);
  return 0;
}

/* Function start: 0x412A50 */
void Parser::ReportUnknownLabel(char* name) {
  ShowError("%s::LBLParse - Uknown Label\n'%s'\nfound in file %s",
            name, lineNumber, filename);
}

void Parser::OnProcessEnd() {}

void Parser::OnProcessStart() {}

/* Function start: 0x412A70 */
void Parser::SaveFilePosition() {
  if (fgetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError("Parser::SaveFilePosition()...", filename);
  }
}

/* Function start: 0x412AA0 */
void Parser::RestoreFilePosition() {
  if (fsetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError("Parser::RestoreFilePosition()...", filename);
  }
}

class FilePosCache {
public:
  __int64 Lookup(char* filename, char* key);   // 0x412130
  void Store(char* filename, char* key, int posLo, int posHi);  // 0x412210
};
FilePosCache* g_FilePosCache_46928c = 0;  // DAT_0046928c
__int64 FilePosCache::Lookup(char* filename, char* key) { return -1; }
void FilePosCache::Store(char* filename, char* key, int posLo, int posHi) {}

/* Function start: 0x412AD0 */
void Parser::FindKey(unsigned char *param_1) {
  char local_100[256];
  int pos[2];

  if (param_1 == NULL) {
    return;
  }

  if (field_0x38 & 1) {
    *(__int64*)pos = g_FilePosCache_46928c->Lookup(filename, (char*)param_1);
    if (pos[1] != -1 || pos[0] != -1) {
      fsetpos(pFile, (fpos_t*)pos);
      return;
    }
  }

  fseek(pFile, 0, 0);

  while (1) {
    if (internal_ReadLine(local_100, 255, pFile) == NULL) {
      ShowError("Parser::FindKey - Unable to find key '%s' in file '%s'", param_1, filename);
    }
    sscanf(local_100, " %s ", DAT_0046aa00);
    if (strcmp(DAT_0046aa00, (char*)param_1) == 0) {
      break;
    }
  }

  if (field_0x38 & 1) {
    fgetpos(pFile, (fpos_t*)pos);
    g_FilePosCache_46928c->Store(filename, (char*)param_1, pos[0], pos[1]);
  }
}

/* Function start: 0x413BD0 */
int Parser::GetKey(char* line) {
  char local_100[256];
  int result;

  if (DAT_00469160 != 0 && field_0x08 == 0) {
    strcpy(local_100, line);
    SubstituteVars(local_100, line);
  }

  result = GetTokenType(line);
  if (result == 0) goto done;

  if (isProcessingKey != 0) {
    if (result == 1 && DoCommentsMatch(line) != 0) {
      EndComment();
      isProcessingKey = 0;
    }
    goto return_one;
  }

  switch (result) {
  case 1:
    HandleToken(1, line);
    return 1;
  case 2:
    BeginComment(line, 0);
    return 1;
  case 3:
    HandleToken(3, line);
    return 1;
  case 4:
    HandleToken(4, line);
    return 1;
  case 5:
    EndComment();
    return 1;
  case 6:
    if (field_0x08 != 0) goto return_one;
    HandleToken(6, line);
    return 1;
  case 7:
    if (field_0x08 != 0) goto return_one;
    HandleToken(7, line);
    return 1;
  case 8:
    if (field_0x08 != 0) goto return_one;
    HandleToken(8, line);
    return 1;
  case 9:
    if (field_0x08 != 0) goto return_one;
    HandleToken(9, line);
    return 1;
  case 10:
    if (field_0x08 != 0) goto return_one;
    HandleToken(10, line);
    return 1;
  case 11:
    if (field_0x08 != 0) goto return_one;
    HandleToken(11, line);
    return 1;
  case 12:
    if (field_0x08 != 0) goto return_one;
    return 2;
  case 13:
    if (field_0x08 != 0) goto return_one;
    HandleToken(13, line);
    goto done;
  default:
    ShowError("Parser::DoPreProcessor() - Invalid line %s", line);
    return 1;
  }

return_one:
  return 1;

done:
  return field_0x08 != 0;
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

/* Function start: 0x413E70 */
Parser* Parser::ProcessFile(Parser* self, Parser* dst, char* key_format, ...) {
  char line_buffer[256];
  char key_buffer[60];
  int result;

  if (g_FilePosCache_46928c != 0) {
    self->field_0x38 |= 1;
  }

  DAT_00469148++;
  self->Copy(dst);

  Timer totalTimer;
  Timer lineTimer;

  if (key_format != NULL && key_format[0] != '\0') {
    va_list args;
    va_start(args, key_format);
    vsprintf(key_buffer, key_format, args);
    va_end(args);
    self->FindKey((unsigned char*)key_buffer);
    DAT_00469158 += lineTimer.Update();
  }

  self->OnProcessStart();

  do {
    do {
      if (self->pFile == NULL || ((int*)self->pFile)[3] == 0) {
        ShowError("Parser::Parser - premature EOF in '%s' - Invalid File Pointer", self->filename);
      }

      lineTimer.Reset();

      if (internal_ReadLine(line_buffer, 255, self->pFile) == NULL) {
        if (self->isProcessingKey == 0 && self->field_0x08 == 0) {
          ShowError("Parser::Parser - premature EOF in '%s'- probably missing END label", self->filename);
        } else {
          ShowError("Parser::Parser - premature EOF in '%s' \nprobably missing end of comment or end of block", self->filename);
        }
      }

      DAT_00469154 += lineTimer.Update();
      result = self->GetKey(line_buffer);
    } while (result == 1);

    if (result == 2) break;

    lineTimer.Reset();
    self->lineNumber = (int)line_buffer;
    result = self->LBLParse(line_buffer);
    DAT_00469150 += lineTimer.Update();
    DAT_0046914c++;
  } while (result == 0);

  DAT_0046915c += totalTimer.Update();
  self->OnProcessEnd();

  return (Parser*)result;
}
