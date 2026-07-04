#include "EngineSubsystems.h"
#include "globals.h"
#include "SC_CombatBase.h"
#include "Target.h"
#include "RockThrower.h"
#include "SC_Gauntlet.h"
#include <stdio.h>
#include "string.h"
#include "SoundList.h"

#include "string.h"

#include "Palette.h"

// Also declared in VBuffer.h; local prototype keeps this TU's MSVC 4.20
// register allocation stable.
void SetVideoRes(int, int);

class FuncCaller0x425a90 {
public:
    static void __cdecl Call(int, int);  // 0x425a90
};

/* Function start: 0x434660 */
EngineInfoParser::EngineInfoParser() : Parser() {
  memset(&anchor, 0, 0xc * 4);
}

/* Function start: 0x434740 */
EngineInfoParser::~EngineInfoParser() {
}

/* Function start: 0x434800 */
void EngineInfoParser::OnProcessEnd() {
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
    sscanf(line + 3, "%d", &scrollSpeed);
  } else if (firstChar == 'D') {
    sscanf(line + 3, "%d", &animSpeed);
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
    sscanf(line, "%d %d", (int*)&g_CombatEngine_0046ae78->field_0xD0,
           &g_CombatEngine_0046ae78->reserved_0xD8);
  } else {
    sscanf(line, "%d %d", &g_CombatEngine_0046ae78->hotspotY,
           (int*)&g_CombatEngine_0046ae78->field_0xC8);
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
  int sound = (int)g_SoundList_0046ae68->Register(formatted);

  switch (index) {
  case 0:
    *(int*)&g_CombatEngine_0046ae78->backgroundSound = sound;
    break;
  case 1:
    g_CombatWeapon_0046ae60->m_sound = (Sample*)sound;
    break;
  case 2:
    g_TargetList_0046ae58->defaultStopSound = (Sample*)sound;
    break;
  case 3:
    g_TargetList_0046ae58->defaultProgressSound = (Sample*)sound;
    break;
  case 4:
    g_TargetList_0046ae58->defaultHitSound = (Sample*)sound;
    break;
  case 5:
    g_TargetList_0046ae58->defaultSound = (Sample*)sound;
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
  ((Palette*)g_Palette_0046ae64)->LoadFile(buffer);
}

/* Function start: 0x42F570 */
void __fastcall InitCombatGrid(int param_1) {
    SC_Gauntlet* gaunt = (SC_Gauntlet*)param_1;
    int row = 6;
    GauntletEntry* ge = gaunt->entries;
    do {
        int col = 6;
        do {
            ge->fields[0] = 0;
            ge++;
            col--;
        } while (col != 0);
        row--;
    } while (row != 0);
    gaunt->entries[5].fields[0] = 1;
    gaunt->entries[10].fields[0] = 1;
    gaunt->entries[8].fields[0] = 1;
    gaunt->entries[20].fields[0] = 1;
    gaunt->entries[26].fields[0] = 1;
    gaunt->board.crystalState[0] = 5;
    gaunt->entries[31].fields[0] = 1;
    gaunt->board.crystalState[1] = 0;
    gaunt->cellSprites[6] = 0;
}
