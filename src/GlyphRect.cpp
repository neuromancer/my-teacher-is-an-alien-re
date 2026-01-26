#include "GlyphRect.h"
#include "globals.h"
#include "Array.h"
#include <stdlib.h>

static void GlyphRect_Destructor(void* ptr)
{
    ((GlyphRect*)ptr)->~GlyphRect();
}

GlyphRect::~GlyphRect()
{
}

/* Function start: 0x40DFE0 */
void InitPuzzleRects1()
{
    g_PuzzleRects1_0043d068[0].top = 0x147;
    g_PuzzleRects1_0043d068[0].right = 0x205;
    g_PuzzleRects1_0043d068[0].bottom = 0x16d;
    g_PuzzleRects1_0043d068[1].left = 0x209;
    g_PuzzleRects1_0043d068[1].top = 0x147;
    g_PuzzleRects1_0043d068[1].bottom = 0x16d;
    g_PuzzleRects1_0043d068[2].top = 0x147;
    g_PuzzleRects1_0043d068[0].left = 0x1de;
    g_PuzzleRects1_0043d068[1].right = 0x230;
    g_PuzzleRects1_0043d068[2].bottom = 0x16d;
    g_PuzzleRects1_0043d068[3].top = 0x11c;
    g_PuzzleRects1_0043d068[3].right = 0x205;
    g_PuzzleRects1_0043d068[4].left = 0x209;
    g_PuzzleRects1_0043d068[3].bottom = 0x143;
    g_PuzzleRects1_0043d068[4].top = 0x11c;
    g_PuzzleRects1_0043d068[2].left = 0x234;
    g_PuzzleRects1_0043d068[2].right = 0x25b;
    g_PuzzleRects1_0043d068[4].bottom = 0x143;
    g_PuzzleRects1_0043d068[5].top = 0x11c;
    g_PuzzleRects1_0043d068[3].left = 0x1de;
    g_PuzzleRects1_0043d068[4].right = 0x230;
    g_PuzzleRects1_0043d068[5].bottom = 0x143;
    g_PuzzleRects1_0043d068[6].top = 0xf1;
    g_PuzzleRects1_0043d068[6].right = 0x205;
    g_PuzzleRects1_0043d068[7].left = 0x209;
    g_PuzzleRects1_0043d068[6].bottom = 0x118;
    g_PuzzleRects1_0043d068[7].top = 0xf1;
    g_PuzzleRects1_0043d068[5].left = 0x234;
    g_PuzzleRects1_0043d068[5].right = 0x25b;
    g_PuzzleRects1_0043d068[7].bottom = 0x118;
    g_PuzzleRects1_0043d068[8].top = 0xf1;
    g_PuzzleRects1_0043d068[6].left = 0x1de;
    g_PuzzleRects1_0043d068[8].bottom = 0x118;
    g_PuzzleRects1_0043d068[7].right = 0x230;
    g_PuzzleRects1_0043d068[8].left = 0x234;
    g_PuzzleRects1_0043d068[8].right = 0x25b;
}

void CleanupPuzzleRects1(void);

/* Function start: 0x40E110 */
void RegisterPuzzleRects1Cleanup()
{
    atexit(CleanupPuzzleRects1);
}

/* Function start: 0x40E120 */
void CleanupPuzzleRects1()
{
    if ((DAT_0043d0f8 & 1) == 0) {
        DAT_0043d0f8 = DAT_0043d0f8 | 1;
        Array_Cleanup(g_PuzzleRects1_0043d068, 0x10, 9, GlyphRect_Destructor);
    }
}

/* Function start: 0x40E160 */
void InitPuzzleRects2()
{
    g_PuzzleRects2_0043d100[0].top = 0xa1;
    g_PuzzleRects2_0043d100[0].bottom = 0x137;
    g_PuzzleRects2_0043d100[0].left = 0x14;
    g_PuzzleRects2_0043d100[0].right = 0x46;
    g_PuzzleRects2_0043d100[1].left = 0xb2;
    g_PuzzleRects2_0043d100[1].top = 0xac;
    g_PuzzleRects2_0043d100[1].right = 0xef;
    g_PuzzleRects2_0043d100[2].top = 0xa1;
    g_PuzzleRects2_0043d100[2].bottom = 0x137;
    g_PuzzleRects2_0043d100[1].bottom = 0x118;
    g_PuzzleRects2_0043d100[2].left = 0x15d;
    g_PuzzleRects2_0043d100[2].right = 0x18f;
}

void CleanupPuzzleRects2(void);

/* Function start: 0x40E1E0 */
void RegisterPuzzleRects2Cleanup()
{
    atexit(CleanupPuzzleRects2);
}

/* Function start: 0x40E1F0 */
void CleanupPuzzleRects2()
{
    if ((DAT_0043d0f8 & 2) == 0) {
        DAT_0043d0f8 = DAT_0043d0f8 | 2;
        Array_Cleanup(g_PuzzleRects2_0043d100, 0x10, 3, GlyphRect_Destructor);
    }
}
