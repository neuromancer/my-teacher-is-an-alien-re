#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Externs for globals
extern HDC DAT_00437488;
extern HDC DAT_004374b4;
extern char DAT_00437506;
extern char DAT_00437507;
extern int DAT_00437508;
extern int DAT_0043750c;
extern int DAT_00437510;
extern int DAT_00437f4c;
extern int DAT_00437f50;

extern HGDIOBJ DAT_00437496;
extern int DAT_0043749a;
extern int DAT_0043749e;
extern int DAT_004374a2;
extern char DAT_00439446[256]; // TEXTMETRIC struct buffer

extern char DAT_00437490;
extern short DAT_004374b2;
extern int DAT_00437491;
extern int DAT_00437518;
extern int DAT_0043751c;
extern int DAT_004383ec;
extern int DAT_004383f4;
extern int DAT_00438404;
extern int DAT_0043840c;
extern int DAT_004374c2;
extern int DAT_004374ce;
extern int DAT_004374d6;
extern int DAT_004374da;
extern int DAT_00437514;
extern char DAT_004374c0;
extern char DAT_004374c1;
extern int DAT_00437f54;
extern int DAT_00437f56;
extern int DAT_00437f5a;
extern int DAT_0043841c;
extern HGDIOBJ DAT_00438424;

extern char DAT_00437520[256];  // Palette identity map
extern int DAT_00437620[64];    // Palette data
extern int DAT_0043826c[32];    // Palette data
extern char DAT_00437720[];     // LOGPALETTE buffer
extern char DAT_00437afc[];     // inside DAT_00437720
extern char DAT_00437b4c[];     // inside DAT_00437b48 typically
extern char DAT_00437b48[];     // BGR palette buffer


// Far pointer storage for WinG
extern char* PTR_DAT_0043843c;
extern char DAT_00438446[256];
extern int DAT_00438442;
extern short DAT_00438440;
extern HANDLE DAT_004374ee;
extern HMODULE DAT_00438420;

// WinG function pointers
extern void* DAT_00438428;
extern void* DAT_0043842c;
extern void* DAT_00438430;
extern void* DAT_00438434;
extern void* DAT_00438438;

// External functions
extern "C" void* GetGameWindowHandle();
extern "C" int SetCursorVisible(unsigned int);

// Forward declarations
int InitStockFont(int);
extern "C" int GetColorBitDepth(void);

// Exported functions
extern "C" int InitMouseSettings(void);
extern "C" int InitVideoSystem(void);


/* Function start: 0x423A54 */
int InitMouseSettings(void)
{
    HWND hWnd;
    int iVar2;
    
    hWnd = GetActiveWindow();
    DAT_00437510 = GetWindowWord(hWnd, -6);
    iVar2 = GetSystemMetrics(0x13);  // SM_SWAPBUTTON
    if (iVar2 != 0) {
        DAT_00437508 = GetSystemMetrics(0xd); // SM_CXDOUBLECLK
        DAT_0043750c = GetSystemMetrics(0xe); // SM_CYDOUBLECLK
        DAT_00437506 = 1;
        DAT_00437507 = 1;
        return 1;
    }
    DAT_00437506 = 0;
    return -1;
}

/* Function start: 0x423AAC */
int InitVideoSystem(void)
{
    DWORD DVar2;
    UINT UVar3;
    HMODULE pHVar4;
    int iVar5;
    char *pcVar6;
    char *pcVar7;
    char *pcVar8;
    int *puVar7;
    char cVar1;
    int uVar9;
    int bVar10;
    int lVar11;
    
    PTR_DAT_0043843c = DAT_00438446;
    DAT_00438442 = 0x1000;
    // DAT_00438440 = DS; // Segment register - skip
    
    InitStockFont(10);  // OEM_FIXED_FONT
    
    cVar1 = '\0';
    puVar7 = DAT_0043826c;
    for (iVar5 = 0x20; iVar5 != 0; iVar5 = iVar5 - 1) {
        *puVar7 = 0;
        puVar7 = puVar7 + 1;
    }
    puVar7 = DAT_00437620;
    for (iVar5 = 0x40; iVar5 != 0; iVar5 = iVar5 - 1) {
        *puVar7 = 0;
        puVar7 = puVar7 + 1;
    }
    iVar5 = 0x100;
    pcVar8 = DAT_00437520;
    do {
        *pcVar8 = cVar1;
        cVar1 = cVar1 + 1;
        iVar5 = iVar5 - 1;
        pcVar8 = pcVar8 + 1;
    } while (iVar5 != 0);
    
    uVar9 = 0x428;
    lVar11 = GetColorBitDepth();
    bVar10 = (unsigned int)lVar11 < 9;
    if (bVar10) {
        uVar9 = 0x228;
    }
    DAT_00437f50 = bVar10;
    DAT_00437f4c = uVar9;
    
    DAT_00437490 = 0;
    DAT_004374b2 = 0;
    DAT_00437491 = 0;
    DAT_00437518 = 0;
    DAT_0043751c = 0;
    DAT_004383ec = 0;
    DAT_004383f4 = 0;
    DAT_00438404 = 0;
    DAT_0043840c = 0;
    DAT_004374c2 = 0;
    DAT_004374ce = 0;
    DAT_004374d6 = 1;
    DAT_004374da = 1;
    DAT_00437514 = 1;
    DAT_004374c0 = (char)0xff;
    DAT_004374c1 = (char)0xff;
    DAT_00437f54 = (char)0xff;
    DAT_00437f56 = -1;
    DAT_00437f5a = -1;
    DAT_0043841c = 0;
    DAT_00438424 = 0;
    
    DVar2 = GetVersion();
    if (((unsigned short)DVar2 & 0xaff) == 0xa03) {
        // Windows 3.x with Win32s - try to load WING32.DLL
        DAT_004374ee = CreateFileA("WING32.DLL", 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0x0, 3, 0, (HANDLE)0x0);
        if (DAT_004374ee == (HANDLE)-1) {
            UVar3 = GetSystemDirectoryA(DAT_00438446, 0x100);
            DAT_00438446[UVar3] = '\\';
            pcVar6 = "WING32.DLL";
            pcVar8 = &DAT_00438446[UVar3 + 1];
            for (iVar5 = 0xb; iVar5 != 0; iVar5 = iVar5 - 1) {
                *pcVar8 = *pcVar6;
                pcVar6 = pcVar6 + 1;
                pcVar8 = pcVar8 + 1;
            }
            DAT_004374ee = CreateFileA(DAT_00438446, 0x80000000, 1, (LPSECURITY_ATTRIBUTES)0x0, 3, 0, (HANDLE)0x0);
            if (DAT_004374ee == (HANDLE)-1) {
                return 0;
            }
        }
        CloseHandle(DAT_004374ee);
        pHVar4 = LoadLibraryA("WING32.DLL");
        if (pHVar4 != (HMODULE)0x0) {
            DAT_00438420 = pHVar4;
            // Get WinG function pointers using ordinals
            DAT_00438428 = (void*)GetProcAddress(pHVar4, (LPCSTR)2);
            if (DAT_00438428 == 0) return 0;
            DAT_0043842c = (void*)GetProcAddress(pHVar4, (LPCSTR)6);
            if (DAT_0043842c == 0) return 0;
            DAT_00438430 = (void*)GetProcAddress(pHVar4, (LPCSTR)3);
            if (DAT_00438430 == 0) return 0;
            DAT_00438434 = (void*)GetProcAddress(pHVar4, (LPCSTR)1);
            if (DAT_00438434 == 0) return 0;
            DAT_00438438 = (void*)GetProcAddress(pHVar4, (LPCSTR)10);
            if (DAT_00438438 == 0) return 0;
            // Call WinGRecommendDIBFormat (ordinal 3)
            typedef int (__stdcall *WinGRecommendDIBFormat_t)();
            DAT_0043841c = ((WinGRecommendDIBFormat_t)DAT_00438430)();
        }
    }
    return 0;
}

/* Function start: 0x423CD9 */
int GetColorBitDepth(void)
{
    unsigned int uVar1;
    unsigned int uVar2;
    
    uVar1 = GetDeviceCaps(DAT_00437488, 0xc);  // BITSPIXEL
    uVar2 = GetDeviceCaps(DAT_00437488, 0xe);  // PLANES
    return uVar2 * uVar1;
}

/* Function start: 0x4244E9 */
int InitStockFont(int param_1)
{
    TEXTMETRICA *pTextMetric;
    
    DAT_00437496 = GetStockObject(param_1);
    SelectObject(DAT_004374b4, DAT_00437496);
    pTextMetric = (TEXTMETRICA *)DAT_00439446;
    GetTextMetricsA(DAT_004374b4, pTextMetric);
    DAT_0043749a = pTextMetric->tmHeight + pTextMetric->tmExternalLeading;
    DAT_0043749e = pTextMetric->tmExternalLeading;
    DAT_004374a2 = pTextMetric->tmAveCharWidth;
    return 0;
}

// Additional globals for palette functions
extern HPALETTE hPal_0043748c;
extern HPALETTE DAT_004374ae;

/* Function start: 0x424162 */
int SetDeviceContext(HDC param_1)
{
    DAT_00437488 = param_1;
    DAT_004374b4 = param_1;
    return 0;
}

/* Function start: 0x423D5B */
int SelectAndRealizePalette(HPALETTE param_1)
{
    HPALETTE pHVar1;
    
    hPal_0043748c = param_1;
    pHVar1 = SelectPalette(DAT_00437488, param_1, 0);
    RealizePalette(DAT_00437488);
    if (DAT_004374ae == (HPALETTE)0x0) {
        DAT_004374ae = pHVar1;
    }
    return 0;
}

// Dummy palette data (should be RGB triplets)
static const unsigned char DAT_00423e92[256 * 3] = {0};
static const unsigned char DAT_00423e98[256 * 3] = {0};

/* Function start: 0x423D96 */
HPALETTE CreateSystemPalette(void)
{
  HDC hdc;
  unsigned char* pEntries;
  unsigned char* src;
  unsigned char* dest;
  unsigned char* pBgrDest;
  int count;
  unsigned char r, g;
  
  hdc = GetDC((HWND)0x0);
  pEntries = (unsigned char*)DAT_00437720 + 4;
  if (GetColorBitDepth() < 8) {
    GetSystemPaletteEntries(hdc, 0, 8, (PALETTEENTRY*)pEntries);
    GetSystemPaletteEntries(hdc, 8, 8, (PALETTEENTRY*)(pEntries + 0x3e0));
    ReleaseDC((HWND)0x0, hdc);
    dest = (unsigned char*)DAT_00437720 + 0x24;
    src = (unsigned char*)DAT_00423e92;
    count = 0xf0;
  }
  else {
    GetSystemPaletteEntries(hdc, 0, 10, (PALETTEENTRY*)pEntries);
    GetSystemPaletteEntries(hdc, 246, 10, (PALETTEENTRY*)(pEntries + 0x3d8));
    ReleaseDC((HWND)0x0, hdc);
    dest = (unsigned char*)DAT_00437720 + 0x2c;
    src = (unsigned char*)DAT_00423e98;
    count = 0xec;
  }
  
  // First copy loop: RGB triplet -> RGBX quad with flags=1
  do {
    dest[0] = src[0];  // Red
    dest[1] = src[1];  // Green
    dest[2] = src[2];  // Blue
    dest[3] = 1;       // Flags = PC_NOCOLLAPSE
    src = src + 3;
    dest = dest + 4;
    count = count - 1;
  } while (count != 0);
  
  // Second loop: Create BGR reordered copy at DAT_00437b4c
  pEntries = (unsigned char*)DAT_00437720 + 4;
  pBgrDest = (unsigned char*)DAT_00437b48 + 4;
  count = 0x100;
  do {
    r = pEntries[0];
    g = pEntries[1];
    pBgrDest[0] = pEntries[2];  // Blue
    pBgrDest[1] = g;            // Green
    pBgrDest[2] = r;            // Red
    pBgrDest[3] = 0;            // Reserved
    pEntries = pEntries + 4;
    pBgrDest = pBgrDest + 4;
    count = count - 1;
  } while (count != 0);
  
  return CreatePalette((LOGPALETTE *)DAT_00437720);
}

#include "VBuffer.h"
extern VBuffer* g_WorkBuffer_00436974;
extern "C" int* GetWindowWidth();
extern "C" int* GetWindowHeight();

/* Function start: 0x4193E0 */
extern "C" void FlipScreen() {
    VBuffer* pThis;
    int* piVar1;
    int iVar2;
    int iVar3;
    
    if (g_WorkBuffer_00436974 == 0) {
        return;
    }
    pThis = g_WorkBuffer_00436974;
    piVar1 = GetWindowHeight();
    iVar2 = *piVar1 - 1;
    iVar3 = 0;
    piVar1 = GetWindowWidth();
    pThis->CallBlitter5(pThis->clip_x1, pThis->clip_x2, pThis->saved_video_mode, pThis->video_mode_lock_count, 0, *piVar1 - 1, iVar3, iVar2);
}

