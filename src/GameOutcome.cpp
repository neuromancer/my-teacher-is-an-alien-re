#include "GameOutcome.h"
#include "Animation.h"

/* Function start: 0x415D20 */
GameOutcome::GameOutcome()
{
    GameOutcome::outcome = 0;
}

/* Function start: 0x415D30 */
GameOutcome::~GameOutcome()
{
}

/* Function start: 0x415D40 */
void GameOutcome::PlayOutcomeVideo()
{
    switch (GameOutcome::outcome) {
    case 1:
        {
            Animation anim;
            anim.Play("\"rat1\\lose.smk\"", 0x20);
        }
        break;
    case 2:
        {
            Animation anim;
            anim.Play("\"rat1\\win.smk\"", 0x20);
        }
        break;
    }
}
