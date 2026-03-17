#include "SC_Combat.h"
#include <string.h>

/* Function start: 0x449100 */
SC_Combat::SC_Combat() {
    memset(&statusPtr, 0, 0x70);
}

SC_Combat::~SC_Combat() {
}

/* Function start: 0x449600 */
int SC_Combat::LBLParse(char* line) { return 0; }
/* Function start: 0x449520 */
void SC_Combat::OnProcessEnd() {}
/* Function start: 0x449260 */
void SC_Combat::Init(SC_Message* msg) {}
/* Function start: 0x449400 */
int SC_Combat::AddMessage(SC_Message* msg) { return 0; }
/* Function start: 0x449320 */
int SC_Combat::ShutDown(SC_Message* msg) { return 0; }
/* Function start: 0x449480 */
void SC_Combat::Update(int p1, int p2) {}
/* Function start: 0x449410 */
int SC_Combat::Exit(SC_Message* msg) { return 0; }
/* Function start: 0x4494E0 */
void SC_Combat::ProcessLose() {}
/* Function start: 0x4491B0 */
void SC_Combat::ProcessAction(int action, int* data) {}
