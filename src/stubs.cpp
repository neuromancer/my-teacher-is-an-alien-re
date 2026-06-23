// ============================================================================
// stubs.cpp - Historical tracking of function identification and migration
//
// All implemented functions have been moved to their proper .cpp files.
// This file is kept for reference only.
// ============================================================================

// SmackSummary stub: present in original SMACKW32.DLL but absent from the
// version of smackw32.lib bundled in this repo. Provide a minimal stub so the
// EXE links; only the debug-only ShowSmackSummary code path uses it.
extern "C" void __stdcall SmackSummary(void*, unsigned int* summary)
{
    for (int i = 0; i < 21; i++) {
        summary[i] = 1;
    }
}

// EnqueueHotspotAction — in SC_CombatBase.cpp
// FUN_00447790 = SC_SpaceShipNav::ShutDown — in SC_SpaceShipNav.cpp
// FUN_0044c660 = Sprite::Sprite(char*) — in Sprite.cpp
// FUN_00426ac0 = WaitForInput — in main.cpp
// FreeArrayItems (0x408EE0) — in CombatSprite.cpp
// AssetCleanup / AssetNodeDelete — in AssetList.cpp
// OpenSaveFile (0x426050) — in CDData.cpp
// DeleteFileAndDir (0x4260A0) — in CDData.cpp
// LogCacheStats (0x434520) — in CDData.cpp
// LogCacheEntries (0x4345B0) — in CDData.cpp
// SavePoolAllocNode (0x43E2A0) — in SC_DuctNav.cpp
// ClearSaveEntries (0x43E320) — in SC_DuctNav.cpp
// FreePoolBlocks (0x43E900) — in SCI_Inventory.cpp
// GetDirectionChar (0x449B60) — in mCNavigator.cpp
// SmackSummary — in SC_Cinematic.cpp
// TextInput::TextInput (0x445680) — in TextInput.cpp
// TextInput::~TextInput (0x4456F0) — in TextInput.cpp
// AnimatedAsset::GetTextWidth (0x43AD50) — in AnimatedAsset.cpp
// AnimatedAsset::IsValidChar (0x43ABA0) — in AnimatedAsset.cpp
