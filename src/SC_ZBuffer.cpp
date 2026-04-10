#include "SC_ZBuffer.h"
#include "SpriteAction.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x414220 */
SC_ZBuffer::SC_ZBuffer() {
}

SC_ZBuffer::~SC_ZBuffer() {
}

/* Function start: 0x414370 */
void SC_ZBuffer::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = ((SpriteAction*)msg)->addressValue;
    }
}

/* Function start: 0x4143A0 */
void SC_ZBuffer::Update(int param1, int param2) {
  unsigned int elapsed;

  elapsed = timer.Update();
  if (elapsed > 10000) {
    SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
  }
  if (handlerId == param2) {
    ShowError("SC_ZBuffer::Update");
  }
}
