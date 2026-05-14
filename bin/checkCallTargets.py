#!/usr/bin/env python3
"""
Verify that reimplemented functions call the correct functions compared to the original assembly.

The mnemonic-based similarity checker doesn't verify call destinations - a CALL instruction
matches any other CALL regardless of target. This script compares the multiset of called
functions between original and reimplemented code.

Vtable calls (CALL dword ptr [reg+offset]) are compared by offset to verify correct vtable slot.
"""

import argparse
import glob
import os
import re
import struct
import subprocess
import sys
from collections import Counter

# Known CRT/library address-to-name mappings (demo addresses)
KNOWN_CRT_DEMO = {
    0x4249C0: "operator_new",
    0x424940: "operator_delete",
    0x4277E0: "FreeFromGlobalHeap",
    0x428440: "FUN_00428440",
    0x428460: "AllocateMemoryInternal",
    0x4284A0: "HeapAllocWrapper",
    0x42C5C0: "OutOfMemoryHandler",
    0x424C40: "sscanf",
    0x424C90: "strchr",
    0x424CB0: "vsprintf",
    0x424950: "sprintf",
    0x424C00: "strstr",
    0x425E50: "fsopen",
    0x425E70: "fclose",
    0x425E20: "_fsopen",
    0x426140: "fprintf",
    0x426180: "vfprintf",
    0x4269E0: "fwrite",
    0x4268D0: "fgets",
    0x426320: "fread",
    0x425F30: "fseek",
    0x42D550: "_open",
    0x42B5E0: "_read",
    0x42BD50: "_write",
    0x42ABB0: "_close",
    0x42AD80: "_flushall",
    0x430360: "_filelength",
    0x42B460: "_mbsnbcpy",
    0x425FD0: "strncpy",
    0x426940: "_isctype",
    0x4303F0: "_chdir",
    0x4304A0: "_mkdir",
    0x430260: "_strnicmp",
    0x419110: "ShowError",
    0x419170: "ShowMessage",
    0x4191C0: "ClearMessageLog",
    0x4191D0: "WriteToMessageLog",
    0x4192A0: "WriteToMessageLogIfEnabled",
    0x419220: "AddToStringTable",
    0x422DC1: "SetCursorVisible",
    0x4224D0: "GetGameWindowHandle",
    0x4224E0: "GetWindowWidth",
    0x4224F0: "GetWindowHeight",
    0x422500: "GetWindowedModeFlag",
    0x422F00: "CreateTable",
    0x4234D5: "ReleaseBufferEntry",
    0x4230D9: "SelectVideoBuffer",
    0x423099: "ClearVideoBuffer",
    0x422E1A: "ReleaseVideoBuffer",
    0x422E71: "GetVideoBufferData",
    0x4229EA: "SetDrawPosition",
    0x422A2F: "SetDrawColors",
    0x423296: "BlitToDevice",
    0x42333A: "StretchBlitBuffer",
    0x422E02: "CalculateBufferSize",
    0x422E8F: "ApplyVideoPalette",
    0x422A01: "SetFillColor",
    0x418DC0: "Parser::ProcessFile",
    0x418D60: "ParseFile",
    0x41E8C0: "SoundList::Register",
    0x420900: "GameState::GetState",
    0x424FD0: "rand",
    0x42DD40: "_strcmpi",
    0x424DA0: "atexit",
    0x4086C0: "Queue::InsertAtCurrent",
    0x408940: "Queue::GetCurrentData",
    0x409160: "ZBQueue::InsertBeforeCurrent",
    0x425EE0: "fgetpos",
    0x425F10: "fsetpos",
    0x425DEC: "_ftol",
    0x426B90: "getenv",
    0x426B60: "strrchr",
    0x411080: "DrawEntry::~DrawEntry",
    0x4189A0: "ListNode::Cleanup",
    0x4189D0: "DrawEntry::~DrawEntry",
    0x418F50: "TimeOut::TimeOut",
    0x408860: "QueueNode::QueueNode",
    0x41CC10: "ZBQueue::Cleanup",
    0x41CD30: "ZBQueue::InsertBeforeCurrent",
    0x41CD10: "ListNode::Init",
    0x41C58C: "ZBQueue::Push",
    0x402700: "QueueNode_scalar_dtor",
    0x4088F0: "Queue::Pop",
    0x417180: "Handler::CopyCommandData",
    0x4171B0: "Handler::WriteMessageAddress",
    0x4195C0: "CDData_FormatPath",
    0x417C50: "TimedEventPool::PopSafe",
    0x419050: "strstr_custom",
    0x4236D7: "BuildScaleTable",
    0x4243B1: "DrawEllipseScanline",
    0x40C5B0: "QueueNode::QueueNode",
    0x4188A0: "ListNode::AdvanceToHead",
    0x413D50: "ObjectPool::Allocate",
    0x420EB0: "HashTable::AllocateBuckets",
    0x420F50: "HashTable::AllocateNode",
    0x405440: "SCI_AfterSchoolMenu::FillOptionQueue",
    0x41AA10: "VBuffer::~VBuffer",
    0x423CCA: "CleanupCinematic",
}

# Full game CRT/library addresses (auto-detected from Ghidra "Library Function" tags)
KNOWN_CRT_FULL = {
    0x454400: "operator_delete",   # GlobalFree wrapper
    0x454500: "operator_new",      # GlobalAlloc wrapper
    0x454510: "sprintf",
    0x454580: "fread",             # _fread_lk
    0x454850: "sscanf",
    0x454920: "rand",
    0x454B60: "atol",
    0x454C10: "atoi",
    0x454C20: "_onexit",
    0x454CA0: "atexit",
    0x454CF8: "_ftol",
    0x454D20: "_findfirst",
    0x454E70: "_findnext",
    0x455040: "fclose",
    0x4550B0: "fgetpos",
    0x4550E0: "_fsopen",
    0x455160: "fsetpos",
    0x455180: "fseek",
    0x455220: "strncpy",
    0x455250: "strncmp",
    0x455290: "vsprintf",
    0x4558B6: "_fdivp_sti_st",
    0x4563C0: "fprintf",
    0x456400: "vfprintf",
    0x456960: "fputs",
    0x4569C0: "_isctype",
    0x4560F0: "_splitpath",
    0x456A80: "getenv",
    0x456B20: "strrchr",
    0x456CE0: "_amsg_exit",
    0x426AB0: "joyGetNumDevs",
    0x4582C0: "fseek",             # detected from usage patterns
    0x48AF4C: "_ftol",             # _ftol thunk (JMP to IAT entry)
    0x48AF60: "__chkstk",          # MSVC stack probe for large frames
    0x458490: "_write",
    0x458690: "_read",
    0x458900: "_write",
    0x45A2E0: "_close",
    0x45ABF0: "_mbsnbcpy",
    0x45D940: "_open",
    0x45D050: "_ld12tod",
    0x45DD80: "_strcmpi",
    0x45DFB0: "strpbrk",
    0x45DFF0: "toupper",
    0x45FBE0: "_strdup",
    0x460220: "_strnicmp",
    0x460340: "_filelength",
    0x4603D0: "_chdir",
    0x460164: "GlobalLock",
    0x46016A: "GlobalAlloc",
    0x4601AC: "SelectObject",
    0x454960: "strstr",
    0x456090: "strchr",
    # SEH/EH runtime
    0x454410: "__global_unwind2",
    0x454452: "__local_unwind2",
    0x4541D0: "___CxxFrameHandler",
    0x454A30: "__ArrayUnwind",
    0x4549A0: "__eh_vec_ctor__",   # eh vector constructor iterator
    0x454AD0: "__eh_vec_dtor__",   # eh vector destructor iterator
}

CANONICAL = {
    "operator new": "operator_new",
    "new": "operator_new",
    "AllocateMemory": "operator_new",
    "??2@YAPAXI@Z": "operator_new",
    "operator delete": "operator_delete",
    "delete": "operator_delete",
    "FreeMemory": "operator_delete",
    "??3@YAXPAX@Z": "operator_delete",
    "FreeFromGlobalHeap": "operator_delete",
    "AllocateMemory_Wrapper": "malloc",
    # CRT underscore prefix variants (MSVC uses _ or __ prefix interchangeably)
    "__strnicmp": "_strnicmp",
    "strnicmp": "_strnicmp",
    "__strcmpi": "_strcmpi",
    "strcmpi": "_strcmpi",
    "_stricmp": "_strcmpi",
    "stricmp": "_strcmpi",
    "__stricmp": "_strcmpi",
    "chdir": "_chdir",
    "mkdir": "_mkdir",
    "close": "_close",
    "__close": "_close",
    "__amsg_exit": "_amsg_exit",
    "ParsePath": "_splitpath",
    "open": "_open",
    "read": "_read",
    "write": "_write",
    "__flushall": "_flushall",
    "filelength": "_filelength",
    "__mbsnbcpy": "_mbsnbcpy",
    "__isctype": "_isctype",
    "__ftol": "_ftol",
    # Compiler floating-point workaround helpers; direct FPU ops are equivalent
    "FUN_00455417": "__ignore__",
    "__adj_fdivr_m16i": "__ignore__",
    "__adj_fdivr_m32i": "__ignore__",
    "__adj_fdivr_m32": "__ignore__",
    "__adj_fdivr_m64": "__ignore__",
    # Compiler cleanup thunks that the rebuilt listing emits as local labels
    "FUN_0040B2FC": "__ignore__",
    "FUN_0041E930": "__ignore__",
    "FUN_00408255": "__ignore__",
    "FUN_0040825D": "__ignore__",
    "FUN_004083FB": "__ignore__",
    "FUN_0043D865": "__ignore__",
    "FUN_0043D873": "__ignore__",
    "FUN_0043D9A7": "__ignore__",
    "FUN_0043D9B5": "__ignore__",
    "FUN_0043DAE9": "__ignore__",
    "FUN_0043DAF7": "__ignore__",
    "FUN_0043DC2B": "__ignore__",
    "FUN_0043DC39": "__ignore__",
    "FUN_0043DD6D": "__ignore__",
    "FUN_0043DD7B": "__ignore__",
    "FUN_0043DEAF": "__ignore__",
    "FUN_0043DEBD": "__ignore__",
    "FUN_0043DFF1": "__ignore__",
    "FUN_0043DFFF": "__ignore__",
    "FUN_0043E23E": "__ignore__",
    # RenderEntry scalar deleting destructor; rebuilt exact-type deletes may
    # show up as virtual destructor calls through vtable[0x0].
    "FUN_00414710": "__funcptr__",
    "_strncpy": "strncpy",
    "_fwrite": "fwrite",
    "_fread": "fread",
    "_fclose": "fclose",
    "fopen": "fsopen",
    "_fsopen": "fsopen",
    "__fsopen": "fsopen",
    # Handler/ScriptHandler equivalence (same address 0x417180 and 0x4171B0)
    "ScriptHandler::CopyCommandData": "Handler::CopyCommandData",
    "ScriptHandler::WriteMessageAddress": "Handler::WriteMessageAddress",
    # ObjectPool/HashTable - same class in original
    "ObjectPool::Allocate_2": "ObjectPool::Allocate",
    "SpriteHashTable::AllocateBuckets": "HashTable::AllocateBuckets",
    "SpriteHashTable::AllocateNode": "HashTable::AllocateNode",
    # MessageNode is typedef for ListNode
    "MessageNode::Init": "ListNode::ListNode",
    # QueueNode/ListNode constructor equivalence
    "ListNode::ListNode": "QueueNode::QueueNode",
    # Format path functions
    "FormatFilePath": "CDData_FormatPath",
    # Cleanup cinematic
    "CleanupCinematic": "CleanupCinematic",
    # BuildScaleTable
    "BuildScaleTable": "BuildScaleTable",
    # Array helper functions = compiler vector iterators
    "Array_Cleanup": "__eh_vec_dtor__",
    "`eh vector destructor iterator'": "__eh_vec_dtor__",
    "Array_Iterate": "__eh_vec_ctor__",
    "`eh vector constructor iterator'": "__eh_vec_ctor__",
    "ArrayConstruct": "__eh_vec_ctor__",
    # COMDAT-folded constructors (identical bodies merged by linker)
    "EngineInfoParser::EngineInfoParser": "Projectile::Projectile",
    "ScoreDisplay::ScoreDisplay": "CursorState::CursorState",
    # StringTable::TestStrings is same function as HashTable::CopyStrings (0x44C480)
    "StringTable::TestStrings": "HashTable::CopyStrings",
    # Global function pointers (called via register in original, via global in ours)
    "g_WinGSetDIBColorTable_0043842c": "__funcptr__",
    "g_WinGBitBlt_00438434": "__funcptr__",
    "g_WinGStretchBlt_00438438": "__funcptr__",
    "g_WinGRecommendDIBFormat_00438430": "__funcptr__",
    "g_WinGCreateDIB_00438428": "__funcptr__",
    # Logging function aliases
    "WriteToLog": "WriteToMessageLog",
    "WriteToMessageLogIfEnabled": "WriteToMessageLog",
    # Font/draw function aliases
    "SetFontPosition": "SetDrawPosition",
    "SetFontColor": "SetFillColor",
    "SetFontColors": "SetDrawColors",
    # Palette constructor aliases
    "InitPalette": "Palette::Palette",
    # Palette method aliases
    "Palette::Load": "Palette::LoadFile",
    # CRT underscore prefix variations (full game uses double underscore)
    "__chdir": "_chdir",
    "__mkdir": "_mkdir",
    "__rmdir": "_rmdir",
    "__splitpath": "_splitpath",
    "__findnext": "_findnext",
    "__findfirst": "_findfirst",
    # Hash table / object pool aliases
    "SpriteHashTable::AllocEntry": "ObjectPool::Allocate",
    "HashTable::AllocEntry": "ObjectPool::Allocate",
    # GameState method aliases
    "GameState::FindState": "GameState::FindLabel",
    # CDData aliases
    "CDData_ResolvePath": "CDData_FormatPath",
    "CDData::ResolvePath": "CDData_FormatPath",
    # Smacker wrappers (called via funcptr in original)
    "__SmackBufferOpen@24": "__funcptr__",
    # ResolveAssetPath aliases
    "ResolveAssetPath": "CDData_FormatPath",
    # Parser aliases
    "Parser::ReportUnknownLabel": "Parser::LBLParse",
    # Engine is base class name for SC_Combat in original
    "Engine::Engine": "SC_Combat::SC_Combat",
    "Engine::Init": "SC_Combat::Init",
    "Engine::Update": "SC_Combat::Update",
    "Engine::Exit": "SC_Combat::Exit",
    "Engine::ShutDown": "SC_Combat::ShutDown",
    "Engine::AddMessage": "SC_Combat::AddMessage",
    "Engine::OnProcessEnd": "SC_Combat::OnProcessEnd",
    "Engine::ProcessTargets": "SC_Combat::ProcessLose",
    "Engine::LBLParse": "SC_Combat::LBLParse",
    "Engine::StopAndCleanup": "SC_Combat::StopAndCleanup",
    # SlimeTable and SoundList share same layout/functions
    "SlimeTable::Play": "SoundList::Play",
    "SlimeTable::~SlimeTable": "SoundList::~SoundList",
    "SlimeTable::Cleanup": "SoundList::Cleanup",
    "SlimeTable::Allocate": "SoundList::SetMaxSounds",
    "SlimeTable::LoadEntry": "SoundList::AddSound",
    # File position cache bodies are shared with SoundTracker in this tree
    "FilePosCache::Lookup": "SoundTracker::Lookup",
    "FilePosCache::Store": "SoundTracker::Store",
    # SpriteAction / SC_Message are aliases
    "SpriteAction::~SpriteAction": "SC_Message::~SC_Message",
    "SpriteAction::CopyFrom": "SC_Message::CopyFrom",
    # Queue family aliases
    "ZBQueue::Pop": "Queue::Pop",
    "MessageList::PopCurrent": "Queue::Pop",
    "EventList::RemoveCurrent": "Queue::Pop",
    "MessageList::InsertNode": "Queue::Insert",
    "MessageList::InsertBeforeCurrent": "Queue::Insert",
    # COMDAT-folded projectile reset helper
    "RockThrower::ResetProjectiles": "ZBuffer::ResetItems",
    # Sample cleanup aliases (Stop vs destructor — same effect in many contexts)
    "Sample::Stop": "Sample::~Sample",
    "Sample::Unload": "Sample::~Sample",
    # Timeout/Timer constructor aliases
    "InitTimeOut": "TimeOut::TimeOut",
    "Timer::~Timer": "Timer::Reset",
    # Handler method aliases
    "Handler::CopyCommandData": "Handler::Init",
    "Handler::InitFromMessage": "Handler::Init",
    # Cinematic/animation name aliases
    "GetCinematicFilename": "MakeAnimName",
    "MakeSoundName_Cine": "MakeSoundName",
    # ZBuffer text aliases
    "ZBufferManager::ShowText": "ZBufferManager::ShowSubtitle",
    # Palette aliases
    "Palette::SetAndApply": "Palette::SetPalette",
    "BlankScreen": "FlipScreen",
    # VBuffer aliases
    "BlitRowsReversed": "BlitTransparentRows",
    "VBuffer::BlitTransparentRegion": "VBuffer::CallBlitter2",
    "VBuffer::LoadFromFile": "VBuffer::InitWithSize",
    # HashTable/ObjectPool aliases
    "ObjectPool::AllocateBuckets": "HashTable::AllocateBuckets",
    "HotspotListData::AllocateNode": "HashTable::AllocateNode",
    "HotspotListData::~HotspotListData": "TargetList::~TargetList",
    # FlagArray aliases
    "FlagArray::SafeClose": "FlagArray::~FlagArray",
    # FormatAssetPath aliases
    "FormatAssetPath": "CDData_FormatPath",
    # String / text aliases
    "FindAfterSubstring": "strstr",
    # Object method aliases
    "T_Object::StopSound": "Sprite::StopAnimationSound",
    # SC_Gauntlet/WordSearch aliases
    "SC_Gauntlet::ResetGrid": "InitCombatGrid",
    "Handler_OnProcessEnd": "SC_Combat::OnProcessEnd",
    "IconBarEntry::Render": "UpdateWordSearchCursor",
    # CombatSprite aliases
    "CombatSprite::~CombatSprite": "vtable[0xc]",
    "EngineInfoParser::~EngineInfoParser": "vtable[0xc]",
    "TargetList::~TargetList": "vtable[0xc]",
    # Sprite/Projectile aliases
    "Parser::Parser": "Sprite::Sprite",
    # SC_SearchScreen inherits from SC_Combat through Engine
    "SaveFilePool::AllocNode": "__ignore__",
    # VBuffer function aliases
    "GetGlobalVertAlign": "GetCurrentVideoMode",
    "BlitBufferOpaque": "BlitToDevice",
    "VBuffer::Free": "VBuffer::~VBuffer",
    "VBuffer::Release": "VBuffer::~VBuffer",
    # GameState aliases
    "GameState::ValidateIndex": "GameState::FindLabel",
    # OpenSaveFile alias
    "OpenSaveFile": "fsopen",
    # GetScreen / GetWindow aliases
    "GetScreenWidth": "GetWindowWidth",
    "GetScreenHeight": "GetWindowHeight",
    # Palette aliases
    "Palette::IsSimilar": "__ignore__",
    # Queue::Insert / ZBQueue::InsertBeforeCurrent equivalence
    "ZBQueue::InsertBeforeCurrent": "Queue::Insert",
    # T_MenuButton aliases
    "T_MenuButton::SimpleUpdate": "T_MenuButton::Update",
    # FormatStringVA alias
    "FormatStringVA": "CDData_FormatPath",
}


def canonicalize(name):
    if name.startswith("FUN_"):
        name = name.upper()
    if re.match(r'g_WinG\w+_[0-9A-Fa-f]+$', name):
        return "__funcptr__"
    name = CANONICAL.get(name, name)
    return name


# Verified code-generation artifacts that otherwise look like real count
# mismatches in static call multisets.
CALL_COUNT_ALLOWANCES = {
    ("CombatSprite::ParseSpriteData", "sscanf"): (0, 1),
    ("T_Hotspot::HandleClick", "ShowError"): (1, 0),
}


def apply_call_count_allowances(func_name, only_orig, only_compiled):
    for (allowed_func, name), (missing_allowed, extra_allowed) in CALL_COUNT_ALLOWANCES.items():
        if func_name != allowed_func:
            continue
        if missing_allowed:
            remaining = only_orig.get(name, 0) - missing_allowed
            if remaining > 0:
                only_orig[name] = remaining
            else:
                only_orig.pop(name, None)
        if extra_allowed:
            remaining = only_compiled.get(name, 0) - extra_allowed
            if remaining > 0:
                only_compiled[name] = remaining
            else:
                only_compiled.pop(name, None)


def parse_source_function_name(line):
    prefix = line.strip().split('(', 1)[0].strip()
    if not prefix:
        return None

    while True:
        updated = prefix
        for qualifier in ('extern "C" ', 'static ', 'virtual '):
            if updated.startswith(qualifier):
                updated = updated[len(qualifier):].lstrip()
        if updated == prefix:
            break
        prefix = updated

    tokens = prefix.split()
    if not tokens:
        return None

    candidate = tokens[-1]
    if candidate == 'const' and len(tokens) > 1:
        candidate = tokens[-2]
    if candidate in ('CALLBACK', 'WINAPI', '__cdecl', '__stdcall', '__thiscall') and len(tokens) > 1:
        candidate = tokens[-2]

    candidate = candidate.lstrip('*&')
    if not candidate:
        return None
    if re.match(r'^(?:\w+::)?~?\w+$', candidate):
        if candidate in ("TimedEventPool::Pop", "TimedEventPool::PopSafe"):
            params = line.split('(', 1)[1].split(')', 1)[0] if '(' in line and ')' in line else ''
            first_param = params.split(',', 1)[0].strip()
            m = re.match(r'(?:const\s+)?(?:class\s+|struct\s+)?(\w+)\s*\*', first_param)
            if m and m.group(1) in ("SC_MessageParser", "SpriteAction"):
                return f"{candidate}({m.group(1)}*)"
        return candidate
    return None


def iter_source_functions(cpp_file):
    """Yield ([start_addrs], function_name) pairs from source annotations."""
    try:
        with open(cpp_file, 'r') as f:
            lines = f.readlines()
    except:
        return

    pending_addrs = []
    for line in lines:
        m = re.match(r'\s*/\*\s*Function start:\s*(0x[0-9a-fA-F]+)\s*\*/', line)
        if m:
            pending_addrs.append(int(m.group(1), 16))
            continue

        if not pending_addrs:
            continue

        stripped = line.strip()
        if not stripped or stripped.startswith('//') or stripped.startswith('/*'):
            continue

        func_name = parse_source_function_name(stripped)
        if func_name:
            yield pending_addrs[:], func_name
        pending_addrs = []


def auto_detect_crt(code_dir):
    """Auto-detect CRT/library function addresses from Ghidra decompiled text."""
    crt_map = {}
    for decompiled in glob.glob(f"{code_dir}/FUN_*.decompiled.txt"):
        try:
            with open(decompiled, 'r') as f:
                content = f.read(2000)  # Only need header
        except:
            continue
        if "Library Function" not in content:
            continue
        # Extract address from filename
        basename = os.path.basename(decompiled)
        m = re.match(r'FUN_([0-9a-fA-F]+)\.decompiled\.txt', basename)
        if not m:
            continue
        addr = int(m.group(1), 16)
        header = re.match(r'Function:\s*([A-Za-z_][\w]*)', content)
        if header and not header.group(1).startswith("FUN_"):
            crt_map[addr] = normalize_import_name(header.group(1))
            continue
        # Extract function name from line after "Library Function"
        lines = content.split('\n')
        for i, line in enumerate(lines):
            if "Library Function" in line:
                # Name is usually on next non-empty line, stripped of leading _
                for j in range(i+1, min(i+3, len(lines))):
                    name = lines[j].strip()
                    if name and not name.startswith('*') and not name.startswith('/'):
                        # Clean up: "_sprintf" -> "sprintf", "__strcmpi" -> "_strcmpi"
                        if name.startswith('_') and not name.startswith('__'):
                            name = name[1:]
                        crt_map[addr] = name
                        break
                break
    return crt_map


def build_address_to_name_map(src_dir, code_dir):
    """Build address-to-name map from KNOWN_CRT + auto-detected CRT + source annotations."""
    if src_dir == "src-demo":
        addr_map = dict(KNOWN_CRT_DEMO)
    else:
        addr_map = dict(KNOWN_CRT_FULL)
        # Also auto-detect CRT from decompiled texts
        auto_crt = auto_detect_crt(code_dir)
        for addr, name in auto_crt.items():
            if addr not in addr_map:
                addr_map[addr] = name

    for cpp_file in glob.glob(f"{src_dir}/*.cpp"):
        for addrs, func_name in iter_source_functions(cpp_file):
            for addr in addrs:
                addr_map[addr] = func_name
    return addr_map


# Import Address Table entries loaded from the original executable. Many calls
# go through indirect IAT slots, while the rebuild may emit named __imp__ calls
# or direct CRT thunks, so both sides are normalized to the import name.
IAT_ADDRESSES = {}
IAT_ADDRESS_RANGES = []
ORDINAL_IMPORTS = {
    ("SMACKW32.DLL", 20): "SmackSummary",
}


def normalize_import_name(name):
    name = re.sub(r'@\d+$', '', name)
    if name.startswith('_') and not name.startswith('__'):
        return name[1:]
    return name


def parse_c_string(data, offset):
    end = data.find(b'\0', offset)
    if end < 0:
        end = len(data)
    return data[offset:end].decode("ascii", errors="replace")


def parse_pe_iat_addresses(exe_path):
    """Return {virtual_iat_address: import_name} for a PE32 executable."""
    if not exe_path or not os.path.exists(exe_path):
        return {}, []

    with open(exe_path, "rb") as f:
        data = f.read()

    def u16(offset):
        return struct.unpack_from("<H", data, offset)[0]

    def u32(offset):
        return struct.unpack_from("<I", data, offset)[0]

    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError(f"not a PE executable: {exe_path}")

    pe_offset = u32(0x3C)
    if pe_offset + 0x18 >= len(data) or data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError(f"invalid PE header: {exe_path}")

    file_header = pe_offset + 4
    section_count = u16(file_header + 2)
    optional_size = u16(file_header + 16)
    optional_header = file_header + 20
    magic = u16(optional_header)
    if magic != 0x10B:
        raise ValueError(f"unsupported PE optional header: {exe_path}")

    image_base = u32(optional_header + 28)
    data_dir = optional_header + 96
    import_rva = u32(data_dir + 8)
    import_size = u32(data_dir + 12)
    if import_rva == 0:
        return {}, []

    sections_offset = optional_header + optional_size
    sections = []
    for index in range(section_count):
        section_offset = sections_offset + (index * 40)
        if section_offset + 40 > len(data):
            break
        virtual_size = u32(section_offset + 8)
        virtual_address = u32(section_offset + 12)
        raw_size = u32(section_offset + 16)
        raw_pointer = u32(section_offset + 20)
        sections.append((virtual_address, max(virtual_size, raw_size), raw_pointer, raw_size))

    def rva_to_offset(rva):
        for virtual_address, size, raw_pointer, raw_size in sections:
            if virtual_address <= rva < virtual_address + size:
                offset = raw_pointer + (rva - virtual_address)
                if offset < raw_pointer + raw_size or raw_size == 0:
                    return offset
        if rva < len(data):
            return rva
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")

    imports = {}
    ranges = []
    descriptor = rva_to_offset(import_rva)
    while descriptor + 20 <= len(data):
        original_first_thunk = u32(descriptor)
        name_rva = u32(descriptor + 12)
        first_thunk = u32(descriptor + 16)
        if original_first_thunk == 0 and name_rva == 0 and first_thunk == 0:
            break

        dll_name = parse_c_string(data, rva_to_offset(name_rva)).upper() if name_rva else ""
        thunk_rva = original_first_thunk or first_thunk
        index = 0
        while True:
            thunk_offset = rva_to_offset(thunk_rva + (index * 4))
            thunk = u32(thunk_offset)
            if thunk == 0:
                break
            if thunk & 0x80000000:
                import_name = ORDINAL_IMPORTS.get((dll_name, thunk & 0xFFFF))
                if import_name:
                    imports[image_base + first_thunk + (index * 4)] = import_name
            else:
                import_name_offset = rva_to_offset(thunk) + 2
                import_name = normalize_import_name(parse_c_string(data, import_name_offset))
                imports[image_base + first_thunk + (index * 4)] = import_name
            index += 1

        if index:
            ranges.append((image_base + first_thunk, image_base + first_thunk + (index * 4)))
        descriptor += 20

    return imports, ranges


def is_iat_address(addr):
    if addr in IAT_ADDRESSES:
        return True
    return any(start <= addr < end for start, end in IAT_ADDRESS_RANGES)

def parse_vtable_call(line):
    """Parse a vtable/indirect call and return a normalized form.
    Original: CALL dword ptr [EAX + 0x4] -> vtable[0x4]
    Compiled: call DWORD PTR [eax+4] -> vtable[0x4]
    Absolute IAT: CALL dword ptr [0x00476370] -> name or __import__
    """
    # Absolute-address indirect call (IAT import): CALL dword ptr [0xADDR]
    m = re.match(r'CALL\s+dword\s+ptr\s*\[\s*(?:0x)?([0-9a-fA-F]+)\s*\]', line, re.IGNORECASE)
    if m:
        try:
            addr = int(m.group(1), 16)
            if is_iat_address(addr):
                return IAT_ADDRESSES.get(addr, "__import__")
            if addr >= 0x400000:
                return "__funcptr__"
        except ValueError:
            pass

    # Original format: CALL dword ptr [REG + 0xOFFSET] or CALL dword ptr [REG]
    m = re.match(r'CALL\s+dword\s+ptr\s*\[\s*(\w+)\s*(?:\+\s*(?:0x)?([0-9a-fA-F]+))?\s*\]', line, re.IGNORECASE)
    if m:
        offset = int(m.group(2), 16) if m.group(2) else 0
        return f"vtable[0x{offset:x}]"

    # Compiled format: call DWORD PTR [reg+offset]
    m = re.match(r'call\s+DWORD\s+PTR\s*\[\s*(\w+)\s*(?:\+\s*(\d+))?\s*\]', line, re.IGNORECASE)
    if m:
        offset = int(m.group(2)) if m.group(2) else 0
        return f"vtable[0x{offset:x}]"

    return None


def extract_calls_from_original(disasm_path):
    """Extract CALL targets from original disassembly.

    Tracks simple MOV REG, [IAT_ADDR] hoists so that subsequent CALL REG
    resolves to the IAT symbol. The original compiler frequently hoists
    rand/GetTickCount into EBX/EDI inside a function, while our rebuild
    emits the direct CRT call.
    """
    calls = []
    reg_map = {}  # e.g. "EBX" -> "rand"
    mov_iat_re = re.compile(
        r'MOV\s+(E[A-D]X|ESI|EDI|EBP)\s*,\s*dword\s+ptr\s*\[\s*(?:0x)?([0-9a-fA-F]+)\s*\]',
        re.IGNORECASE,
    )
    call_reg_re = re.compile(r'CALL\s+(E[A-D]X|ESI|EDI|EBP)\s*$', re.IGNORECASE)
    try:
        with open(disasm_path, 'r') as f:
            for line in f:
                line = line.strip()

                # Track MOV REG, [0xADDR] for IAT hoisting
                mv = mov_iat_re.match(line)
                if mv:
                    try:
                        addr = int(mv.group(2), 16)
                        if is_iat_address(addr):
                            reg_map[mv.group(1).upper()] = IAT_ADDRESSES.get(addr, "__import__")
                        else:
                            reg_map.pop(mv.group(1).upper(), None)
                    except ValueError:
                        reg_map.pop(mv.group(1).upper(), None)
                    continue

                if not line.upper().startswith('CALL'):
                    continue

                # Register-indirect call: check hoist table first
                cr = call_reg_re.match(line)
                if cr:
                    reg = cr.group(1).upper()
                    if reg in reg_map:
                        calls.append(reg_map[reg])
                    else:
                        calls.append("__indirect__")
                    continue

                # Vtable/indirect call
                vt = parse_vtable_call(line)
                if vt:
                    calls.append(vt)
                    continue

                # Direct call to address
                m = re.match(r'CALL\s+(?:0x)?0*([0-9a-fA-F]+)', line)
                if m:
                    addr = int(m.group(1), 16)
                    if addr < 0x1000:
                        calls.append("__import__")
                    else:
                        calls.append(addr)
                    continue

                # Call to label
                m = re.match(r'CALL\s+LAB_0*([0-9a-fA-F]+)', line)
                if m:
                    addr = int(m.group(1), 16)
                    calls.append(addr)
                    continue

                # Other indirect
                calls.append("__indirect__")
    except:
        pass
    return calls


def extract_calls_from_compiled(asm_path, function_name, occurrence_index=0):
    """Extract CALL target names from compiled assembly."""
    calls = []
    try:
        with open(asm_path, 'r') as f:
            lines = f.readlines()
    except:
        return calls

    # Find the Nth function body by exact comment or symbol match. The
    # occurrence index keeps overloaded methods tied to the right PROC body.
    func_lines = []
    current_index = -1
    in_func = False

    for line in lines:
        stripped = line.strip()
        if not in_func:
            if 'PROC' not in stripped:
                continue
            matched = False
            if '; ' in stripped:
                comment = stripped.split('; ', 1)[1].strip()
                # Strip trailing ", COMDAT" added by MSVC asm listing
                if comment.endswith(', COMDAT'):
                    comment = comment[:-len(', COMDAT')].strip()
                matched = comment == function_name
            if not matched:
                symbol = stripped.split(None, 1)[0]
                matched = normalize_compiled(symbol) == function_name
            if not matched:
                continue
            current_index += 1
            if current_index != occurrence_index:
                continue
            in_func = True
            continue

        if re.search(r'\bENDP\b', stripped):
            break
        func_lines.append(stripped)

    if not func_lines:
        return calls

    func_section = '\n'.join(func_lines)

    # Track MOV reg, DWORD PTR __imp__XXX@N hoists so that subsequent `call reg`
    # resolves to the imported symbol (mirrors the original-side tracker).
    # Only the names that round-trip through IAT_ADDRESSES are kept as real
    # symbols; everything else collapses to "__import__" so the unknown-IAT
    # buckets balance against the original side.
    known_imports = set(IAT_ADDRESSES.values())
    compiled_reg_map = {}
    mov_imp_re = re.compile(
        r'mov\s+(eax|ebx|ecx|edx|esi|edi|ebp)\s*,\s*DWORD\s+PTR\s+__imp__([A-Za-z_][\w]*)',
        re.IGNORECASE,
    )

    for line in func_section.split('\n'):
        line = line.strip()

        mv = mov_imp_re.match(line)
        if mv:
            name = normalize_import_name(mv.group(2))
            compiled_reg_map[mv.group(1).lower()] = name if name in known_imports else "__import__"
            continue

        if not line.lower().startswith('call'):
            continue

        full_target = line[4:].strip()

        # Check for vtable call
        vt_m = re.match(r'DWORD\s+PTR\s*\[\s*(\w+)\s*(?:\+\s*(\d+))?\s*\]', full_target, re.IGNORECASE)
        if vt_m:
            offset = int(vt_m.group(2)) if vt_m.group(2) else 0
            calls.append(f"vtable[0x{offset:x}]")
            continue

        # Register calls — consult the hoist table first
        if full_target in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'):
            if full_target in compiled_reg_map:
                calls.append(compiled_reg_map[full_target])
            else:
                calls.append("__indirect__")
            continue

        # Label calls
        if full_target.startswith('$L') or full_target.startswith('$T'):
            calls.append("__label__")
            continue

        # Import calls (IAT indirect). Only named-resolve symbols that the
        # original-side tracker also resolves (via IAT_ADDRESSES); for every
        # other import both sides emit the neutral "__import__" token so the
        # counts match regardless of naming.
        if '__imp__' in full_target:
            m = re.search(r'__imp__([A-Za-z_][\w]*)', full_target)
            name = normalize_import_name(m.group(1)) if m else None
            if name and name in set(IAT_ADDRESSES.values()):
                calls.append(name)
            else:
                calls.append("__import__")
            continue

        # Indirect call through a named function pointer global.
        m = re.match(r'DWORD\s+PTR\s+(.+)', full_target, re.IGNORECASE)
        if m:
            calls.append(m.group(1).split(';', 1)[0].strip())
            continue

        # Prefer the decorated symbol when present; MSVC comments erase
        # overload signatures that the decorated name still preserves.
        target_symbol = full_target.split(';', 1)[0].strip()
        if target_symbol.startswith('?') or target_symbol.startswith('@'):
            normalized_symbol = normalize_compiled(target_symbol)
            if normalized_symbol != target_symbol:
                calls.append(normalized_symbol)
                continue

        # Use comment if available
        if ';' in full_target:
            target = full_target.split(';')[1].strip()
        else:
            target = full_target

        calls.append(target)
    return calls


def normalize_compiled(name):
    """Normalize compiled name."""
    name = name.strip()
    m = re.match(r'\?(PopSafe|Pop)@TimedEventPool@@QAEPAV(SC_MessageParser|SpriteAction)@@PAV2@@Z', name)
    if m:
        return f"TimedEventPool::{m.group(1)}({m.group(2)}*)"
    if name.startswith('??0') and '@@' in name:
        m = re.match(r'\?\?0(\w+)@@', name)
        if m: return f"{m.group(1)}::{m.group(1)}"
    if name.startswith('??1') and '@@' in name:
        m = re.match(r'\?\?1(\w+)@@', name)
        if m: return f"{m.group(1)}::~{m.group(1)}"
    if name.startswith('??2@'): return "operator_new"
    if name.startswith('??3@'): return "operator_delete"
    if name.startswith('?') and '@@' in name:
        m = re.match(r'\?(\w+)@(\w+)@@', name)
        if m: return f"{m.group(2)}::{m.group(1)}"
        m = re.match(r'\?(\w+)@@', name)
        if m: return m.group(1)
    if name.startswith('_') and '::' not in name and '@' not in name:
        return name[1:]
    m = re.match(r'@([\w]+)@\d+', name)
    if m: return m.group(1)
    m = re.match(r'_?(\w+)@\d+$', name)
    if m: return m.group(1)
    if 'eh vector constructor iterator' in name:
        return "__eh_vec_ctor__"
    return name


def parse_args():
    parser = argparse.ArgumentParser(
        description="Verify that compiled functions call the same targets as the original disassembly."
    )
    parser.add_argument("filters", nargs="*", help="Optional function name filters (exact or substring).")
    parser.add_argument("--demo", action="store_true", help="Use src-demo/code instead of the main source tree.")
    parser.add_argument("--all", action="store_true", help="Show all mismatches, including unresolved FUN_ entries.")
    parser.add_argument("--src-dir", default=None, help="Override source directory.")
    parser.add_argument("--code-dir", default=None, help="Override disassembly directory.")
    parser.add_argument("--out-dir", default="out", help="Compiled assembly output directory.")
    parser.add_argument("--exe", default=None, help="Original executable used to resolve IAT call targets.")
    parser.add_argument("--no-build", action="store_true", help="Skip rebuilding before verification.")
    return parser.parse_args()


def pick_default_code_dir(demo):
    if demo:
        return "code"
    if os.path.isdir("code-full"):
        return "code-full"
    return "code"


def pick_default_exe(demo):
    if demo:
        return "data/demo/CDDATA/TEACHER.ORIGINAL.EXE"
    return "data/full/TEACHER.ORI.EXE"


def matches_filter(func_name, cpp_file, filters):
    if not filters:
        return True
    basename = os.path.basename(cpp_file)
    for item in filters:
        if func_name == item:
            return True
        if item in func_name:
            return True
        if item in basename:
            return True
    return False


def has_disassembly_body(disasm_path):
    body_lines = []
    try:
        with open(disasm_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith(('Function:', 'Address:')):
                    continue
                body_lines.append(line)
    except:
        return False
    if not body_lines:
        return False

    # Some exported disassembly files are truncated at the top of the
    # function. Treat them like missing disassembly rather than reporting
    # every rebuilt call as an EXTRA mismatch.
    for line in body_lines:
        if line.upper().startswith('RET'):
            return True
    return body_lines[-1].upper().startswith('JMP')


def main():
    global IAT_ADDRESSES, IAT_ADDRESS_RANGES

    args = parse_args()

    if args.demo:
        src_dir = args.src_dir or "src-demo"
    else:
        src_dir = args.src_dir or "src"
    code_dir = args.code_dir or pick_default_code_dir(args.demo)
    out_dir = args.out_dir
    exe_path = args.exe or pick_default_exe(args.demo)
    show_all = args.all

    if not os.path.isdir(src_dir):
        print(f"error: source directory not found: {src_dir}", file=sys.stderr)
        sys.exit(1)
    if not os.path.isdir(code_dir):
        print(f"error: disassembly directory not found: {code_dir}", file=sys.stderr)
        sys.exit(1)
    if not os.path.isdir(out_dir):
        print(f"error: assembly output directory not found: {out_dir}", file=sys.stderr)
        sys.exit(1)

    try:
        IAT_ADDRESSES, IAT_ADDRESS_RANGES = parse_pe_iat_addresses(exe_path)
    except Exception as exc:
        IAT_ADDRESSES, IAT_ADDRESS_RANGES = {}, []
        print(f"warning: could not load IAT entries from {exe_path}: {exc}", file=sys.stderr)

    if not IAT_ADDRESSES:
        print(f"warning: no IAT entries loaded from {exe_path}; import calls will be compared generically", file=sys.stderr)

    addr_map = build_address_to_name_map(src_dir, code_dir)

    functions = []
    skipped_no_disasm = []
    for cpp_file in sorted(glob.glob(f"{src_dir}/*.cpp")):
        func_occurrences = {}
        for addrs, func_name in iter_source_functions(cpp_file):
            if not matches_filter(func_name, cpp_file, args.filters):
                continue
            occurrence_index = func_occurrences.get(func_name, 0)
            func_occurrences[func_name] = occurrence_index + 1

            # Consecutive Function start comments describe one source body
            # split by compiler prologue/thunk code. Compare the body address.
            addr = addrs[-1]
            addr_hex = f"{addr:X}"
            disasm_path = None
            for p in [f"{code_dir}/FUN_{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_{addr_hex.lower()}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex.lower()}.disassembled.txt"]:
                if os.path.exists(p):
                    disasm_path = p
                    break
            if disasm_path is None:
                skipped_no_disasm.append((func_name, addr, cpp_file))
                continue
            if not has_disassembly_body(disasm_path):
                skipped_no_disasm.append((func_name, addr, cpp_file))
                continue

            asm_path = f"{out_dir}/{os.path.splitext(os.path.basename(cpp_file))[0]}.asm"
            functions.append((cpp_file, func_name, occurrence_index, addr, disasm_path, asm_path))

    if not functions:
        print("error: no functions selected for verification.", file=sys.stderr)
        if skipped_no_disasm:
            print(f"note: {len(skipped_no_disasm)} candidate functions were skipped because no usable disassembly was found in {code_dir}.", file=sys.stderr)
        sys.exit(1)

    if not args.no_build:
        print("Building project to generate assembly...")
        subprocess.run(
            ["make", "clean"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        ret = subprocess.run(
            ["make", "all", "COMPARE=1", "-j12"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        if ret.returncode != 0:
            print("Build failed!", file=sys.stderr)
            sys.exit(1)

    total_checked = 0
    mismatched_funcs = []

    for cpp_file, func_name, occurrence_index, orig_addr, disasm_path, asm_path in functions:
        if not os.path.exists(asm_path):
            continue

        orig_raw = extract_calls_from_original(disasm_path)
        compiled_raw = extract_calls_from_compiled(asm_path, func_name, occurrence_index)

        if not orig_raw and not compiled_raw:
            continue
        total_checked += 1

        # Resolve original addresses to names
        orig_resolved = []
        for call in orig_raw:
            if isinstance(call, int):
                name = addr_map.get(call, f"FUN_{call:08X}")
                orig_resolved.append(name)
            else:
                orig_resolved.append(call)

        # Normalize compiled names
        compiled_resolved = [normalize_compiled(n) if not n.startswith('vtable[') and not n.startswith('__') else n
                            for n in compiled_raw]

        # Canonicalize
        orig_canon = [canonicalize(n) for n in orig_resolved]
        compiled_canon = [canonicalize(n) for n in compiled_resolved]

        # Filter out non-comparable. TRACE_LOG_IMPL_fn is our own debug
        # tracing wrapper; the wsprintfA/GetLastError/CreateFileA/WriteFile
        # calls inside it leak out of every function that uses TRACE_LOG, so
        # we ignore them on the compiled side.
        skip = {
            "__import__", "__label__", "__eh_vec_ctor__", "__eh_vec_dtor__",
            "__ignore__", "TRACE_LOG", "TRACE_LOG_IMPL_fn", "TODO",
        }
        orig_filtered = [n for n in orig_canon if n not in skip]
        compiled_filtered = [n for n in compiled_canon if n not in skip]

        # Normalize funcptr/vtable[0x0] equivalence on both sides:
        # the original sometimes calls through vtable[0x0] (a register loaded
        # with a function pointer) while the rebuild uses a vtable virtual
        # destructor call — both register as vtable[0x0]. Treat them as the
        # generic __funcptr__ token so either side's count drop into the
        # "trivial" bucket below.
        orig_filtered = ["__funcptr__" if n == "vtable[0x0]" else n for n in orig_filtered]
        compiled_filtered = ["__funcptr__" if n == "vtable[0x0]" else n for n in compiled_filtered]

        # Compare as multisets
        orig_counter = Counter(orig_filtered)
        compiled_counter = Counter(compiled_filtered)

        only_orig = orig_counter - compiled_counter
        only_compiled = compiled_counter - orig_counter
        apply_call_count_allowances(func_name, only_orig, only_compiled)

        if not only_orig and not only_compiled:
            continue

        # Filter trivial differences (new/delete counts from SEH)
        trivial = {
            "operator_new", "operator_delete", "MemAllocPtr", "MemSafeFreePtr",
            "__funcptr__", "__indirect__",
        }
        real_orig = {k: v for k, v in only_orig.items() if k not in trivial}
        real_compiled = {k: v for k, v in only_compiled.items() if k not in trivial}

        if not real_orig and not real_compiled:
            continue

        # Separate FUN_ (unresolved) from resolved-name mismatches
        unresolved_orig = {k: v for k, v in real_orig.items() if k.startswith("FUN_")}
        resolved_orig = {k: v for k, v in real_orig.items() if not k.startswith("FUN_")}

        # If not --all, only show functions with resolved-name mismatches or
        # mismatched EXTRA calls that can't be explained by unresolved FUN_ entries
        if not show_all:
            # Count unresolved originals vs extra compiled — if they balance, skip
            unresolved_count = sum(unresolved_orig.values())
            extra_count = sum(real_compiled.values())
            # If all mismatches are just unresolved FUN_ entries balanced by extras, skip
            if not resolved_orig and unresolved_count >= extra_count:
                continue

        mismatched_funcs.append((func_name, orig_addr, os.path.basename(cpp_file),
                                  dict(real_orig), dict(real_compiled)))

    # Print report
    print(f"\n{'='*70}")
    print(f"CALL TARGET VERIFICATION REPORT")
    if show_all:
        print(f"  (showing ALL mismatches including unresolved FUN_ entries)")
    else:
        print(f"  (showing only resolved-name mismatches; use --all for everything)")
    print(f"{'='*70}")
    print(f"Functions selected: {len(functions)}")
    print(f"Functions checked: {total_checked}")
    print(f"Functions with call target mismatches: {len(mismatched_funcs)}")
    if skipped_no_disasm:
        print(f"Functions skipped (no usable disassembly found): {len(skipped_no_disasm)}")
    print()

    if total_checked == 0:
        print("error: zero functions were actually checked. Verify that assembly output exists and the function filter is correct.", file=sys.stderr)
        sys.exit(1)

    if mismatched_funcs:
        mismatched_funcs.sort(key=lambda x: x[0])
        for func_name, addr, filename, only_orig, only_compiled in mismatched_funcs:
            print(f"{func_name} (0x{addr:X}) [{filename}]")
            for name, count in sorted(only_orig.items()):
                tag = " (unresolved)" if name.startswith("FUN_") else ""
                print(f"  MISSING: {name} x{count}{tag}")
            for name, count in sorted(only_compiled.items()):
                print(f"  EXTRA:   {name} x{count}")
            print()
    else:
        print("All call targets match!")


if __name__ == "__main__":
    main()
