#include "EngineSubsystems.h"
#include "globals.h"
#include "SC_CombatBase.h"
#include <stdio.h>
#include "string.h"
#include "SoundList.h"

extern "C" int g_CombatEngine_0046ae78;
#include "string.h"

#include "Palette.h"

class FuncCaller0x425a90 {
public:
    static void __cdecl Call(int, int);  // 0x425a90
};
extern "C" void SetVideoRes(int, int);

// 0x434660 = Projectile::Projectile (COMDAT)
EngineInfoParser::EngineInfoParser() : Parser() {
  memset(&anchor, 0, 0xc * 4);
}

/* Function start: 0x434740 */
EngineInfoParser::~EngineInfoParser() {
}

/* Function start: 0x434800 */
void EngineInfoParser::SetupDimensions() {
    SetVideoRes(EngineInfoParser::dimensions.x, EngineInfoParser::dimensions.y);
}

/* Function start: 0x434820 */
int EngineInfoParser::LBLParse(char* line) {
  char secondChar;
  char firstChar;
  char label[32];

  secondChar = ';';
  sscanf(line, " %s ", label);
  sscanf(label, "%c%c", &firstChar, &secondChar);

  if (strcmp(label, "END") == 0) {
    return 1;
  }

  if (firstChar == 'A') {
    ParseAnchor(line + 3);
  } else if (firstChar == 'C') {
    sscanf(line + 3, "%d", &field_0xB0);
  } else if (firstChar == 'D') {
    sscanf(line + 3, "%d", &field_0xB4);
  } else if (firstChar == 'O') {
    ParseOffset(line + 3, secondChar);
  } else if (firstChar == 'P') {
    sscanf(line + 3, "%d %d", &paletteStart.x, &paletteStart.y);
  } else if (firstChar == 'Q') {
    sscanf(line + 3, "%d %d", &paletteEnd.x, &paletteEnd.y);
  } else if (firstChar == 'R') {
    ParsePalette(line + 3);
  } else if (firstChar == 'S') {
    ParseSound(line + 3, secondChar - '0');
  } else if (firstChar == 'V') {
    ParseDimensions(line + 3);
  } else {
    Parser::ReportUnknownLabel("MapScene");
  }

  return 0;
}

/* Function start: 0x434A00 */
void EngineInfoParser::ParseOffset(char *line, int arg2) {
  if ((char)arg2 == '1') {
    sscanf(line, "%d %d", (int*)(g_CombatEngine_0046ae78 + 0xD0),
           (int*)(g_CombatEngine_0046ae78 + 0xD8));
  } else {
    sscanf(line, "%d %d", (int*)(g_CombatEngine_0046ae78 + 0xC0),
           (int*)(g_CombatEngine_0046ae78 + 0xC8));
  }
}

/* Function start: 0x434A60 */
void EngineInfoParser::ParseAnchor(char *line) {
  sscanf(line, "%d %d", &anchor.x, &anchor.y);
}

/* Function start: 0x434A90 */
void EngineInfoParser::ParseDimensions(char *line) {
  sscanf(line, "%d %d", &dimensions.x, &dimensions.y);
}

/* Function start: 0x434AC0 */
void EngineInfoParser::ParseSound(char *line, int index) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  char* path = MakeAudioName(buffer);
  char* formatted = FormatStringVA(path);
  int sound = (int)DAT_0046ae68->Register(formatted);

  switch (index) {
  case 0:
    *(int*)(g_CombatEngine_0046ae78 + 0xEC) = sound;
    break;
  case 1:
    *(int*)((char*)g_CombatWeapon_0046ae60 + 0xAC) = sound;
    break;
  case 2:
    *(int*)((char*)g_TargetList_0046ae58 + 0x1BC) = sound;
    break;
  case 3:
    *(int*)((char*)g_TargetList_0046ae58 + 0x1C0) = sound;
    break;
  case 4:
    *(int*)((char*)g_TargetList_0046ae58 + 0x1C4) = sound;
    break;
  case 5:
    *(int*)((char*)g_TargetList_0046ae58 + 0x1C8) = sound;
    break;
  default:
    ShowError("MapScene::ParseSound() - Undefined sound type => %s", line);
    break;
  }
}

/* Function start: 0x434BD0 */
void EngineInfoParser::ParsePalette(char *line) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  ((Palette*)DAT_0046ae64)->LoadFile(buffer);
}

/* Function start: 0x42F570 */
void __fastcall InitCombatGrid(int param_1) {
    int row = 6;
    int* ptr = (int*)(param_1 + 0x13C);
    do {
        int col = 6;
        do {
            *ptr = 0;
            ptr += 7;
            col--;
        } while (col != 0);
        row--;
    } while (row != 0);
    *(int*)(param_1 + 0x1C8) = 1;
    *(int*)(param_1 + 0x120) = 5;
    *(int*)(param_1 + 0x4A0) = 1;
    *(int*)(param_1 + 0x124) = 0;
}
