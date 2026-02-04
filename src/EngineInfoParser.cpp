#include "EngineSubsystems.h"
#include "globals.h"
#include <stdio.h>
#include "string.h"
#include "Engine.h"
#include "SoundList.h"
#include "Palette.h"
#include "RockThrower.h"

extern "C" char* CDData_FormatPath(char* filename, ...);

/* Function start: 0x416BD0 */
EngineInfoParser::EngineInfoParser() : Parser() {
  memset(&anchorRect, 0, sizeof(GlyphRect) * 3);
  anchorRect.left = 0;
  anchorRect.top = 199; // 0xc7
  anchorRect.right = 320;   // 0x140
  anchorRect.bottom = 200;  // 0xc8
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
    sscanf(line + 3, "%d", &rect_0xa8.left);
  } else if (firstChar == 'D') {
    sscanf(line + 3, "%d", &rect_0xa8.top);
  } else if (firstChar == 'O') {
    ParseOffset(line + 3, secondChar);
  } else if (firstChar == 'P') {
    sscanf(line + 3, "%d %d", &paletteRect.left, &paletteRect.top);
  } else if (firstChar == 'Q') {
    sscanf(line + 3, "%d %d", &paletteRect.right, &paletteRect.bottom);
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

/* Function start: 0x416F70 */
void EngineInfoParser::ParseOffset(char *line, int arg2) {
  if ((char)arg2 == '1') {
    sscanf(line, "%d %d", &g_CombatEngine_00435eb0->m_viewOffset1X,
           &g_CombatEngine_00435eb0->m_viewOffset1Y);
  } else {
    sscanf(line, "%d %d", &g_CombatEngine_00435eb0->m_scrollOffsetX,
           &g_CombatEngine_00435eb0->m_scrollOffsetY);
  }
}

/* Function start: 0x416FD0 */
void EngineInfoParser::ParseAnchor(char *line) {
  sscanf(line, "%d %d", &anchorRect.left, &anchorRect.top);
}

/* Function start: 0x417000 */
void EngineInfoParser::ParseDimensions(char *line) {
  sscanf(line, "%d %d", &anchorRect.right, &anchorRect.bottom);
}

/* Function start: 0x417030 */
void EngineInfoParser::ParseSound(char *line, int index) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  int sound = (int)g_SoundList_00435f1c->Register(buffer);

  switch (index) {
  case 0:
    g_CombatEngine_00435eb0->m_backgroundSample = (Sample*)sound;
    break;
  case 1:
    g_Weapon_00435f14->field_0xa4 = sound;
    break;
  case 2:
    g_TargetList_00435f0c->field_0x1b4 = sound;
    break;
  case 3:
    g_TargetList_00435f0c->field_0x1b8 = sound;
    break;
  case 4:
    g_TargetList_00435f0c->field_0x1bc = sound;
    break;
  case 5:
    g_TargetList_00435f0c->field_0x1c0 = (void*)sound;
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
  g_EnginePalette_00435f18->Load(CDData_FormatPath(buffer));
}

/* Function start: 0x417170 */
void EngineInfoParser::ParseText(char *line) {
}
