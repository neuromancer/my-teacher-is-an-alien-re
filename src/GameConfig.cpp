#include "GameConfig.h"
#include "Memory.h"
#include <string.h>

extern "C" {
    void* __fastcall FUN_00422690(void* ptr);
    void __fastcall FUN_00422870(void* ptr);
}

void GameConfig::Constructor() {
    FUN_00422690(this);
}

/* Function start: 0x418C70 */
int GameConfig::Init(char* cmdLine) {
    char local_40[64];
    // ESI corresponds to original_state
    int original_state = this->isSet;
    
    char* start = strstr(cmdLine, "-[");
    if (start) {
        start += 2;
        char* end = strstr(cmdLine, "]");
        
        int len = (int)end - (int)start;
        
        if (end && len > 0) {
            strncpy(local_40, start, len);
            local_40[len] = 0;
            
            if (this->isSet == 0) {
                strcpy(this->value, local_40);
                this->isSet = 1;
                original_state = 1;
                goto done;
            } else {
                if (strcmp(this->value, local_40) == 0) {
                    memset(this->value, 0, 32);
                    this->isSet = 0;
                     // original_state is 1 (implied by else), and we return it.
                     // But in assembly, it sets field_C to 0 (EAX=0).
                     // And returns ESI (which is 1).
                }
                // Return original_state.
                goto done;
            }
        }
    }
done:
    return original_state;
}

/* Function start: 0x422870 */
void GameConfig::LoadConfig() {
    FUN_00422870(this);
}
