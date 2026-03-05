#include "SCI_SchoolMenu.h"
#include "SpriteAction.h"
#include <string.h>
#include <stdio.h>
#include "GameState.h"
#include "globals.h"
#include "Sprite.h"
#include "Sample.h"
#include "Timer.h"
#include "Palette.h"

// extern globals - C linkage (matching stubs.cpp)
extern "C" {
    extern void* DAT_0046aa30;
    extern int DAT_0046a190;
    extern void* DAT_0046af08;
    extern int DAT_00472d20;
}
// extern globals - C++ linkage (matching stubs.cpp)
extern void* DAT_0046aa24;
extern int DAT_00472d58;
extern int DAT_0046cb90;
extern char* DAT_0046cb94;
extern int DAT_00473334;
extern int DAT_00473358;
extern int DAT_004733e8;
extern int DAT_0047337c;

// extern functions (single-param __fastcall == thiscall with 0 stack params)
extern void __fastcall FUN_0041dc10(void*);
extern void __fastcall FUN_00443990(void*);
extern void __fastcall FUN_00420d90(void*);
extern void __fastcall FUN_00424ee0(void*);
extern void __fastcall FUN_00424ed0(void*);
extern void __fastcall FUN_004250e0(void*);
extern void __fastcall FUN_00421020(void*);
extern void __fastcall FUN_00421880(void*);
extern void __fastcall FUN_004218b0(void*);
extern void* __fastcall FUN_0041dbe0(void*);
extern void* __fastcall FUN_004438a0(void*);
extern void __fastcall FUN_00401130(void*);

// Thiscall wrapper classes for correct calling convention (no real class header found)
class GSVal { public: void FUN_00409f20(int); int GetStateValue(int); };
class QObj { public: void FUN_00444920(void*); };
class SprInit { public: void FUN_00420ce0(int); };
class HitRect { public: int HitTest(int, int); };

// extern "C" functions (matching stubs.cpp)
extern "C" {
    void FUN_00413e10(void*, char*, char*, ...);
    int FUN_00425fa0(void*);
    void FUN_004265a0();
    void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
    char* FUN_0044e530(int);
    void FUN_00454400(void*);
    void* FUN_00454500(int);
    int FUN_00454510(char*, char*, ...);
    int FUN_00454850(char*, char*, ...);
    char* FUN_00454960(char*, char*);
    void FUN_00444e20(void*);
}
// C++ linkage functions (matching stubs.cpp)
extern void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int);
extern void __cdecl FUN_00413e70(void*, int, char*);
extern void __cdecl FUN_00425c50(char*, ...);
extern void __cdecl FUN_00425d70(char*, ...);

extern void* __fastcall FUN_00403520(void*);
extern void* __fastcall FUN_004035a0(void*);
extern void* __fastcall FUN_00403620(void*);
extern void __fastcall FUN_00401c80(void*);

/* Function start: 0x41E030 */
SCI_SchoolMenu::SCI_SchoolMenu() {
    memset(&field_A8, 0, 0x50);
    handlerId = 0x25;
    moduleParam = 1;
}

/* Function start: 0x41E0E0 */
SCI_SchoolMenu::~SCI_SchoolMenu() {
    void* ptr;
    int i;

    ptr = (void*)field_A8;
    if (ptr != 0) {
        FUN_0041dc10(ptr);
        FUN_00454400(ptr);
        field_A8 = 0;
    }

    ptr = (void*)field_AC;
    if (ptr != 0) {
        FUN_00443990(ptr);
        FUN_00454400(ptr);
        field_AC = 0;
    }

    ptr = (void*)field_B0;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        FUN_00454400(ptr);
        field_B0 = 0;
    }

    ptr = (void*)field_B4;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        FUN_00454400(ptr);
        field_B4 = 0;
    }

    ptr = (void*)field_E8;
    if (ptr != 0) {
        FUN_00424ee0(ptr);
        FUN_00454400(ptr);
        field_E8 = 0;
    }

    for (i = 3; i != 0; i--) {
        ptr = (void*)characters[3 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            FUN_00454400(ptr);
            characters[3 - i] = 0;
        }
    }

    for (i = 9; i != 0; i--) {
        ptr = (void*)options[9 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            FUN_00454400(ptr);
            options[9 - i] = 0;
        }
    }
}

/* Function start: 0x41E270 */
void SCI_SchoolMenu::Init(SC_Message* msg) {
    int periodIdx;
    int periodVal;
    int* charSprite;
    void* ptr;
    void* gs;
    int i;

    if (DAT_0046a190 != 0) {
        return;
    }
    DAT_0046a190 = 1;

    IconBar::InitIconBar(msg);

    int* arr = &DAT_00473334;
    do {
        *arr = 1;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&DAT_004733e8);
    DAT_00473334 = 0;
    DAT_00473358 = 0;

    gs = DAT_0046aa30;
    periodIdx = ((GameState*)gs)->FindState("PERIOD");
    ((GSVal*)gs)->FUN_00409f20(periodIdx);
    periodVal = ((int*)((int)gs + 0x90))[0];
    periodVal = ((int*)periodVal)[periodIdx];
    if (periodVal == 1) {
        DAT_0047337c = 0;
    } else if (periodVal > 1) {
        DAT_0047337c = 1;
    }

    FUN_00413e10(this, "mis\\skool.mis", "[DURINGSCHOOLMENU]");
    FUN_00413e10(this, "mis\\skool.mis", "[ROOMAVALIBLITY_PERIOD%d]", periodVal);

    int palVal = field_A8;
    if (palVal != 0) {
        int* dstPtr = (int*)((int)DAT_0046aa24 + 0xa8);
        if (*dstPtr != 0) {
            FUN_00425d70("ddouble palette");
        }
        *dstPtr = palVal;
    }

    // Clean up existing sound
    ptr = (void*)field_E8;
    if (ptr != 0) {
        FUN_00424ee0(ptr);
        FUN_00454400(ptr);
        field_E8 = 0;
    }

    // Allocate new sound object
    void* sndMem = FUN_00454500(0x10);
    void* sndObj = 0;
    if (sndMem != 0) {
        FUN_00424ed0(sndMem);
        sndObj = sndMem;
    }
    field_E8 = (int)sndObj;
    selectedOption = -1;

    // Send initial message
    FUN_00444d90(5, 0x455, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);

    // Reset character sprites
    i = 0;
    charSprite = characters;
    do {
        int chr = *charSprite;
        if (chr != 0) {
            ((int*)chr)[0x25] = 0;
            void* animCtrl = (void*)((int*)chr)[0x67];
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(0, 0);
            }
        }
        charSprite++;
        i++;
    } while (i < 3);

    // Select current character
    gs = DAT_0046aa30;
    int charIdx = DAT_0046cb90;
    ((GSVal*)gs)->FUN_00409f20(charIdx);
    int* stateVals = (int*)((int)gs + 0x90);
    stateVals = (int*)*stateVals;
    int selChar = stateVals[charIdx];
    if (characters[selChar] != 0 && characters[i] != 0) {
        gs = DAT_0046aa30;
        charIdx = DAT_0046cb90;
        ((GSVal*)gs)->FUN_00409f20(charIdx);
        stateVals = (int*)((int)gs + 0x90);
        stateVals = (int*)*stateVals;
        int chrPtr = characters[stateVals[charIdx]];
        ((int*)chrPtr)[0x25] = 1;
        void* animCtrl = (void*)((int*)chrPtr)[0x67];
        if (animCtrl != 0) {
            ((Sprite*)animCtrl)->ResetAnimation(1, 0);
        }
    }

    // Set cursor
    charIdx = DAT_0046cb90;
    gs = DAT_0046aa30;
    ((GSVal*)gs)->FUN_00409f20(charIdx);
    stateVals = (int*)((int)gs + 0x90);
    stateVals = (int*)*stateVals;
    ((Sprite*)DAT_0046af08)->ResetAnimation(stateVals[charIdx] + 1, 0);

    SetupOptions();

    // Check AWARE_TEACHER state
    gs = DAT_0046aa30;
    int awareIdx = ((GameState*)gs)->FindState("AWARE_TEACHER");
    ((GSVal*)gs)->FUN_00409f20(awareIdx);
    stateVals = (int*)((int)gs + 0x90);
    stateVals = (int*)*stateVals;
    if (stateVals[awareIdx] == 0) {
        // Check TEACHER_ALIEN state
        gs = DAT_0046aa30;
        int alienIdx = ((GameState*)gs)->FindState("TEACHER_ALIEN");
        ((GSVal*)gs)->FUN_00409f20(alienIdx);
        stateVals = (int*)((int)gs + 0x90);
        stateVals = (int*)*stateVals;
        if (stateVals[alienIdx] == 1) {
            // Check 2_NOTALIEN
            gs = DAT_0046aa30;
            int na2Idx = ((GameState*)gs)->FindState("2_NOTALIEN");
            ((GSVal*)gs)->FUN_00409f20(na2Idx);
            stateVals = (int*)((int)gs + 0x90);
            stateVals = (int*)*stateVals;
            if (stateVals[na2Idx] != 0) {
                // Check 3_NOTALIEN
                gs = DAT_0046aa30;
                int na3Idx = ((GameState*)gs)->FindState("3_NOTALIEN");
                ((GSVal*)gs)->FUN_00409f20(na3Idx);
                stateVals = (int*)((int)gs + 0x90);
                stateVals = (int*)*stateVals;
                if (stateVals[na3Idx] != 0) {
                    // Set AWARE_TEACHER = 1
                    gs = DAT_0046aa30;
                    awareIdx = ((GameState*)gs)->FindState("AWARE_TEACHER");
                    ((GSVal*)gs)->FUN_00409f20(awareIdx);
                    stateVals = (int*)((int)gs + 0x90);
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 1;
                    FUN_00444d90(4, 0x1e87, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 2) {
            // Check 1_NOTALIEN
            gs = DAT_0046aa30;
            int na1Idx = ((GameState*)gs)->FindState("1_NOTALIEN");
            ((GSVal*)gs)->FUN_00409f20(na1Idx);
            stateVals = (int*)((int)gs + 0x90);
            stateVals = (int*)*stateVals;
            if (stateVals[na1Idx] != 0) {
                gs = DAT_0046aa30;
                int na3Idx = ((GameState*)gs)->FindState("3_NOTALIEN");
                ((GSVal*)gs)->FUN_00409f20(na3Idx);
                stateVals = (int*)((int)gs + 0x90);
                stateVals = (int*)*stateVals;
                if (stateVals[na3Idx] != 0) {
                    gs = DAT_0046aa30;
                    awareIdx = ((GameState*)gs)->FindState("AWARE_TEACHER");
                    ((GSVal*)gs)->FUN_00409f20(awareIdx);
                    stateVals = (int*)((int)gs + 0x90);
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 2;
                    FUN_00444d90(4, 0x1e89, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 3) {
            // Check 1_NOTALIEN with bounds check
            gs = DAT_0046aa30;
            int na1Idx = ((GameState*)gs)->FindState("1_NOTALIEN");
            if (na1Idx < 0 || na1Idx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", na1Idx);
            }
            stateVals = (int*)((int)gs + 0x90);
            stateVals = (int*)*stateVals;
            if (stateVals[na1Idx] != 0) {
                // Check 2_NOTALIEN with bounds check
                gs = DAT_0046aa30;
                int na2Idx = ((GameState*)gs)->FindState("2_NOTALIEN");
                if (na2Idx < 0 || na2Idx > ((int*)((int)gs + 0x98))[0] - 1) {
                    FUN_00425c50("Invalid gamestate %d", na2Idx);
                }
                stateVals = (int*)((int)gs + 0x90);
                stateVals = (int*)*stateVals;
                if (stateVals[na2Idx] != 0) {
                    gs = DAT_0046aa30;
                    awareIdx = ((GameState*)gs)->FindState("AWARE_TEACHER");
                    ((GSVal*)gs)->FUN_00409f20(awareIdx);
                    stateVals = (int*)((int)gs + 0x90);
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 3;
                    FUN_00444d90(4, 0x1e88, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        }
    }

    // Send background sound message
    FUN_00444d90(5, field_F0, 0, 0, 0x1b, 0, 0, 0, 0, 0);

    // Set PREVIOUSROOM state
    gs = DAT_0046aa30;
    int prevIdx = ((GameState*)gs)->FindState("PREVIOUSROOM");
    ((GSVal*)gs)->FUN_00409f20(prevIdx);
    stateVals = (int*)((int)gs + 0x90);
    stateVals = (int*)*stateVals;
    stateVals[prevIdx] = 0x62;

    // Check P_IN_DETENTION
    gs = DAT_0046aa30;
    int pDetIdx = ((GameState*)gs)->FindState("P_IN_DETENTION");
    if (pDetIdx < 0 || pDetIdx > ((int*)((int)gs + 0x98))[0] - 1) {
        FUN_00425c50("Invalid gamestate %d", pDetIdx);
    }
    stateVals = (int*)((int)gs + 0x90);
    stateVals = (int*)*stateVals;
    if (stateVals[pDetIdx] != 0) {
        // Check S_IN_DETENTION
        gs = DAT_0046aa30;
        int sDetIdx = ((GameState*)gs)->FindState("S_IN_DETENTION");
        if (sDetIdx < 0 || sDetIdx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", sDetIdx);
        }
        stateVals = (int*)((int)gs + 0x90);
        stateVals = (int*)*stateVals;
        if (stateVals[sDetIdx] != 0) {
            // Check D_IN_DETENTION
            gs = DAT_0046aa30;
            int dDetIdx = ((GameState*)gs)->FindState("D_IN_DETENTION");
            if (dDetIdx < 0 || dDetIdx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", dDetIdx);
            }
            stateVals = (int*)((int)gs + 0x90);
            stateVals = (int*)*stateVals;
            if (stateVals[dDetIdx] != 0) {
                // All 3 in detention - send detention message
                FUN_00444d90(3, 0x1202, 0x2d, 1, 4, 0, 0, 0, 0, 0);
                // Create and push empty SpriteAction
                char local_48[52];
                void* action = FUN_00444a40(local_48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                ((QObj*)&DAT_00472d58)->FUN_00444920(action);

                // Clear MUST_SAVEGAME
                gs = DAT_0046aa30;
                int saveIdx = ((GameState*)gs)->FindState("MUST_SAVEGAME");
                ((GSVal*)gs)->FUN_00409f20(saveIdx);
                stateVals = (int*)((int)gs + 0x90);
                stateVals = (int*)*stateVals;
                stateVals[saveIdx] = 0;
            }
        }
    }
}

/* Function start: 0x41E950 */
int SCI_SchoolMenu::ShutDown(SC_Message* msg) {
    void* ptr;
    int i;

    if (DAT_0046a190 == 0) {
        return 0;
    }
    DAT_0046a190 = 0;

    // Clean up global lists
    void* engPtr = DAT_0046aa24;
    int* list1 = (int*)((int*)engPtr)[0x28]; // offset 0xA0
    if (*list1 != 0) {
        list1[2] = *list1;
        while (*list1 != 0) {
            void* item = (void*)FUN_00403520((void*)list1);
            if (item != 0) {
                *(int*)item = 0x461030;
                FUN_00454400(item);
            }
        }
    }

    int* list2 = (int*)((int*)engPtr)[0x29]; // offset 0xA4
    if (*list2 != 0) {
        list2[2] = *list2;
        while (*list2 != 0) {
            void* item = (void*)FUN_004035a0((void*)list2);
            if (item != 0) {
                FUN_00401c80(item);
                FUN_00454400(item);
            }
        }
    }

    int* list3 = (int*)((int*)engPtr)[0x27]; // offset 0x9C
    if (*list3 != 0) {
        list3[2] = *list3;
        while (*list3 != 0) {
            void* item = (void*)FUN_00403620((void*)list3);
            if (item != 0) {
                *(int*)item = 0x46102c;
                FUN_00401130((void*)((int)item + 4));
                FUN_00454400(item);
            }
        }
    }

    ((int*)engPtr)[0x2a] = 0; // offset 0xA8

    // Clean up own fields
    ptr = (void*)field_A8;
    if (ptr != 0) {
        FUN_0041dc10(ptr);
        FUN_00454400(ptr);
        field_A8 = 0;
    }

    ptr = (void*)field_AC;
    if (ptr != 0) {
        FUN_00443990(ptr);
        FUN_00454400(ptr);
        field_AC = 0;
    }

    ptr = (void*)field_B0;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        FUN_00454400(ptr);
        field_B0 = 0;
    }

    ptr = (void*)field_B4;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        FUN_00454400(ptr);
        field_B4 = 0;
    }

    ptr = (void*)field_E8;
    if (ptr != 0) {
        FUN_00424ee0(ptr);
        FUN_00454400(ptr);
        field_E8 = 0;
    }

    for (i = 3; i != 0; i--) {
        ptr = (void*)characters[3 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            FUN_00454400(ptr);
            characters[3 - i] = 0;
        }
    }

    for (i = 9; i != 0; i--) {
        ptr = (void*)options[9 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            FUN_00454400(ptr);
            options[9 - i] = 0;
        }
    }

    IconBar::CleanupIconBar(msg);
    return 0;
}

/* Function start: 0x41F610 */
int HitRect::HitTest(int x, int y) {
    int* r = (int*)this;
    if (r[0] > x || r[2] < x || r[1] > y || r[3] < y) {
        return 0;
    }
    return 1;
}

/* Function start: 0x41F640 */
int GSVal::GetStateValue(int index) {
    if (index < 0 || index > ((int*)((int)this + 0x98))[0] - 1) {
        FUN_00425c50("Invalid gamestate %d", index);
    }
    int* vals = (int*)((int*)((int)this + 0x90))[0];
    return vals[index];
}

/* Function start: 0x41F6A0 */
void SCI_SchoolMenu::PlayMenuSound() {
    FUN_00444e20(&DAT_00472d20);
}

/* Function start: 0x41F6B0 */
int SCI_SchoolMenu::IsCharacterActive() {
    int i = 0;
    int* p = characters;
    do {
        int chr = *p;
        if (chr != 0) {
            int state = ((int*)chr)[0x25];
            if (state == 1 || state == 2) {
                return 1;
            }
        }
        p++;
        i++;
    } while (i < 3);
    return 0;
}

/* Function start: 0x41F6E0 */
void SCI_SchoolMenu::SetupOptions() {
    void* gs;
    int* stateVals;
    int periodIdx;
    int periodVal;
    int charIdx;
    int charVal;
    int i;
    int stIdx;
    char buf[8];

    gs = DAT_0046aa30;
    periodIdx = ((GameState*)gs)->FindState("PERIOD");
    if (periodIdx < 0 || periodIdx > ((int*)((int)gs + 0x98))[0] - 1) {
        FUN_00425c50("Invalid gamestate %d", periodIdx);
    }
    stateVals = (int*)((int*)((int)gs + 0x90))[0];
    periodVal = stateVals[periodIdx];

    if (periodVal == 5 || periodVal == 10 || periodVal == 15 || periodVal == 20) {
        // Free period - hide all options
        int* p = options;
        int count = 9;
        do {
            int opt = *p;
            if (opt != 0) {
                ((int*)opt)[0x25] = 0;
                void* animCtrl = (void*)((int*)opt)[0x67];
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
            }
            p++;
            count--;
        } while (count != 0);
        return;
    }

    i = 0;
    int* optPtr = options;
    do {
        charIdx = DAT_0046cb90;
        gs = DAT_0046aa30;
        if (charIdx < 0 || charIdx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", charIdx);
        }
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        charVal = stateVals[charIdx];

        char ch;
        if (charVal == -1) {
            // Use default character
            charIdx = DAT_0046cb90;
            gs = DAT_0046aa30;
            if (charIdx < 0 || charIdx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", charIdx);
            }
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            ch = DAT_0046cb94[stateVals[charIdx]];
        } else {
            ch = DAT_0046cb94[charVal];
        }

        i++;
        FUN_00454510(buf, "%cRM%d", (int)ch, i);

        if (*optPtr == 0) {
            goto next_option;
        }

        gs = DAT_0046aa30;
        stIdx = ((GameState*)gs)->FindState(buf);
        if (stIdx < 0 || stIdx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", stIdx);
        }

        int stVal;
        int opt;
        opt = *optPtr;
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        stVal = stateVals[stIdx];

        gs = DAT_0046aa30;
        stIdx = ((GameState*)gs)->FindState(buf);
        if (stIdx < 0 || stIdx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", stIdx);
        }
        stateVals = (int*)((int*)((int)gs + 0x90))[0];

        if (stateVals[stIdx] == 0) {
            ((int*)opt)[0x25] = -1;
            void* animCtrl = (void*)((int*)opt)[0x67];
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(-1, 0);
            }
        } else {
            ((int*)opt)[0x25] = 0;
            void* animCtrl = (void*)((int*)opt)[0x67];
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(0, 0);
            }
        }

next_option:
        optPtr++;
    } while (i < 9);
}

/* Function start: 0x41F900 */
void InitAllSchedule() {
    int charIdx;
    int i;
    char buf[16];

    for (charIdx = 0; charIdx < 3; charIdx++) {
        for (i = 1; i <= 0x12; i++) {
            char ch;
            if (charIdx == -1) {
                void* gs = DAT_0046aa30;
                int periodIdx = DAT_0046cb90;
                if (periodIdx < 0 || periodIdx > ((int*)((int)gs + 0x98))[0] - 1) {
                    FUN_00425c50("Invalid gamestate %d", periodIdx);
                }
                int* stateVals = (int*)((int*)((int)gs + 0x90))[0];
                ch = DAT_0046cb94[stateVals[periodIdx]];
            } else {
                ch = DAT_0046cb94[charIdx];
            }

            FUN_00454510(buf, "%cRM%d", (int)ch, i);

            void* gs = DAT_0046aa30;
            int stIdx = ((GameState*)gs)->FindState(buf);
            if (stIdx < 0 || stIdx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", stIdx);
            }
            int* stateVals = (int*)((int*)((int)gs + 0x90))[0];
            stateVals[stIdx] = 1;
        }
    }
}

/* Function start: 0x41ECE0 */
int SCI_SchoolMenu::AddMessage(SC_Message* msg) {
    int ret;
    void* gs;
    int* stateVals;
    int idx;
    int i;

    ret = IconBar::CheckButtonClick(msg);
    if (ret != 0) return 1;
    if (((int*)msg)[9] < 2) return 1;

    // Check NUM_ACTIONS state
    gs = DAT_0046aa30;
    idx = ((GameState*)gs)->FindState("NUM_ACTIONS");
    ((GSVal*)gs)->FUN_00409f20(idx);
    stateVals = (int*)((int*)((int)gs + 0x90))[0];
    if (stateVals[idx] < 1) {
        // Check character sprite hit
        int* charPtr = characters;
        int hitChar = 0;

        for (i = 0; i < 3; i++) {
            int chr = *charPtr;
            if (((int*)chr)[0x24] != 0) {
                int x = ((int*)msg)[7];
                int y = ((int*)msg)[8];
                if (x >= ((int*)chr)[0x27] && x <= ((int*)chr)[0x29] &&
                    y >= ((int*)chr)[0x28] && y <= ((int*)chr)[0x2a]) {
                    hitChar = 1;
                } else {
                    hitChar = 0;
                }
            } else {
                hitChar = 0;
            }

            if (hitChar != 0) {
                break;
            }
            charPtr++;
        }

        if (hitChar != 0) {
            // Character was clicked
            char ch;
            if (i == -1) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                ((GSVal*)gs)->FUN_00409f20(charIdx);
                stateVals = (int*)((int*)((int)gs + 0x90))[0];
                ch = DAT_0046cb94[stateVals[charIdx]];
            } else {
                ch = DAT_0046cb94[i];
            }

            char buf[28];
            FUN_00454510(buf, "%c_IN_DETENTION", (int)ch);
            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf);
            ((GSVal*)gs)->FUN_00409f20(idx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            if (stateVals[idx] != 0) goto check_options;

            if (i == -1) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                ((GSVal*)gs)->FUN_00409f20(charIdx);
                stateVals = (int*)((int*)((int)gs + 0x90))[0];
                ch = DAT_0046cb94[stateVals[charIdx]];
            } else {
                ch = DAT_0046cb94[i];
            }
            FUN_00454510(buf, "%c_DETENTION_SUSPENSION", (int)ch);
            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf);
            ((GSVal*)gs)->FUN_00409f20(idx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            if (stateVals[idx] > 2) goto check_options;

            if (i == -1) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                ((GSVal*)gs)->FUN_00409f20(charIdx);
                stateVals = (int*)((int*)((int)gs + 0x90))[0];
                ch = DAT_0046cb94[stateVals[charIdx]];
            } else {
                ch = DAT_0046cb94[i];
            }
            FUN_00454510(buf, "%c_CAPTURED", (int)ch);
            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf);
            ((GSVal*)gs)->FUN_00409f20(idx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            if (stateVals[idx] != 0) goto check_options;

            // Select this character
            int* basePtr = characters;
            int count = 3;
            do {
                int chr = *basePtr;
                ((int*)chr)[0x25] = 0;
                void* animCtrl = (void*)((int*)chr)[0x67];
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
                basePtr++;
                count--;
            } while (count != 0);

            int* selCharPtr = &characters[i];
            int selChr = *selCharPtr;
            ((int*)selChr)[0x25] = 1;
            void* animCtrl = (void*)((int*)selChr)[0x67];
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(1, 0);
            }

            // Update global character selection
            gs = DAT_0046aa30;
            int charStateIdx = DAT_0046cb90;
            ((GSVal*)gs)->FUN_00409f20(charStateIdx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            stateVals[charStateIdx] = i;

            charStateIdx = DAT_0046cb90;
            gs = DAT_0046aa30;
            ((GSVal*)gs)->FUN_00409f20(charStateIdx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            ((Sprite*)DAT_0046af08)->ResetAnimation(stateVals[charStateIdx] + 1, 0);

            SetupOptions();
            selectedOption = -1;

            // Play sound
            FUN_004250e0((void*)field_E8);
            // TimedEvent for sound
            char timedEvtBuf[24];
            FUN_00421880(timedEvtBuf);
            ((Timer*)timedEvtBuf)->Wait(0x96);

            // Load character-specific sound
            int selSpr = *selCharPtr;
            gs = DAT_0046aa30;
            int periodIdx2 = DAT_0046cb90;
            int pIdx = ((GameState*)gs)->FindState("PERIOD");
            ((GSVal*)gs)->FUN_00409f20(pIdx);
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            int period = stateVals[pIdx];
            int offset = ((GSVal*)DAT_0046aa30)->GetStateValue(periodIdx2);
            int fullIdx = period * 3 + offset;
            char* sndFile = FUN_0044e530(((int*)selSpr)[0x28 + fullIdx]);
            ((Sample*)(void*)field_E8)->Load(sndFile);
            ((Sample*)(void*)field_E8)->Play(100, 1);

            goto check_options;
        }
    }

check_options:
    if (IsCharacterActive() == 0) goto check_go_button;

    // Check option hit
    {
        int* optPtr = options;
        int hitOpt = 0;

        for (i = 0; i < 9; i++) {
            int opt = *optPtr;
            if (opt == 0) goto next_opt;

            int optX, optY;
            optX = ((int*)msg)[7];
            optY = ((int*)msg)[8];
            if (((int*)opt)[0x24] != 0) {
                if (((HitRect*)((int)opt + 0x9c))->HitTest(optX, optY)) {
                    hitOpt = 1;
                } else {
                    hitOpt = 0;
                }
            } else {
                hitOpt = 0;
            }

            if (hitOpt != 0) {
                break;
            }
next_opt:
            optPtr++;
        }

        if (hitOpt == 0 && i >= 9) goto check_go_button2;

        // Option was clicked
        int* baseOpts = options;
        int count = 9;
        do {
            int opt = *baseOpts;
            if (opt != 0) {
                ((int*)opt)[0x25] = 0;
                void* animCtrl = (void*)((int*)opt)[0x67];
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
            }
            baseOpts++;
            count--;
        } while (count != 0);

        int* selOptPtr = &options[i];
        int selOpt = *selOptPtr;
        ((int*)selOpt)[0x25] = 1;
        void* animCtrl = (void*)((int*)selOpt)[0x67];
        if (animCtrl != 0) {
            ((Sprite*)animCtrl)->ResetAnimation(1, 0);
        }

        // Load option sound
        gs = DAT_0046aa30;
        int charStateIdx = DAT_0046cb90;
        int pIdx = ((GameState*)gs)->FindState("PERIOD");
        ((GSVal*)gs)->FUN_00409f20(pIdx);
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        int period = stateVals[pIdx];
        int offset = ((GSVal*)DAT_0046aa30)->GetStateValue(charStateIdx);
        int fullIdx = period * 3 + offset;
        char* sndFile = FUN_0044e530(((int*)selOpt)[0x28 + fullIdx]);
        ((Sample*)(void*)field_E8)->Load(sndFile);
        ((Sample*)(void*)field_E8)->Play(100, 1);
        selectedOption = i;
    }

check_go_button2:
    ;

check_go_button:
    if (selectedOption <= -1) goto check_back;
    if (IsCharacterActive() == 0) goto check_back;

    // Check go button hit
    {
        int goSpr = field_B0;
        if (((int*)goSpr)[0x24] != 0) {
            int x = ((int*)msg)[7];
            int y = ((int*)msg)[8];
            int hit;
            if (((HitRect*)(goSpr + 0x9c))->HitTest(x, y)) {
                hit = 1;
            } else {
                hit = 0;
            }
            if (hit != 0) {
                // Go button clicked - increment NUM_ACTIONS
                gs = DAT_0046aa30;
                idx = ((GameState*)gs)->FindState("NUM_ACTIONS");
                ((GSVal*)gs)->FUN_00409f20(idx);
                stateVals = (int*)((int*)((int)gs + 0x90))[0];
                stateVals[idx]++;

                // Execute option action
                FUN_00421020((void*)options[selectedOption]);

                // Determine next character
                gs = DAT_0046aa30;
                int pIdx = ((GameState*)gs)->FindState("PERIOD");
                char nextChar = 0;
                ((GSVal*)gs)->FUN_00409f20(pIdx);
                stateVals = (int*)((int*)((int)gs + 0x90))[0];
                int period = stateVals[pIdx];

                if (period == 0x12) {
                    // Period 18 - check current char and assign next
                    gs = DAT_0046aa30;
                    int charIdx = DAT_0046cb90;
                    ((GSVal*)gs)->FUN_00409f20(charIdx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    if (DAT_0046cb94[stateVals[charIdx]] == 'D') {
                        nextChar = 'P';
                    } else {
                        gs = DAT_0046aa30;
                        charIdx = DAT_0046cb90;
                        ((GSVal*)gs)->FUN_00409f20(charIdx);
                        stateVals = (int*)((int*)((int)gs + 0x90))[0];
                        if (DAT_0046cb94[stateVals[charIdx]] == 'S') {
                            nextChar = 'D';
                        } else {
                            gs = DAT_0046aa30;
                            charIdx = DAT_0046cb90;
                            ((GSVal*)gs)->FUN_00409f20(charIdx);
                            stateVals = (int*)((int*)((int)gs + 0x90))[0];
                            if (DAT_0046cb94[stateVals[charIdx]] == 'P') {
                                nextChar = 'S';
                            }
                        }
                    }
                } else if (period == 0x13) {
                    // Period 19 - check captured states
                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState("D_CAPTURED");
                    ((GSVal*)gs)->FUN_00409f20(idx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    if (stateVals[idx] == 0) {
                        gs = DAT_0046aa30;
                        int charIdx = DAT_0046cb90;
                        ((GSVal*)gs)->FUN_00409f20(charIdx);
                        stateVals = (int*)((int*)((int)gs + 0x90))[0];
                        if (DAT_0046cb94[stateVals[charIdx]] != 'D') {
                            nextChar = 'D';
                            goto set_captured;
                        }
                    }

                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState("S_CAPTURED");
                    ((GSVal*)gs)->FUN_00409f20(idx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    if (stateVals[idx] == 0) {
                        gs = DAT_0046aa30;
                        int charIdx = DAT_0046cb90;
                        ((GSVal*)gs)->FUN_00409f20(charIdx);
                        stateVals = (int*)((int*)((int)gs + 0x90))[0];
                        if (DAT_0046cb94[stateVals[charIdx]] != 'S') {
                            nextChar = 'S';
                            goto set_captured;
                        }
                    }

                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState("P_CAPTURED");
                    ((GSVal*)gs)->FUN_00409f20(idx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    if (stateVals[idx] == 0) {
                        gs = DAT_0046aa30;
                        int charIdx = DAT_0046cb90;
                        ((GSVal*)gs)->FUN_00409f20(charIdx);
                        stateVals = (int*)((int*)((int)gs + 0x90))[0];
                        if (DAT_0046cb94[stateVals[charIdx]] != 'P') {
                            nextChar = 'P';
                        }
                    }
                }

set_captured:
                if (nextChar != 0) {
                    char capBuf[28];
                    FUN_00454510(capBuf, "%c_CAPTURED", (int)nextChar);
                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState(capBuf);
                    ((GSVal*)gs)->FUN_00409f20(idx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    stateVals[idx] = 1;

                    FUN_00454510(capBuf, "%c_IN_DETENTION", (int)nextChar);
                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState(capBuf);
                    ((GSVal*)gs)->FUN_00409f20(idx);
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    stateVals[idx] = 1;
                }
            }
        }
    }

check_back:
    // Check back button hit
    if (field_B4 == 0) return 1;
    {
        int backSpr = field_B4;
        int x = ((int*)msg)[7];
        int y = ((int*)msg)[8];
        int hit;
        if (((int*)backSpr)[0x24] != 0) {
            if (((HitRect*)(backSpr + 0x9c))->HitTest(x, y)) {
                hit = 1;
            } else {
                hit = 0;
            }
        } else {
            hit = 0;
        }
        if (hit != 0) {
            PlayMenuSound();
        }
    }

    return 1;
}

/* Function start: 0x41F9D0 */
int SCI_SchoolMenu::LBLParse(char* line) {
    char label[32];
    char buf1[32];
    char buf2[32];
    char buf3[32];
    char buf4[32];
    char buf5[32];
    char buf6[28];
    int period;
    int val1;
    int val2;
    int val3;
    int count;
    void* gs;
    int* stateVals;
    int idx;

    FUN_00454850(line, "%s", label);

    if (strcmp(label, "BACK") == 0) {
        void* mem = FUN_00454500(0xa0);
        void* obj = 0;
        if (mem != 0) {
            FUN_004438a0(mem);
            obj = mem;
        }
        field_AC = (int)obj;
        FUN_00413e70(obj, (int)this, 0);
        return 0;
    }

    if (strcmp(label, "AVAILABILITY") == 0) {
        count = FUN_00454850(line, "%s %d %s %s %s", label, &period, buf1, buf2, buf3);
        if (count < 3 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = DAT_0046aa30;
        idx = ((GameState*)gs)->FindState("PERIOD");
        if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        if (stateVals[idx] == period) {
            count = count - 2;
            if (count > 0) {
                char* p = buf1;
                do {
                    gs = DAT_0046aa30;
                    idx = ((GameState*)gs)->FindState(p);
                    if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
                        FUN_00425c50("Invalid gamestate %d", idx);
                    }
                    stateVals = (int*)((int*)((int)gs + 0x90))[0];
                    p = p + 0x20;
                    count--;
                    stateVals[idx] = 0;
                } while (count != 0);
            }
        }
        return 0;
    }

    if (strcmp(label, "PALE") == 0) {
        char filename[64];
        FUN_00454850(line, "%s %s", label, filename);
        void* mem = FUN_00454500(8);
        void* obj = 0;
        if (mem != 0) {
            FUN_0041dbe0(mem);
            obj = mem;
        }
        field_A8 = (int)obj;
        ((Palette*)obj)->Load(filename);
        return 0;
    }

    if (strcmp(label, "BGSND") == 0) {
        FUN_00454850(line, "%s %d", label, &field_F0);
        return 0;
    }

    if (strcmp(label, "CHAR") == 0) {
        FUN_00454850(line, "%s %d", label, &period);
        void* mem = FUN_00454500(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            ((SprInit*)mem)->FUN_00420ce0(period);
            obj = mem;
        }
        characters[period] = (int)obj;
        FUN_00413e70(obj, (int)this, 0);
        return 0;
    }

    if (FUN_00454960(label, "OPTION") != 0) {
        FUN_00454850(line, "%s %d", label, &period);
        void* mem = FUN_00454500(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            ((SprInit*)mem)->FUN_00420ce0(period);
            obj = mem;
        }
        options[period] = (int)obj;
        FUN_00413e70(obj, (int)this, 0);
        return 0;
    }

    if (strcmp(label, "OKAY") == 0) {
        void* mem = FUN_00454500(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            ((SprInit*)mem)->FUN_00420ce0(0);
            obj = mem;
        }
        field_B0 = (int)obj;
        FUN_00413e70(obj, (int)this, 0);
        return 0;
    }

    if (strcmp(label, "CANCEL") == 0) {
        void* mem = FUN_00454500(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            ((SprInit*)mem)->FUN_00420ce0(0);
            obj = mem;
        }
        field_B4 = (int)obj;
        FUN_00413e70(obj, (int)this, 0);

        // Check if NUM_ACTIONS is 0 - if so, destroy back button
        gs = DAT_0046aa30;
        idx = ((GameState*)gs)->FindState("NUM_ACTIONS");
        if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        if (stateVals[idx] == 0 && field_B4 != 0) {
            FUN_00420d90((void*)field_B4);
            FUN_00454400((void*)field_B4);
            field_B4 = 0;
        }
        return 0;
    }

    if (FUN_00454960(label, "CLASS") != 0) {
        count = FUN_00454850(line, "%s %d %s %d %s %d %s %d", label, &period, buf4, &val1, buf5, &val2, buf6, &val3);
        if (count < 4 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = DAT_0046aa30;
        idx = ((GameState*)gs)->FindState("PERIOD");
        if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        stateVals = (int*)((int*)((int)gs + 0x90))[0];
        if (stateVals[idx] == period) {
            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf4);
            if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            stateVals[idx] = val1;

            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf5);
            if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            stateVals[idx] = val2;

            gs = DAT_0046aa30;
            idx = ((GameState*)gs)->FindState(buf6);
            if (idx < 0 || idx > ((int*)((int)gs + 0x98))[0] - 1) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            stateVals = (int*)((int*)((int)gs + 0x90))[0];
            stateVals[idx] = val3;
        }
        return 0;
    }

    if (strcmp(label, "END") == 0) {
        return 1;
    }

    ReportUnknownLabel("SCI_SchoolMenu");
    return 0;
}
