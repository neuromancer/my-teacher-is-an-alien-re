#include "SpaceShipEngine.h"
#include "SpriteAction.h"
#include "globals.h"

extern void ShowError(const char* format, ...);

/* Function start: 0x447C30 */
SpaceShipEngine::SpaceShipEngine() {
}

/* Function start: 0x447CB0 */
SpaceShipEngine::~SpaceShipEngine() {
}

/* Function start: 0x447D00 */
int SpaceShipEngine::HandleAction(SpriteAction* param) {
    int result = 0;
    SpriteAction* action = param;

    if (action->instruction != 0x37) {
        ShowError("EngineSpaceShipNav::OnLogicMessage - unsupported instruction %d",
                  action->instruction);
    }

    if (action->extra1 == 2) {
        SendGameMessage(1, action->mousePos.x, 0x41, 1, 4, 0, 0, 0, 0, 0);
        SendGameMessage(0x20, action->mousePos.y, 0x41, 1, 4, 0, 0, 0, 0, 0);
        result = 1;
    }

    return result;
}
