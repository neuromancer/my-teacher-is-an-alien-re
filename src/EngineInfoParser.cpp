#include "EngineSubsystems.h"
#include "Parser.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

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
  char label[32];
  
  sscanf(line, " %s ", label);

  if (strcmp(label, "END") == 0) {
    return 1;
  }

  if (label[0] == 'A') {
    ParseAnchor(line + 3);
  } else if (label[0] == 'C') {
    sscanf(line + 3, "%d", &field_0xa8);
  } else if (label[0] == 'D') {
    sscanf(line + 3, "%d", &field_0xac);
  } else if (label[0] == 'O') {
    FUN_00416F70(line + 3, *(int*)&label[1]);
  } else if (label[0] == 'P') {
    sscanf(line + 3, "%d %d", &paletteStart, &paletteEnd);
  } else if (label[0] == 'Q') {
    sscanf(line + 3, "%d %d", &field_0xa0, &field_0xa4);
  } else if (label[0] == 'R') {
    FUN_00417130(line + 3);
  } else if (label[0] == 'S') {
    FUN_00417030(line + 3, label[1] - '0');
  } else if (label[0] == 'T') {
    FUN_00417170(line + 1);
  } else if (label[0] == 'V') {
    ParseDimensions(line + 3);
  } else {
    if (label[0] == '\0') {
      return 1;
    }
    FUN_00418B30("MapScene");
  }

  return 0;
}

/* Function start: 0x416FD0 */
void EngineInfoParser::ParseAnchor(char* line) {
  sscanf(line, "%d %d", &anchorX, &anchorY);
}

/* Function start: 0x417000 */
void EngineInfoParser::ParseDimensions(char* line) {
  sscanf(line, "%d %d", &width, &height);
}
