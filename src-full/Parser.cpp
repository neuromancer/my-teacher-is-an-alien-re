#include "globals.h"
#include "Parser.h"
#include "GameConfig.h"
#include "Memory.h"
#include "Timer.h"
#include "TimedEvent.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int g_ParserCount;          // DAT_00469288
extern void* g_FilePosCache;
extern "C" extern GameState* DAT_0046aa30;       // DAT_00469144
extern char* DAT_0046aa00;        // global string buffer

// FUN_004128f0 = TimedEventPool destructor wrapper (inlined below)

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
    delete (TimedEventPool*)g_FilePosCache;
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

// FilePosCache moved to FilePosCache.cpp
#include "FilePosCache.h"


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

extern "C" char* FUN_00426570(char*, char*); // case-insensitive strstr (0x426570)

/* Function start: 0x413810 */
int Parser::GetTokenType(char* line) {
    char local_40[64];
    char* openTag;
    char* closeTag;
    int len;

    openTag = FUN_00426570(line, "<<");
    if (openTag == 0) {
        return 0;
    }

    closeTag = strstr(line, ">>");
    len = (int)closeTag - (int)openTag;
    if (closeTag == 0 || len <= 0) {
        return 0;
    }

    strncpy(DAT_0046aa00, openTag, len);
    DAT_0046aa00[len] = '\0';
    sscanf(DAT_0046aa00, " %s ", local_40);

    if (strcmp(local_40, "_IF_") == 0) return 2;
    if (strcmp(local_40, "_ELSEIF_") == 0) return 3;
    if (strcmp(local_40, "_ELSE_") == 0) return 4;
    if (strcmp(local_40, "_ENDIF_") == 0) return 5;
    if (strcmp(local_40, "_SET_GAMESTATE_") == 0) return 6;
    if (strcmp(local_40, "_SET_MOUSE_") == 0) return 7;
    if (strcmp(local_40, "_GOTO_") == 0) return 8;
    if (strcmp(local_40, "_GOSUB_") == 0) return 9;
    if (strcmp(local_40, "_RETURN_") == 0) return 10;
    if (strcmp(local_40, "_EXIT_") == 0) return 0xC;
    if (strcmp(local_40, "_HALT_") == 0) return 0xB;
    if (strcmp(local_40, "_V_") == 0) return 0xD;
    if (strncmp(local_40, "**", 2) == 0) return 1;

    ShowError("Parser::GetTokenType - Invaild TOKEN \n'%s'", local_40);
    return 1;
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

// Duplicates removed — FUN_00426570, g_FilePosCache, DAT_0046aa30 declared at top of file

/* Function start: 0x412C00 */
void Parser::BeginComment(char* line, int flag) {
    char* start = FUN_00426570(line, "/*");
    char* end = strstr(line, "*/");
    int len = end - start;
    if (start == 0 || end == 0 || len <= 0) {
        ShowError("Parser::SaveComment - %s", line);
    }
    memcpy(&isProcessingKey + 2, start, len); // offset +0x10 = commentBuffer
    ((char*)(&isProcessingKey + 2))[len] = '\0';
}

/* Function start: 0x412C60 */
int Parser::DoCommentsMatch(char* line) {
    char localBuf[32];
    char* start = FUN_00426570(line, "/*");
    char* end = strstr(line, "*/");
    int len = end - start;
    if (start == 0 || end == 0 || len <= 0) {
        ShowError("Parser::DoCommentsMatch - %s", line);
    }
    memcpy(localBuf, start, len);
    localBuf[len] = '\0';
    if (strcmp(localBuf, (char*)(&isProcessingKey + 2)) == 0) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4130E0 */
void Parser::UpdateProcessingState() {
    isProcessingKey = 0;
    int* cache = (int*)g_FilePosCache;
    int* node = (int*)cache[0]; // head
    while (node != 0) {
        if (node[2] & 1) {
            node = (int*)node[0]; // next
        } else {
            isProcessingKey = 1;
            return;
        }
    }
}

/* Function start: 0x412EB0 */
void Parser::PushConditionalState(int value) {
    // Ensure g_FilePosCache exists
    if (g_FilePosCache == 0) {
        g_FilePosCache = (void*)new TimedEventPool(10);
    }

    // Allocate node from pool
    TimedEventPool* pool = (TimedEventPool*)g_FilePosCache;
    int* freeList = (int*)&pool->m_free_list;
    if (*freeList == 0) {
        // Grow pool
        int growBy = pool->m_pool_size;
        int* block = (int*)AllocateMemory(growBy * 12 + 4);
        *block = (int)pool->m_pool;
        pool->m_pool = (PooledEvent*)block;
        int i = growBy - 1;
        int* p = (int*)((char*)block + growBy * 12 - 8);
        while (i >= 0) {
            *p = *freeList;
            *freeList = (int)p;
            p -= 3;
            i--;
        }
    }

    int* node = (int*)*freeList;
    *freeList = node[0];
    node[1] = 0;
    node[0] = (int)pool->list.head;
    pool->m_count++;
    node[2] = 0;

    // Zero loop (matches assembly volatile pattern)
    { volatile int n = 0; while (n-- != 0); }

    node[2] = value;

    // Link to head
    if (pool->list.head != 0) {
        ((int*)pool->list.head)[1] = (int)node;
    } else {
        pool->list.tail = (PooledEvent*)node;
    }
    pool->list.head = (PooledEvent*)node;

    UpdateProcessingState();
}

/* Function start: 0x412FE0 */
int Parser::EndComment() {
    // Ensure g_FilePosCache exists
    if (g_FilePosCache == 0) {
        g_FilePosCache = (void*)new TimedEventPool(10);
    }

    TimedEventPool* pool = (TimedEventPool*)g_FilePosCache;
    if (pool->m_count == 0) {
        ShowError("Parser::Pop - IF/ELSEIF ordering Error in %s", filename);
    }

    // Pop head node
    int* head = (int*)pool->list.head;
    int result = head[2]; // saved value
    int* next = (int*)head[0];
    pool->list.head = (PooledEvent*)next;
    if (next != 0) {
        next[1] = 0;
    } else {
        pool->list.tail = 0;
    }

    // Zero loop (matches assembly volatile pattern)
    { volatile int n = 0; while (n-- != 0); }

    // Return to free list
    head[0] = (int)pool->m_free_list;
    pool->m_free_list = (PooledEvent*)head;
    pool->m_count--;

    UpdateProcessingState();
    return result;
}

/* Function start: 0x412D00 */
void Parser::HandleToken_IF(char* line, int prevResult) {
    int result;

    if (isProcessingKey != 0) {
        goto push_result;
    }

    {
        char gsName[128];
        char gsOp[32];
        int gsValue;
        result = 0;

        char* ifPos = FUN_00426570(line, "GAMESTATE");
        if (ifPos == 0) {
            goto check_params;
        }
        result = sscanf(ifPos, " GAMESTATE %s %s %d", gsName, gsOp, &gsValue);

    check_params:
        if (result != 3) {
            ShowError("Parser::HandleToken_IF - Invaild IF/ELSEIF statement '%s'", line);
        }

        // Build SpriteAction with parsed values
        SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        Parser temp;

        // Format: "ADDRESS\t\tGAMESTATE %s"
        char tempBuf[256];
        sprintf(tempBuf, "ADDRESS\t\tGAMESTATE %s", gsName);
        temp.LBLParse(tempBuf);

        sprintf(tempBuf, "INSTRUCTION   %s", gsOp);
        temp.LBLParse(tempBuf);

        sprintf(tempBuf, "EXTRA1        %d", gsValue);
        temp.LBLParse(tempBuf);

        // Evaluate the condition
        result = 0; // TODO: condition evaluation
    }

push_result:
    if (prevResult != 0) {
        prevResult |= 3;
    }
    PushConditionalState(prevResult);
}

/* Function start: 0x413120 */
#include "Sprite.h"

extern "C" extern GameState* DAT_0046aa30;
extern GameState* DAT_0046aa38;
extern MouseControl* DAT_0046aa18;
extern int DAT_00469160;
extern void __stdcall ParseGosubParams(char* line);

extern "C" char* FUN_00426570(char* s1, char* s2);
extern "C" char* strstr(const char*, const char*);
extern void __fastcall FUN_00433cb0(void*, int, int);

/* Function start: 0x413120 */
void Parser::HandleToken(int tokenType, char* line) {
    char local_210[128];
    char local_190[128];
    char local_110[32];
    char local_f0[96];
    char local_90[24];
    SpriteAction local_70;
    char* local_74;
    int local_6c;
    int local_60;
    char* local_5c;
    char local_58[32];
    char local_38[24];
    int local_20;
    char* local_1c;
    int local_18;
    char* local_14;
    int iVar12;
    char* pcVar6;
    char* pcVar7;
    int result;

    switch (tokenType) {
    case 1:
        BeginComment(line, 0);
        PushConditionalState(0);
        isProcessingKey = 1;
        break;

    case 3:
        result = EndComment();
        if ((result & 2) == 0) {
            HandleToken_IF(line, result);
        } else {
            PushConditionalState(result & 0xFFFFFFFE);
        }
        break;

    case 4:
        result = EndComment();
        if ((result & 2) == 0) {
            result = result | 1;
        } else {
            result = result & 0xFFFFFFFE;
        }
        PushConditionalState(result);
        break;

    case 6:
        pcVar7 = FUN_00426570(line, "SET_GAMESTATE_");
        pcVar6 = local_14;
        if (pcVar7 != 0) {
            pcVar6 = (char*)sscanf(pcVar7, " %s %s %d", local_38, local_90, &local_14);
        }
        if (pcVar6 != (char*)3) {
            ShowError("Parser::HandleToken - Invalid SET_GAMESTATE statement '%s'");
        }
        {
            SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            local_6c = DAT_0046aa30->FindState(local_38);
            local_60 = DAT_0046aa38->FindState(local_90);
            local_5c = local_14;
            FUN_00433cb0(DAT_0046aa30, 0, (int)&action);
        }
        break;

    case 7:
        pcVar7 = FUN_00426570(line, "SET_MOUSE_");
        pcVar6 = local_14;
        if (pcVar7 != 0) {
            pcVar6 = (char*)sscanf(pcVar7, " %d", &local_14);
        }
        if (pcVar6 != (char*)1) {
            ShowError("Parser::HandleToken - Invalid SET_MOUSE statement '%s'");
        }
        if (DAT_0046aa18 != 0 && *(void**)((char*)DAT_0046aa18 + 0x94) != 0) {
            ((Sprite*)*(void**)((char*)DAT_0046aa18 + 0x94))->ResetAnimation((int)local_14, 0);
        }
        break;

    case 8:
        local_14 = strstr(line, "(");
        pcVar6 = FUN_00426570(line, ")");
        iVar12 = (int)pcVar6 - (int)local_14;
        if (local_14 == 0 || pcVar6 == 0 || iVar12 < 1) {
            ShowError("Parser::HandleToken - Invalid GOTO statement. cannot find sub name '%s'");
        }
        strncpy((char*)local_90, local_14, iVar12);
        local_90[iVar12] = 0;
        FindKey((unsigned char*)local_90);
        break;

    case 9:
        if (field_0x3c == 0) {
            local_14 = (char*)operator new(0x18);
            if (local_14 == 0) {
                pcVar6 = 0;
            } else {
                memset(local_14, 0, 0x18);
                *(int*)(local_14 + 0x14) = 10;
                pcVar6 = local_14;
            }
            field_0x3c = (int)pcVar6;
        }
        {
            int filePos[2];
            filePos[0] = 0;
            filePos[1] = 0;
            fgetpos(pFile, (fpos_t*)filePos);

            int* pool = (int*)field_0x3c;
            int headVal = *pool;
            local_18 = filePos[0];
            local_14 = (char*)filePos[1];

            if (pool[3] == 0) {
                int* newBlock = (int*)operator new(pool[5] * 16 + 4);
                *newBlock = pool[4];
                pool[4] = (int)newBlock;
                int cnt = pool[5];
                int* p = newBlock + cnt * 4 - 3;
                while (cnt-- > 0) {
                    *p = pool[3];
                    pool[3] = (int)p;
                    p -= 4;
                }
            }
            int* node = (int*)pool[3];
            pool[3] = *node;
            node[1] = 0;
            *node = headVal;
            pool[2]++;
            node[2] = 0;
            node[3] = 0;
            {
                int dummy = 0;
                do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
            }
            node[2] = local_18;
            node[3] = (int)local_14;
            if (*pool == 0) {
                pool[1] = (int)node;
            } else {
                *(int*)(*pool + 4) = (int)node;
            }
            *pool = (int)node;

            local_14 = strstr(line, "(");
            local_74 = FUN_00426570(line, ")");
            iVar12 = (int)local_74 - (int)local_14;
            if (local_14 == 0 || local_74 == 0 || iVar12 < 1) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot find sub name '%s'");
            }
            strncpy((char*)local_58, local_14, iVar12);
            local_58[iVar12] = 0;

            local_14 = strstr(local_74, "(");
            if (local_14 != 0) {
                local_74 = FUN_00426570(local_14, ")");
            }
            iVar12 = (int)local_74 - (int)local_14;
            if (local_14 != 0 && local_74 != 0 && iVar12 > 0) {
                if (DAT_00469160 != 0) {
                    ShowError("Parser::HandleToken - Invalid GOSUB statement. Nested sub not allowed '%s'");
                }
                DAT_00469160 = 1;
                strncpy(local_f0, local_14, iVar12);
                local_f0[iVar12] = 0;
                ParseGosubParams(local_f0);
            }
            FindKey((unsigned char*)local_58);
        }
        break;

    case 10:
        if (field_0x3c == 0 || *(int*)(field_0x3c + 8) == 0) {
            ShowError("Parser::HandleToken - Invalid <<_RETURN_>>");
        }
        {
            int* pool = (int*)field_0x3c;
            int* node = (int*)*pool;
            local_74 = (char*)node[3];
            int savedPos = node[2];
            int nextNode = *node;
            *pool = nextNode;
            if (nextNode == 0) {
                pool[1] = 0;
            } else {
                *(int*)(nextNode + 4) = 0;
            }
            {
                int dummy = 0;
                do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
            }
            *node = pool[3];
            pool[3] = (int)node;
            pool[2]--;
            local_18 = savedPos;
            local_14 = local_74;
            fsetpos(pFile, (fpos_t*)&savedPos);
            DAT_00469160 = 0;
        }
        break;

    case 0xB:
        FUN_00426570(line, "HALT_");
        pcVar6 = strstr(line, "\0");
        if (pcVar6 != 0) {
            *pcVar6 = 0;
        }
        ShowError(" %s %s");
        break;

    case 0xD:
        iVar12 = 0;
        local_14 = FUN_00426570(line, "=");
        if (local_14 == 0) {
            pcVar6 = 0;
        } else {
            pcVar6 = strstr(local_14, "(");
            if (pcVar6 != 0) {
                local_1c = FUN_00426570(pcVar6, ")");
                if (local_1c != 0) {
                    iVar12 = sscanf(pcVar6 + 2, "%s %s", local_110, local_58);
                    pcVar7 = strstr((char*)local_58, ")");
                    if (pcVar7 != 0) {
                        *pcVar7 = 0;
                    }
                }
            }
        }
        if (iVar12 != 2) {
            ShowError("Parser::HandleToken - Invalid VARIABLE statement '%s'");
        }
        if (strcmp((char*)local_110, "INT") == 0) {
            local_74 = (char*)DAT_0046aa30;
            int idx = DAT_0046aa30->FindState((char*)local_58);
            if (idx < 0 || DAT_0046aa30->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            pcVar7 = (char*)DAT_0046aa30->stateValues[idx];
        } else {
            ShowError("Parser::HandleToken - Invalid VARIABLE statement '%s'");
            pcVar7 = local_14;
        }
        iVar12 = (int)pcVar6 - (int)local_14;
        strncpy(local_190, local_14, iVar12);
        pcVar6 = local_1c;
        local_190[iVar12] = 0;
        sprintf(local_210, "%s_%d_%s", local_190, pcVar7, pcVar6);
        strcpy(line, local_210);
        break;
    }
}

extern char DAT_00469168[160];

/* Function start: 0x414040 */
void __stdcall ParseGosubParams(char* line) {
    int done;
    char* start;
    int i;

    done = 0;
    i = 0;

    do {
        start = line + 1;
        line = strchr(start, ',');
        if (line == 0) {
            line = strchr(start, ')');
            done = 1;
            if (line == 0) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot fill variable list '%s'", line);
            }
        }
        {
            int len = line - start;
            if (start == 0 || line == 0 || len <= 0) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot fill variable list '%s'", line);
            }
            strncpy(&DAT_00469168[i], start, len);
            DAT_00469168[i + len] = 0;
            i += 0x20;
        }
        if (start == 0 || done != 0) break;
    } while (1);
}

/* Function start: 0x4140F0 */
void Parser::SubstituteVars(char* src, char* dst) {
    char* pos;
    int varIdx;
    char* varStr;
    int varLen;

    *dst = 0;
    while (1) {
        pos = strchr(src, '%');
        if (pos == 0) break;
        strncat(dst, src, pos - src);
        varIdx = *(pos + 1) - '1';
        varStr = &DAT_00469168[varIdx * 0x20];
        varLen = strlen(varStr);
        strcat(dst, varStr);
        src = pos + 2;
    }
    strncat(dst, src, -1 - (int)src);
}
