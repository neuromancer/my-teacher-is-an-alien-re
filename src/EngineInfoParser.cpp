#include "EngineSubsystems.h"
#include "globals.h"
#include <stdio.h>
#include "string.h"
#include "Engine.h"
#include "SoundList.h"
#include "Palette.h"

/* Function start: 0x416BD0 */
EngineInfoParser::EngineInfoParser() : Parser() {
  anchorX = 0;
  anchorY = 199; // 0xc7
  width = 320;   // 0x140
  height = 200;  // 0xc8
  paletteStart = 0;
  paletteEnd = 0;
  field_0xa0 = 0;
  field_0xa4 = 0;
  field_0xa8 = 0;
  field_0xac = 0;
}

/* Function start: 0x416CD0 */
EngineInfoParser::~EngineInfoParser() {
}



/* Function start: 0x416D70 */
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
    sscanf(line + 3, "%d", &field_0xa8);
  } else if (firstChar == 'D') {
    sscanf(line + 3, "%d", &field_0xac);
  } else if (firstChar == 'O') {
    ParseOffset(line + 3, secondChar);
  } else if (firstChar == 'P') {
    sscanf(line + 3, "%d %d", &paletteStart, &paletteEnd);
  } else if (firstChar == 'Q') {
    sscanf(line + 3, "%d %d", &field_0xa0, &field_0xa4);
  } else if (firstChar == 'R') {
    ParsePalette(line + 3);
  } else if (firstChar == 'S') {
    ParseSound(line + 3, secondChar - '0');
  } else if (firstChar == 'T') {
    ParseText(line + 1);
  } else if (firstChar == 'V') {
    ParseDimensions(line + 3);
  } else {
    Parser::LBLParse("MapScene");
  }

  return 0;
}

extern "C" char *FUN_004195c0(char *filename);

/* Function start: 0x416F70 */
void EngineInfoParser::ParseOffset(char *line, int arg2) {
  if ((char)arg2 == '1') {
    sscanf(line, "%d %d", &g_CombatEngine->field_0xc8,
           &g_CombatEngine->field_0xd0);
  } else {
    sscanf(line, "%d %d", &g_CombatEngine->field_0xb8,
           &g_CombatEngine->field_0xc0);
  }
}

/* Function start: 0x416FD0 */
void EngineInfoParser::ParseAnchor(char *line) {
  sscanf(line, "%d %d", &anchorX, &anchorY);
}

/* Function start: 0x417000 */
void EngineInfoParser::ParseDimensions(char *line) {
  sscanf(line, "%d %d", &width, &height);
}

/* Function start: 0x417030 */
void EngineInfoParser::ParseSound(char *line, int index) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  int sound = (int)g_SoundList_00435f1c->Register(buffer);

  switch (index) {
  case 0:
    g_CombatEngine->field_0xe0 = sound;
    break;
  case 1:
    *(int *)((char *)DAT_00435f14 + 0xa4) = sound;
    break;
  case 2:
    *(int *)((char *)DAT_00435f0c + 0x1b4) = sound;
    break;
  case 3:
    *(int *)((char *)DAT_00435f0c + 0x1b8) = sound;
    break;
  case 4:
    *(int *)((char *)DAT_00435f0c + 0x1bc) = sound;
    break;
  case 5:
    *(int *)((char *)DAT_00435f0c + 0x1c0) = sound;
    break;
  default:
    ShowError("MapScene::ParseSound() - Undefined sound type => %s", line);
    break;
  }
}

/* Function start: 0x417130 */
void EngineInfoParser::ParsePalette(char *line) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  g_EnginePalette->Load(FUN_004195c0(buffer));
}

/* Function start: 0x417170 */
void EngineInfoParser::ParseText(char *line) {
}
