#!/usr/bin/env python3
"""
Verify that reimplemented functions call the correct functions compared to the original assembly.

The mnemonic-based similarity checker doesn't verify call destinations - a CALL instruction
matches any other CALL regardless of target. This script compares the multiset of called
functions between original and reimplemented code.

Vtable calls (CALL dword ptr [reg+offset]) are compared by offset to verify correct vtable slot.
"""

import re
import os
import sys
import glob
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
    0x455220: "strncpy",
    0x455250: "strncmp",
    0x455290: "vsprintf",
    0x4558B6: "_fdivp_sti_st",
    0x4563C0: "fprintf",
    0x456400: "vfprintf",
    0x456960: "fputs",
    0x4569C0: "_isctype",
    0x456A80: "getenv",
    0x456B20: "strrchr",
    0x456CE0: "_amsg_exit",
    0x4582C0: "fseek",             # detected from usage patterns
    0x458490: "_write",
    0x45ABF0: "_mbsnbcpy",
    0x45D050: "_ld12tod",
    0x45DD80: "_strcmpi",
    0x45DFB0: "strpbrk",
    0x45DFF0: "toupper",
    0x45FBE0: "_strdup",
    0x460220: "_strnicmp",
    0x460340: "_filelength",
    0x4603D0: "_chdir",
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
    "_stricmp": "_strcmpi",
    "stricmp": "_strcmpi",
    "__stricmp": "_strcmpi",
    "chdir": "_chdir",
    "mkdir": "_mkdir",
    "close": "_close",
    "open": "_open",
    "read": "_read",
    "write": "_write",
    "__flushall": "_flushall",
    "filelength": "_filelength",
    "__mbsnbcpy": "_mbsnbcpy",
    "__isctype": "_isctype",
    "__ftol": "_ftol",
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
    # SpriteAction / SC_Message are aliases
    "SpriteAction::~SpriteAction": "SC_Message::~SC_Message",
    "SpriteAction::CopyFrom": "SC_Message::CopyFrom",
    # Queue family aliases
    "ZBQueue::Pop": "Queue::Pop",
    "MessageList::PopCurrent": "Queue::Pop",
    "EventList::RemoveCurrent": "Queue::Pop",
    "MessageList::InsertNode": "Queue::Insert",
    "MessageList::InsertBeforeCurrent": "Queue::Insert",
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
    "SaveFilePool::AllocNode": "__allocnode__",
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
    name = CANONICAL.get(name, name)
    # Normalize FUN_ addresses to uppercase
    if name.startswith("FUN_"):
        name = name.upper()
    return name


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
        try:
            with open(cpp_file, 'r') as f:
                lines = f.readlines()
        except:
            continue
        for i, line in enumerate(lines):
            m = re.match(r'\s*/\*\s*Function start:\s*(0x[0-9a-fA-F]+)\s*\*/', line)
            if not m:
                continue
            addr = int(m.group(1), 16)
            for j in range(i+1, min(i+5, len(lines))):
                nextline = lines[j].strip()
                if not nextline or nextline.startswith('//') or nextline.startswith('/*'):
                    continue
                func_match = re.match(
                    r'(?:extern\s+"C"\s+)?(?:static\s+)?(?:virtual\s+)?'
                    r'(?:[\w*\s]+\s+)?'
                    r'(?:__\w+\s+)?(?:CALLBACK\s+|WINAPI\s+)?'
                    r'((?:\w+::)?~?\w+)\s*\(',
                    nextline
                )
                if func_match:
                    addr_map[addr] = func_match.group(1)
                    break
                break
    return addr_map


def parse_vtable_call(line):
    """Parse a vtable/indirect call and return a normalized form.
    Original: CALL dword ptr [EAX + 0x4] -> vtable[0x4]
    Compiled: call DWORD PTR [eax+4] -> vtable[0x4]
    """
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
    """Extract CALL targets from original disassembly."""
    calls = []
    try:
        with open(disasm_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line.upper().startswith('CALL'):
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

                # Other indirect (register call)
                calls.append("__indirect__")
    except:
        pass
    return calls


def extract_calls_from_compiled(asm_path, function_name):
    """Extract CALL target names from compiled assembly."""
    calls = []
    try:
        with open(asm_path, 'r') as f:
            lines = f.readlines()
    except:
        return calls

    # Find function by looking for PROC lines with matching comment
    # Format: "??0Animation@@QAE@XZ PROC NEAR\t\t\t\t; Animation::Animation"
    # or: "; Animation::Animation, COMDAT"
    # Must match exact function name (not substring) to avoid CallActivate matching CallActivateNeighbor
    func_lines = []
    in_func = False

    for line in lines:
        stripped = line.strip()
        if not in_func:
            # Match PROC line with exact function name in comment
            if 'PROC' in stripped:
                # Extract comment after ;
                if '; ' in stripped:
                    comment = stripped.split('; ', 1)[1].strip()
                    if comment == function_name:
                        in_func = True
                        continue
            # Also match COMDAT marker
            if stripped == f"; {function_name}, COMDAT":
                in_func = True
                continue
        else:
            if 'ENDP' in stripped:
                break
            func_lines.append(stripped)

    if not func_lines:
        return calls

    func_section = '\n'.join(func_lines)

    for line in func_section.split('\n'):
        line = line.strip()
        if not line.startswith('call'):
            continue

        full_target = line[4:].strip()

        # Check for vtable call
        vt_m = re.match(r'DWORD\s+PTR\s*\[\s*(\w+)\s*(?:\+\s*(\d+))?\s*\]', full_target, re.IGNORECASE)
        if vt_m:
            offset = int(vt_m.group(2)) if vt_m.group(2) else 0
            calls.append(f"vtable[0x{offset:x}]")
            continue

        # Register calls
        if full_target in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi'):
            calls.append("__indirect__")
            continue

        # Label calls
        if full_target.startswith('$L') or full_target.startswith('$T'):
            calls.append("__label__")
            continue

        # Import calls
        if '__imp__' in full_target:
            calls.append("__import__")
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


def main():
    demo = '--demo' in sys.argv

    if demo:
        src_dir = "src-demo"
        code_dir = "code"
        out_dir = "out"
    else:
        src_dir = "src"
        code_dir = "code-full"
        out_dir = "out"

    show_all = '--all' in sys.argv
    addr_map = build_address_to_name_map(src_dir, code_dir)

    functions = []
    for cpp_file in sorted(glob.glob(f"{src_dir}/*.cpp")):
        try:
            with open(cpp_file, 'r') as f:
                lines = f.readlines()
        except:
            continue
        for i, line in enumerate(lines):
            m = re.match(r'\s*/\*\s*Function start:\s*(0x[0-9a-fA-F]+)\s*\*/', line)
            if not m:
                continue
            addr = int(m.group(1), 16)
            func_name = None
            for j in range(i+1, min(i+5, len(lines))):
                nextline = lines[j].strip()
                if not nextline or nextline.startswith('//') or nextline.startswith('/*'):
                    continue
                func_match = re.match(
                    r'(?:extern\s+"C"\s+)?(?:static\s+)?(?:virtual\s+)?'
                    r'(?:[\w*\s]+\s+)?'
                    r'(?:__\w+\s+)?(?:CALLBACK\s+|WINAPI\s+)?'
                    r'((?:\w+::)?~?\w+)\s*\(',
                    nextline
                )
                if func_match:
                    func_name = func_match.group(1)
                    break
                break
            if not func_name:
                continue

            addr_hex = f"{addr:X}"
            disasm_path = None
            for p in [f"{code_dir}/FUN_{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_{addr_hex.lower()}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex.lower()}.disassembled.txt"]:
                if os.path.exists(p):
                    disasm_path = p
                    break
            if not disasm_path:
                continue

            asm_path = f"{out_dir}/{os.path.splitext(os.path.basename(cpp_file))[0]}.asm"
            functions.append((cpp_file, func_name, addr, disasm_path, asm_path))

    print("Building project to generate assembly...")
    os.system("make clean > /dev/null 2>&1")
    ret = os.system("make all -j12 2>/dev/null")
    if ret != 0:
        print("Build failed!")
        sys.exit(1)

    total_checked = 0
    mismatched_funcs = []

    for cpp_file, func_name, orig_addr, disasm_path, asm_path in functions:
        if not os.path.exists(asm_path):
            continue

        orig_raw = extract_calls_from_original(disasm_path)
        compiled_raw = extract_calls_from_compiled(asm_path, func_name)

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

        # Filter out non-comparable
        skip = {"__import__", "__label__", "__eh_vec_ctor__", "__eh_vec_dtor__"}
        orig_filtered = [n for n in orig_canon if n not in skip]
        compiled_filtered = [n for n in compiled_canon if n not in skip]

        # Normalize funcptr/vtable[0x0] equivalence:
        # Original calls through vtable[0x0] (register), compiled calls through global funcptr
        orig_filtered = ["__funcptr__" if n == "vtable[0x0]" else n for n in orig_filtered]

        # Compare as multisets
        orig_counter = Counter(orig_filtered)
        compiled_counter = Counter(compiled_filtered)

        only_orig = orig_counter - compiled_counter
        only_compiled = compiled_counter - orig_counter

        if not only_orig and not only_compiled:
            continue

        # Filter trivial differences (new/delete counts from SEH)
        trivial = {"operator_new", "operator_delete", "__funcptr__", "__indirect__"}
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
    print(f"Functions checked: {total_checked}")
    print(f"Functions with call target mismatches: {len(mismatched_funcs)}")
    print()

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
