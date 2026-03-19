#include "GameLoop.h"
#include "Handler.h"
#include "SpriteAction.h"
#include "SC_Question.h"
#include "Memory.h"
#include "SC_Game.h"
#include "SC_Cinematic.h"
#include "SC_OnScreenMessage.h"
#include "SC_BgSnd.h"
#include "SC_Timer.h"
#include "SCI_Inventory.h"
#include "Handler31.h"
#include "SCI_IconBarModule.h"
#include "SC_ZBuffer.h"
#include "SCI_SchoolMenu.h"
#include "SC_CrystalPuzzle.h"
#include "SCI_Schedule.h"
#include "SC_DemoScreen.h"
#include "SCI_PracticeRoom.h"
#include "SC_Detention.h"
#include "SC_SelectHotSpot.h"
#include "SC_DuctNav.h"
#include "SC_SaveLoad.h"
#include "SC_Wahoo.h"
#include "SC_Fan.h"
#include "SC_Slime.h"
#include "SC_FireAlarm.h"
#include "SC_ExtBridge.h"
#include "SC_Rats.h"
#include "SC_DodgeOrville.h"
#include "SC_Pods.h"
#include "SC_SpaceShipNav.h"
#include "SC_Roach.h"
#include "SCI_SearchScreen.h"
#include "SC_FakeRoom.h"
#include "SC_Gauntlet.h"
#include "SC_WordSearch.h"
#include <stdio.h>
#include <string.h>

/* Function start: 0x424050 */
GameLoop::~GameLoop()
{
}

/* Function start: 0x4240A0 */ /* 98.25% match (--full) */
int GameLoop::LBLParse(char* param_1) {
    char local_24[32];
    SpriteAction* action;

    local_24[0] = 0;
    sscanf(param_1, "%s", local_24);

    if (strcmp(local_24, "MESSAGE") == 0) {
        action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(action, this);
        EnqueueSpriteAction(action);
        if (action != 0) {
            delete (SpriteAction*)action;
        }
    }
    else if (strcmp(local_24, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("GameLoop");
    }

    return 0;
}

/* Function start: 0x424240 */
Handler* CreateHandler(int command) {
    Handler* handler = 0;

    switch(command) {
    case 2:
        handler = new SC_Game();
        break;
    case 3:
        handler = new SC_Cinematic();
        break;
    case 4:
        handler = new SC_OnScreenMessage();
        break;
    case 5:
        handler = new SC_BgSnd();
        break;
    case 6:
        handler = new SC_Timer();
        break;
    case 0x1E:
        handler = new SCI_Inventory();
        break;
    case 0x1F:
        handler = new Handler31();
        break;
    case 0x20:
        handler = new SCI_IconBarModule();
        break;
    case 0x22:
        handler = new SC_ZBuffer();
        break;
    case 0x25:
        handler = new SCI_SchoolMenu();
        break;
    case 0x26:
        handler = new SC_CrystalPuzzle();
        break;
    case 0x27:
        handler = new SCI_Schedule();
        break;
    case 0x29:
        handler = new SC_DemoScreen();
        break;
    case 0x2B:
        handler = new SCI_PracticeRoom();
        break;
    case 0x2C:
        handler = new SC_Detention();
        break;
    case 0x2D:
        handler = new SC_SelectHotSpot();
        break;
    case 0x2E:
        handler = new SC_DuctNav();
        break;
    case 0x2F:
        handler = new SC_SaveLoad();
        break;
    case 0x3D:
        handler = new SC_Wahoo();
        break;
    case 0x3E:
        handler = new SC_Fan();
        break;
    case 0x3F:
        handler = new SC_Slime();
        break;
    case 0x40:
        handler = new SC_FireAlarm();
        break;
    case 0x41:
        handler = new SC_ExtBridge();
        break;
    case 0x42:
        handler = new SC_Rats();
        break;
    case 0x43:
        handler = new SC_DodgeOrville();
        break;
    case 0x44:
        handler = new SC_Pods();
        break;
    case 0x46:
        handler = new SC_SpaceShipNav();
        break;
    case 0x47:
        handler = new SC_Roach();
        break;
    case 0x48:
        handler = new SCI_SearchScreen();
        break;
    case 0x49:
        handler = new SC_FakeRoom();
        break;
    case 0x50:
        handler = new SC_Gauntlet();
        break;
    case 0x51:
        handler = new SC_WordSearch();
        break;
    default:
        ShowError("Unknown modual %d", command);
        handler = 0;
        break;
    }
    return handler;
}
