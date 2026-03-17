#include "EngineSubsystems.h"
#include "globals.h"
#include "SC_CombatBase.h"
#include <stdio.h>
#include "string.h"
#include "SoundList.h"

extern int DAT_0046ae78;
extern char* __cdecl FUN_0044e470(char*);

class PaletteLoader {
public:
    void Load(char* path);  // 0x41dcc0
};

class FuncCaller0x425a90 {
public:
    static void __cdecl Call(int, int);  // 0x425a90
};
extern "C" void SetVideoRes(int, int);

/* Function start: 0x434660 */
EngineInfoParser::EngineInfoParser() : Parser() {
  memset(&anchor, 0, 0xc * 4);
}

/* Function start: 0x434740 */
EngineInfoParser::~EngineInfoParser() {
}

/* Function start: 0x434800 */
void EngineInfoParser::SetupDimensions() {
    SetVideoRes(EngineInfoParser::dimensions.field_0, EngineInfoParser::dimensions.field_4);
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
    sscanf(line + 3, "%d %d", &paletteStart.field_0, &paletteStart.field_4);
  } else if (firstChar == 'Q') {
    sscanf(line + 3, "%d %d", &paletteEnd.field_0, &paletteEnd.field_4);
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
    sscanf(line, "%d %d", (int*)(DAT_0046ae78 + 0xD0),
           (int*)(DAT_0046ae78 + 0xD8));
  } else {
    sscanf(line, "%d %d", (int*)(DAT_0046ae78 + 0xC0),
           (int*)(DAT_0046ae78 + 0xC8));
  }
}

/* Function start: 0x434A60 */
void EngineInfoParser::ParseAnchor(char *line) {
  sscanf(line, "%d %d", &anchor.field_0, &anchor.field_4);
}

/* Function start: 0x434A90 */
void EngineInfoParser::ParseDimensions(char *line) {
  sscanf(line, "%d %d", &dimensions.field_0, &dimensions.field_4);
}

/* Function start: 0x434AC0 */
void EngineInfoParser::ParseSound(char *line, int index) {
  char buffer[128];
  sscanf(line, "%s", buffer);
  char* path = FUN_0044e470(buffer);
  char* formatted = FormatStringVA(path);
  int sound = (int)((SoundList*)DAT_0046ae68)->Register(formatted);

  switch (index) {
  case 0:
    *(int*)(DAT_0046ae78 + 0xEC) = sound;
    break;
  case 1:
    *(int*)((char*)DAT_0046ae60 + 0xAC) = sound;
    break;
  case 2:
    *(int*)((char*)DAT_0046ae58 + 0x1BC) = sound;
    break;
  case 3:
    *(int*)((char*)DAT_0046ae58 + 0x1C0) = sound;
    break;
  case 4:
    *(int*)((char*)DAT_0046ae58 + 0x1C4) = sound;
    break;
  case 5:
    *(int*)((char*)DAT_0046ae58 + 0x1C8) = sound;
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
  ((PaletteLoader*)DAT_0046ae64)->Load(buffer);
}
