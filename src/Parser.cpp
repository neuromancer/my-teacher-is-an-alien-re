#include "globals.h"
#include "Parser.h"
#include "GameConfig.h"
#include "Memory.h"
#include "Timer.h"
#include "ObjectPool.h"
#include "MouseControl.h"
#include "Sprite.h"
#include "TimedEvent.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "SC_Question.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FUN_004128f0 = TimedEventPool destructor wrapper (inlined below)

/* Function start: 0x4127C0 */
Parser::Parser() {
  memset(&m_vbuffer, 0, sizeof(Parser) - 4);
  g_ParserCount++;
}

/* Function start: 0x4127E0 */
Parser::~Parser() {
  CloseFile();
  if (field_0x3c != 0) {
    delete (ConfigData*)field_0x3c;
    field_0x3c = 0;
  }
  g_ParserCount--;
  if (g_ParserCount == 0 && g_FilePosCache != 0) {
    // Original compiles this `delete` with an EH-tracked temp: the pointer is
    // spilled to a stack slot and ~TimedEventPool runs via the shared unwind
    // funclet (mov ecx,[ebp-0x10] / jmp 0x4128F0) before FreeMemory.
    TimedEventPool* volatile pool = g_FilePosCache;
    pool->~TimedEventPool();
    FreeMemory((void*)pool);
    g_FilePosCache = 0;
  }
}

/* Function start: 0x412940 */
void Parser::Open(char *param_1) {
  CloseFile();
  strcpy(filename, param_1);
  pFile = fsopen(filename, "rb");
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

/* Function start: 0x405CD0 */
int Parser::LBLParse(char *param_1) {
  ShowError("Parser:: Must define an LBLParse func");
  return 0;
}

/* Function start: 0x412A50 */
void Parser::ReportUnknownLabel(char* name) {
  // Original bug at 0x412A50: lineNumber is passed to a "%s" slot.
  ShowError("%s::LBLParse - Uknown Label\n'%s'\nfound in file %s",
            name, lineNumber, filename);
}

/* Function start: 0x401150 */
void Parser::OnProcessEnd() {}

/* Function start: 0x401140 */
void Parser::OnProcessStart() {}

/* Function start: 0x412A70 */
void Parser::SaveFilePosition() {
  if (fgetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError("Parser::SaveFilePosition() - unable to save file pos in file %s", filename);
  }
}

/* Function start: 0x412AA0 */
void Parser::RestoreFilePosition() {
  if (fsetpos(pFile, (fpos_t *)&savedFilePos) != 0) {
    ShowError("Parser::RestoreFilePosition() - unable to restore file pos in file %s", filename);
  }
}

// SoundTracker provides Lookup/Store for file position caching
#include "SoundTracker.h"


/* Function start: 0x412AD0 */
void Parser::FindKey(unsigned char *param_1) {
  char local_100[256];
  int pos[2];

  if (param_1 == NULL) {
    return;
  }

  if (field_0x38 & 1) {
    *(__int64*)pos = g_SoundTracker_0046928c->Lookup(filename, (char*)param_1);
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
    sscanf(local_100, " %s ", g_Buffer_0046aa00);
    if (strcmp(g_Buffer_0046aa00, (char*)param_1) == 0) {
      break;
    }
  }

  if (field_0x38 & 1) {
    fgetpos(pFile, (fpos_t*)pos);
    g_SoundTracker_0046928c->Store(filename, (char*)param_1, pos[0], pos[1]);
  }
}

// FindAfterSubstring declared in string.h (0x426570)

/* Function start: 0x413810 */
int Parser::GetTokenType(char* line) {
    char local_40[64];
    char* openTag;
    char* closeTag;
    int len;

    openTag = FindAfterSubstring(line, "<<");
    if (openTag == 0) {
        goto invalid;
    }

    closeTag = strstr(line, ">>");
    len = (int)closeTag - (int)openTag;
    if (closeTag == 0 || len <= 0) {
        goto invalid;
    }

    strncpy(g_Buffer_0046aa00, openTag, len);
    g_Buffer_0046aa00[len] = '\0';
    sscanf(g_Buffer_0046aa00, " %s ", local_40);

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

    ShowError("Parser::GetTokenType - Invaild TOKEN \n'%s'", line);
invalid:
    return 0;
}

/* Function start: 0x413BD0 */
int Parser::GetKey(char* line) {
  char local_100[256];
  int result;

  if (g_VarSubstFlag_00469160 != 0 && field_0x08 == 0) {
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
    HandleToken_IF(line, 0);
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

  if (g_SoundTracker_0046928c != 0) {
    self->field_0x38 |= 1;
  }

  g_ProcessFileCount_00469148++;
  self->Copy(dst);

  Timer totalTimer;
  Timer lineTimer;

  if (key_format != NULL && key_format[0] != '\0') {
    va_list args;
    va_start(args, key_format);
    vsprintf(key_buffer, key_format, args);
    va_end(args);
    self->FindKey((unsigned char*)key_buffer);
    g_FindKeyTime_00469158 += lineTimer.Update();
  }

  self->OnProcessStart();

  do {
    do {
      if (self->pFile == NULL || self->pFile->_flag == 0) {
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

      g_ReadLineTime_00469154 += lineTimer.Update();
      result = self->GetKey(line_buffer);
    } while (result == 1);

    if (result == 2) break;

    lineTimer.Reset();
    self->lineNumber = (int)line_buffer;
    result = self->LBLParse(line_buffer);
    g_LBLParseTime_00469150 += lineTimer.Update();
    g_LBLParseCount_0046914c++;
  } while (result == 0);

  g_TotalProcessTime_0046915c += totalTimer.Update();
  self->OnProcessEnd();

  return (Parser*)result;
}

// Duplicates removed — FindAfterSubstring, g_FilePosCache, g_GameState_0046aa30 declared at top of file

/* Function start: 0x412C00 */
void Parser::BeginComment(char* line) {
    char* start;
    char* end;
    int len;

    start = FindAfterSubstring(line, "<<");
    end = strstr(line, ">>");
    len = end - start;
    if (start == 0 || end == 0 || len <= 0) {
        ShowError("Parser::SaveComment - %s", line);
    }
    strncpy(currentKey, start, len);
    currentKey[len] = '\0';
}

/* Function start: 0x412C60 */
int Parser::DoCommentsMatch(char* line) {
    char localBuf[32];
    char* start = FindAfterSubstring(line, "<<");
    char* end = strstr(line, ">>");
    int len = end - start;
    if (start == 0 || end == 0 || len <= 0) {
        ShowError("Parser::DoCommentsMatch - %s", line);
    }
    strncpy(localBuf, start, len);
    localBuf[len] = '\0';
    if (strcmp(localBuf, currentKey) == 0) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4130E0 */
void Parser::UpdateProcessingState() {
    field_0x08 = 0;
    PooledEvent* volatile node = g_FilePosCache->list.head;
    while (node != 0) {
        PooledEvent* pNode = node;
        node = pNode->next;
        if (!(pNode->field_0x8 & 1)) {
            field_0x08 = 1;
            return;
        }
    }
}

/* Function start: 0x412EB0 */
void Parser::PushConditionalState(int value) {
    int* pool;
    int* freeList;
    int headVal;

    // Ensure g_FilePosCache exists
    if (g_FilePosCache == 0) {
        g_FilePosCache = new TimedEventPool(10);
    }

    pool = (int*)g_FilePosCache;
    headVal = pool[0];
    freeList = &pool[3];
    if (*freeList == 0) {
        int growBy = pool[5];
        int* block = (int*)new char[growBy * 12 + 4];
        *block = pool[4];
        pool[4] = (int)block;
        int cnt = pool[5];
        int stride = cnt * 3;
        cnt--;
        int* p = (int*)((char*)block + stride * 4 - 8);
        if (cnt >= 0) {
            do {
                *p = *freeList;
                cnt--;
                *freeList = (int)p;
                p = (int*)((char*)p - 12);
            } while (cnt >= 0);
        }
    }

    int* node = (int*)*freeList;
    int n = 0;
    *freeList = node[0];
    node[1] = n;
    node[0] = headVal;
    pool[2]++;
    node[2] = n;
    do {
        int prev = n;
        n--;
        if (prev == 0) break;
    } while (1);
    node[2] = value;

    if (pool[0] != 0) {
        ((int*)pool[0])[1] = (int)node;
    } else {
        pool[1] = (int)node;
    }
    pool[0] = (int)node;

    UpdateProcessingState();
}

/* Function start: 0x412FE0 */
int Parser::EndComment() {
    // Ensure g_FilePosCache exists
    if (g_FilePosCache == 0) {
        g_FilePosCache = new TimedEventPool(10);
    }

    if (g_FilePosCache->m_count == 0) {
        ShowError("Parser::Pop - IF/ELSEIF ordering Error in %s", filename);
    }

    // Pop head node
    TimedEventPool* ecxPool = g_FilePosCache;
    PooledEvent* head = ecxPool->list.head;
    int result = head->field_0x8; // saved value
    PooledEvent* next = head->next;
    ecxPool->list.head = next;
    if (next != 0) {
        next->prev = 0;
    } else {
        ecxPool->list.tail = 0;
    }

    { int n = 0; do { int tmp = n; n--; if (tmp == 0) break; } while (1); }

    head->next = ecxPool->m_free_list;
    ecxPool->m_free_list = head;
    ecxPool->m_count--;

    UpdateProcessingState();
    return result;
}

/* Function start: 0x412D00 */
void Parser::HandleToken_IF(char* line, int prevResult) {
    int result;

    if (field_0x08 != 0) {
        goto push_result;
    }

    {
        char gsName[32];
        char gsOp[32];
        int gsValue;
        result = 0;

        char* ifPos = FindAfterSubstring(line, "IF_");
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
        SC_MessageParser msg;
        msg.targetAddress = (int)&action;

        char tempBuf[128];
        sprintf(tempBuf, "ADDRESS\t\tGAMESTATE %s", gsName);
        msg.LBLParse(tempBuf);

        sprintf(tempBuf, "INSTRUCTION   %s", gsOp);
        msg.LBLParse(tempBuf);

        sprintf(tempBuf, "EXTRA1        %d", gsValue);
        msg.LBLParse(tempBuf);

        // Evaluate the condition
        // No-op cast is load-bearing: without it MSVC flips regalloc in
        // SubstituteVars below (100% -> 92.73%).
        prevResult = g_GameState_0046aa30->CheckCondition((SpriteAction*)&action);
    }

push_result:
    if (prevResult != 0) {
        prevResult |= 3;
    }
    PushConditionalState(prevResult);
}

/* Function start: 0x413120 */
#include "Sprite.h"


/* Function start: 0x413120 */
void Parser::HandleToken(int tokenType, char* line) {
    char local_210[128];
    char local_190[128];
    char local_110[32];
    char local_f0[96];
    char local_90[24];
    char* local_74;
    char local_58[32];
    char local_38[32];
    char* local_1c;
    int local_18;
    char* local_14;
    int iVar12;
    char* pcVar6;
    char* pcVar7;
    int result;

    switch (tokenType) {
    case 1:
        BeginComment(line);
        PushConditionalState(0);
        isProcessingKey = 1;
        break;

    case 3:
        result = EndComment();
        if (result & 2) {
            PushConditionalState(result & 0xFFFFFFFE);
        } else {
            HandleToken_IF(line, result);
        }
        break;

    case 4:
        result = EndComment();
        if ((result & 2) != 0) {
            result = result & 0xFFFFFFFE;
        } else {
            result = result | 1;
        }
        PushConditionalState(result);
        break;

    case 6:
        {
            // Original quirk: 'value' is never zero-initialized; the not-found
            // path below reads it uninitialized (and always fails the != 3 check).
            int value;
            int scanResult;
            pcVar7 = FindAfterSubstring(line, "SET_GAMESTATE_");
            if (pcVar7 != 0) {
                scanResult = sscanf(pcVar7, " %s %s %d ", local_38, local_90, &value);
            } else {
                // Original quirk: the not-found path reads the (still zero) %d slot.
                scanResult = value;
            }
            if (scanResult != 3) {
                ShowError("Parser::HandleToken - Invalid SET_GAMESTATE statement '%s'", line);
            }
            SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            action.addressValue = g_GameState_0046aa30->FindState(local_38);
            action.instruction = g_StringState_0046aa38->FindState(local_90);
            action.extra1 = value;
            // No-op cast is load-bearing (see CheckCondition call above).
            g_GameState_0046aa30->SetFromAction((SpriteAction*)&action);
        }
        break;

    case 7:
        {
            // Original quirk (same as SET_GAMESTATE): 'mouseState' is never
            // initialized; the not-found arm reads it as the scan result.
            int mouseState;
            int scanResult;
            pcVar7 = FindAfterSubstring(line, "SET_MOUSE_");
            if (pcVar7 != 0) {
                scanResult = sscanf(pcVar7, " %d", &mouseState);
            } else {
                scanResult = mouseState;
            }
            if (scanResult != 1) {
                ShowError("Parser::HandleToken - Invalid SET_MOUSE statement '%s'", line);
            }
            if (g_Mouse_0046aa18 != 0 && g_Mouse_0046aa18->m_sprite != 0) {
                g_Mouse_0046aa18->m_sprite->ResetAnimation(mouseState, 0);
            }
        }
        break;

    case 8:
        local_14 = strstr(line, "[");
        pcVar6 = FindAfterSubstring(line, "]");
        iVar12 = (int)pcVar6 - (int)local_14;
        if (local_14 == 0 || pcVar6 == 0 || iVar12 <= 0) {
            ShowError("Parser::HandleToken - Invalid GOTO statement. needs [LABEL] '%s'", line);
        }
        strncpy(local_58, local_14, iVar12);
        local_58[iVar12] = 0;
        FindKey((unsigned char*)local_58);
        break;

    case 9:
        if (field_0x3c == 0) {
            TimedEventPool* pool = new TimedEventPool(10);
            field_0x3c = (char*)pool;
        }
        {
            fpos_t filePos;
            ((int*)&filePos)[0] = 0;
            ((int*)&filePos)[1] = 0;
            fgetpos(pFile, &filePos);

            local_18 = ((int*)&filePos)[0];
            local_14 = (char*)((int*)&filePos)[1];
            int* pool = (int*)field_0x3c;
            int headVal = *pool;

            if (pool[3] == 0) {
                int* newBlock = (int*)operator new(pool[5] * 16 + 4);
                *newBlock = pool[4];
                pool[4] = (int)newBlock;
                int cnt = pool[5];
                int stride = cnt * 16;
                cnt--;
                int* p = (int*)((char*)newBlock + stride - 12);
                if (cnt >= 0) {
                    do {
                        *p = pool[3];
                        cnt--;
                        pool[3] = (int)p;
                        p = (int*)((char*)p - 16);
                    } while (cnt >= 0);
                }
            }
            int* node = (int*)pool[3];
            pool[3] = *node;
            int* fp = &node[2];
            node[1] = 0;
            *node = headVal;
            pool[2]++;
            fp[0] = 0;
            fp[1] = 0;
            {
                int dummy = 0;
                do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
            }
            fp[0] = local_18;
            fp[1] = (int)local_14;
            if (*pool != 0) {
                *(int*)(*pool + 4) = (int)node;
            } else {
                pool[1] = (int)node;
            }
            *pool = (int)node;

            local_14 = strstr(line, "[");
            local_74 = FindAfterSubstring(line, "]");
            iVar12 = (int)local_74 - (int)local_14;
            if (local_14 == 0 || local_74 == 0 || iVar12 <= 0) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. needs [LABEL] '%s'", line);
            }
            strncpy((char*)local_58, local_14, iVar12);
            local_58[iVar12] = 0;

            local_14 = strstr(local_74, "(");
            if (local_14 != 0) {
                local_74 = FindAfterSubstring(local_14, ")");
            }
            iVar12 = (int)local_74 - (int)local_14;
            if (local_14 != 0 && local_74 != 0 && iVar12 > 0) {
                if (g_VarSubstFlag_00469160 != 0) {
                    ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot nest variable lists '%s'", line);
                }
                g_VarSubstFlag_00469160 = 1;
                strncpy(local_f0, local_14, iVar12);
                local_f0[iVar12] = 0;
                ParseGosubParams(local_f0);
            }
            FindKey((unsigned char*)local_58);
        }
        break;

    case 10:
        if (field_0x3c == 0 || ((int*)field_0x3c)[2] == 0) {
            ShowError("Parser::HandleToken - Invalid <<_RETURN_>>");
        }
        {
            int* pool = (int*)field_0x3c;
            int* node = (int*)*pool;
            local_14 = (char*)node[3];
            int* fp = &node[2];
            local_18 = fp[0];
            int nextNode = *node;
            *pool = nextNode;
            if (nextNode != 0) {
                *(int*)(nextNode + 4) = 0;
            } else {
                pool[1] = 0;
            }
            {
                int dummy = 0;
                do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
            }
            *node = pool[3];
            pool[3] = (int)node;
            pool[2]--;
            {
                fpos_t restorePos;
                ((int*)&restorePos)[0] = local_18;
                ((int*)&restorePos)[1] = (int)local_14;
                fsetpos(pFile, &restorePos);
            }
            g_VarSubstFlag_00469160 = 0;
        }
        break;

    case 0xB:
        pcVar7 = FindAfterSubstring(line, "HALT_");
        pcVar6 = strstr(line, ">>");
        if (pcVar6 != 0) {
            *pcVar6 = 0;
        }
        ShowError("%s \n%s", filename, pcVar7);
        break;

    case 0xD:
        // Original quirk: local_1c is never initialized; the failure paths
        // below hand the stale value to sprintf (unreached in practice
        // because ShowError fires first on iVar12 != 2).
        iVar12 = 0;
        local_14 = FindAfterSubstring(line, "V_>>");
        if (local_14 == 0) {
            pcVar6 = 0;
        } else {
            pcVar6 = strstr(local_14, "<<");
            if (pcVar6 != 0) {
                local_1c = FindAfterSubstring(pcVar6, ">>");
                if (local_1c != 0) {
                    iVar12 = sscanf(pcVar6 + 2, "%s %s", local_110, local_58);
                    pcVar7 = strstr((char*)local_58, ">>");
                    if (pcVar7 != 0) {
                        *pcVar7 = 0;
                    }
                }
            }
        }
        if (iVar12 != 2) {
            ShowError("Parser::HandleToken - Invalid VARIABLE statement '%s'", line);
        }
        int stateValue;
        if (strcmp((char*)local_110, "GS") == 0) {
            GameState* gameState = g_GameState_0046aa30;
            int idx = gameState->FindState((char*)local_58);
            if (idx < 0 || gameState->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateValue = gameState->stateValues[idx];
        } else {
            ShowError("Parser::HandleToken - Invalid VARIABLE statement '%s'", line);
            stateValue = 0;
        }
        iVar12 = (int)pcVar6 - (int)local_14;
        strncpy(local_190, local_14, iVar12);
        pcVar6 = local_1c;
        local_190[iVar12] = 0;
        sprintf(local_210, "%s %d %s", local_190, stateValue, pcVar6);
        strcpy(line, local_210);
        break;
    }
}

/* Function start: 0x414040 */
void Parser::ParseGosubParams(char* line) {
    int done;
    char* pos;
    char* start;
    int i;

    done = 0;
    i = 0;
    pos = line;

    do {
        start = pos + 1;
        pos = strchr(start, ',');
        if (pos == 0) {
            pos = strchr(start, ')');
            done = 1;
            if (pos == 0) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot fill variable list '%s'", line);
            }
        }
        {
            int len = pos - start;
            if (start == 0 || pos == 0 || len <= 0) {
                ShowError("Parser::HandleToken - Invalid GOSUB statement. cannot fill variable list '%s'", line);
            }
            strncpy(&g_VarSubstBuffer_00469168[i], start, len);
            g_VarSubstBuffer_00469168[i + len] = 0;
            i += 0x20;
        }
        if (start == 0 || done != 0) break;
    } while (1);
}

/* Function start: 0x4140F0 */
void Parser::SubstituteVars(char* src, char* dst) {
    char* pos;

    *dst = 0;
    while (1) {
        pos = strchr(src, '%');
        if (pos == 0) break;
        {
            char* digit = pos + 1;
            int varIdx = *digit - '1';
            strncat(dst, src, pos - src);
            varIdx <<= 5;
            strcat(dst, &g_VarSubstBuffer_00469168[varIdx]);
            src = digit + 1;
        }
    }
    // Original bug at 0x4140F0: pos is null here, so the count is derived from -src.
    strncat(dst, src, pos - src);
}
