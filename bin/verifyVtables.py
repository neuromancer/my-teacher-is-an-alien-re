#!/usr/bin/env python3
"""
Verify vtables from the original executable against reimplemented source code.

For every vtable override (function pointer that differs from the parent class),
checks that a matching /* Function start: 0xXXXXXX */ comment exists in src/.
Trivial stubs (RET, return 0) are flagged as low priority.

Usage:
  python3 bin/verifyVtables.py            # Summary table
  python3 bin/verifyVtables.py --dump     # Full vtable dump
  python3 bin/verifyVtables.py --class SC_Timer
"""
import struct
import re
import os
import sys
import glob
import argparse
from cppSourceParser import (
    CPP_PARSER,
    find_function_declarator,
    node_text,
    parse_source_function_comments,
    parse_source_function_markers,
    sanitize_source,
    walk,
)
from projectConfig import (
    ConfigError,
    add_config_argument,
    config_or_arg,
    load_config,
    mode_paths,
    parse_int,
    parse_range,
    parse_slot_map,
)

SLOT_SYMBOL_ALIASES = {}
VTABLE_MANUAL_CLASSES = []
VTABLE_DUPLICATE_CLASS_DROPS = []
VTABLE_CLASS_OVERRIDES = {}
VTABLE_SLOT_NAME_SETS = []


def configure_vtable_checker(config):
    global SLOT_SYMBOL_ALIASES, VTABLE_MANUAL_CLASSES, VTABLE_DUPLICATE_CLASS_DROPS
    global VTABLE_CLASS_OVERRIDES, VTABLE_SLOT_NAME_SETS

    vtables_config = config.get('vtables', {})
    SLOT_SYMBOL_ALIASES = {
        (item['class'], int(item['slot'])): {tuple(alias) for alias in item.get('aliases', [])}
        for item in vtables_config.get('slot_symbol_aliases', [])
    }
    VTABLE_MANUAL_CLASSES = list(vtables_config.get('manual_classes', []))
    VTABLE_DUPLICATE_CLASS_DROPS = list(vtables_config.get('duplicate_class_drops', []))
    VTABLE_CLASS_OVERRIDES = dict(vtables_config.get('class_overrides', {}))
    VTABLE_SLOT_NAME_SETS = [
        {
            'class_or_ancestor': item['class_or_ancestor'],
            'slots': parse_slot_map(item.get('slots', {})),
        }
        for item in vtables_config.get('slot_name_sets', [])
    ]


def strip_inline_comments(text):
    text = re.sub(r'//.*', '', text)
    text = re.sub(r'/\*.*?\*/', '', text)
    return text


def extract_slot_index(text):
    m = re.search(r'\[(\d+)\]', text)
    if m:
        return int(m.group(1))
    m = re.search(r'\bvtable\[(\d+)\]', text, re.IGNORECASE)
    if m:
        return int(m.group(1))
    m = re.search(r'\+0x([0-9a-fA-F]+)', text)
    if m:
        return int(m.group(1), 16) // 4
    return None


def split_params(params):
    parts = []
    current = []
    depth = 0
    for ch in params:
        if ch in '(<[':
            depth += 1
        elif ch in ')>]':
            if depth > 0:
                depth -= 1
        if ch == ',' and depth == 0:
            parts.append(''.join(current))
            current = []
            continue
        current.append(ch)
    if current:
        parts.append(''.join(current))
    return parts


def normalize_param(param):
    param = strip_inline_comments(param)
    param = param.split('=')[0].strip()
    if not param or param == 'void':
        return ''
    param = re.sub(r'\b(class|struct|const|volatile)\s+', '', param)
    param = re.sub(r'\s+', ' ', param)
    param = re.sub(r'\s*([*&])\s*', r'\1', param)
    m = re.match(r'(.+[\s*&])([A-Za-z_]\w*)$', param)
    if m:
        param = m.group(1).strip()
    return param


def build_signature_key(method_name, params, is_const=False):
    if method_name.startswith('~'):
        return '__dtor__'
    norm_params = [normalize_param(p) for p in split_params(params)]
    norm_params = [p for p in norm_params if p]
    key = f"{method_name}({','.join(norm_params)})"
    if is_const:
        key += " const"
    return key


def parse_method_declaration(decl, class_name):
    original = decl
    decl = strip_inline_comments(decl).strip()
    if not decl:
        return None
    decl = re.sub(r'\s+', ' ', decl)
    if '{' in decl:
        decl = decl.split('{', 1)[0].strip()
    if decl.endswith(';'):
        decl = decl[:-1].strip()

    m = re.match(
        r'^(?P<virtual>virtual\s+)?(?P<prefix>.*?)(?P<name>~?\w+)\s*\((?P<params>[^()]*)\)\s*(?P<const>const\b)?(?:\s*=\s*0)?$',
        decl
    )
    if not m:
        return None

    method_name = m.group('name')
    params = m.group('params') or ''
    is_virtual = bool(m.group('virtual'))
    is_const = bool(m.group('const'))

    return {
        'class_name': class_name,
        'method_name': method_name,
        'signature_key': build_signature_key(method_name, params, is_const),
        'is_virtual': is_virtual,
        'slot_index': extract_slot_index(original),
        'display_name': f"{class_name}::{method_name}",
    }


def parse_source_tree(path):
    with open(path, 'rb') as f:
        source = f.read()
    return source, CPP_PARSER.parse(sanitize_source(source))


def line_context(source, node):
    lines = source.splitlines()
    start = node.start_point.row
    end = node.end_point.row
    return ' '.join(
        lines[i].decode('utf-8', errors='ignore')
        for i in range(start, min(end + 1, len(lines)))
    )


def text_before_node_comments(source, tree, node, max_bytes=1500):
    min_start = max(0, node.start_byte - max_bytes)
    comments = []
    for candidate in walk(tree.root_node):
        if candidate.type != 'comment':
            continue
        if min_start <= candidate.start_byte < node.start_byte:
            comments.append(node_text(source, candidate))
    return ' '.join(comments)


def first_child_of_type(node, type_name):
    for child in node.children:
        if child.type == type_name:
            return child
    return None


def class_parent_from_node(source, class_node):
    base_clause = first_child_of_type(class_node, 'base_class_clause')
    if base_clause is None:
        return None
    for child in base_clause.children:
        if child.type in ('type_identifier', 'qualified_identifier'):
            return node_text(source, child).split('::')[-1].strip()
    return None


def parameter_list_text(source, function_declarator):
    params = function_declarator.child_by_field_name('parameters')
    if params is None:
        params = first_child_of_type(function_declarator, 'parameter_list')
    if params is None:
        return ''
    text = node_text(source, params).strip()
    if text.startswith('(') and text.endswith(')'):
        return text[1:-1]
    return text


def method_name_from_declarator(source, function_declarator):
    name_node = function_declarator.child_by_field_name('declarator')
    if name_node is None:
        return None
    name = node_text(source, name_node).strip()
    if '::' in name:
        name = name.split('::')[-1]
    return name


def parse_class_method_node(source, member_node, class_name, basename):
    function_declarator = find_function_declarator(member_node)
    if function_declarator is None:
        return None

    method_name = method_name_from_declarator(source, function_declarator)
    if not method_name or not re.match(r'^~?[A-Za-z_]\w*$', method_name):
        return None

    member_text = node_text(source, member_node)
    context = f"{member_text} {line_context(source, member_node)}"
    is_virtual = any(child.type == 'virtual' for child in member_node.children)
    is_const = bool(re.search(r'\)\s*const\b', member_text))
    params = parameter_list_text(source, function_declarator)

    return {
        'class_name': class_name,
        'method_name': method_name,
        'signature_key': build_signature_key(method_name, params, is_const),
        'is_virtual': is_virtual,
        'slot_index': extract_slot_index(context),
        'display_name': f"{class_name}::{method_name}",
        'file': basename,
        'line_number': member_node.start_point.row + 1,
    }


def iter_class_nodes(tree):
    for node in walk(tree.root_node):
        if node.type not in ('class_specifier', 'struct_specifier'):
            continue
        name_node = node.child_by_field_name('name')
        body_node = node.child_by_field_name('body')
        if name_node is None or body_node is None:
            continue
        yield node, name_node, body_node


def parse_header_metadata(src_dir):
    vtable_pat = re.compile(r'\bvtable(?:\s+address)?(?:\s+at)?[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)
    ctor_pat = re.compile(r'Constructor[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)

    hierarchy = {}
    class_header = {}
    class_methods = {}
    explicit_vtables = {}
    constructors = {}

    for hfile in sorted(glob.glob(os.path.join(src_dir, '*.h'))):
        basename = os.path.basename(hfile)
        source, tree = parse_source_tree(hfile)

        for class_node, name_node, body_node in iter_class_nodes(tree):
            class_name = node_text(source, name_node).strip()
            parent = class_parent_from_node(source, class_node)
            if parent:
                hierarchy[class_name] = parent
            class_header[class_name] = basename
            class_methods.setdefault(class_name, [])

            comment_context = text_before_node_comments(source, tree, class_node)
            for match in vtable_pat.finditer(comment_context):
                explicit_vtables[class_name] = int(match.group(1), 16)
            for match in ctor_pat.finditer(comment_context):
                constructors[class_name] = int(match.group(1), 16)

            for member_node in body_node.children:
                if member_node.type not in ('field_declaration', 'declaration', 'function_definition'):
                    continue
                parsed = parse_class_method_node(source, member_node, class_name, basename)
                if parsed:
                    class_methods[class_name].append(parsed)

    return hierarchy, class_header, class_methods, explicit_vtables, constructors

# ── PE parsing ──────────────────────────────────────────────────────────────

def parse_pe(filename):
    with open(filename, 'rb') as f:
        f.seek(0x3C)
        pe_off = struct.unpack('<I', f.read(4))[0]
        f.seek(pe_off + 6)
        num_sections = struct.unpack('<H', f.read(2))[0]
        f.seek(pe_off + 20)
        opt_size = struct.unpack('<H', f.read(2))[0]
        f.seek(pe_off + 52)
        image_base = struct.unpack('<I', f.read(4))[0]
        section_start = pe_off + 24 + opt_size
        f.seek(section_start)
        sections = {}
        for _ in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            sections[name] = {
                'va_start': image_base + va,
                'va_end': image_base + va + vsize,
                'rawptr': rawptr,
                'rawsize': rawsize,
            }
    return image_base, sections


def read_bytes_at_va(f, sections, va, size):
    """Read raw bytes at a virtual address."""
    for sec in sections.values():
        if sec['va_start'] <= va < sec['va_end']:
            off = va - sec['va_start']
            if off + size > sec['rawsize']:
                return None
            f.seek(sec['rawptr'] + off)
            return f.read(size)
    return None


def read_dword(f, sections, va):
    data = read_bytes_at_va(f, sections, va, 4)
    return struct.unpack('<I', data)[0] if data else None


# ── Stub detection ──────────────────────────────────────────────────────────

def classify_function(f, sections, addr):
    """
    Classify a function as 'stub', 'ret0', 'error', or 'real'.
    Reads first few bytes to detect trivial patterns.
    """
    data = read_bytes_at_va(f, sections, addr, 16)
    if not data:
        return 'unknown'

    # JMP dword ptr [...] import/helper thunk
    if data[0:2] == b'\xff\x25':
        return 'thunk'
    # Pure RET
    if data[0] == 0xC3:
        return 'stub'
    # RET imm16
    if data[0] == 0xC2:
        return 'stub'
    # XOR EAX,EAX; RET or RET imm
    if data[0:2] == b'\x33\xc0' and data[2] in (0xC3, 0xC2):
        return 'ret0'
    if data[0:2] in (b'\x31\xc0', b'\x32\xc0', b'\x30\xc0') and data[2] in (0xC3, 0xC2):
        return 'ret0'
    if data[0] == 0xB0 and data[2] in (0xC3, 0xC2):
        return 'retconst'
    # MOV EAX,1; RET
    if data[0:5] == b'\xb8\x01\x00\x00\x00' and data[5] in (0xC3, 0xC2):
        return 'ret1'
    if data[0] == 0xB8 and data[5] in (0xC3, 0xC2):
        return 'retconst'
    # Simple `return this->field` helpers.
    if data[0:3] == b'\x8b\x41\x08' and data[3] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0] == 0x8B and data[1] == 0x41 and data[3] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0] == 0x8B and data[1] == 0x81 and data[6] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0:2] == b'\x8a\x41' and data[3] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0:2] == b'\x8a\x81' and data[6] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0:3] == b'\x66\x8b\x41' and data[4] in (0xC3, 0xC2):
        return 'fieldget'
    if data[0:3] == b'\x66\x8b\x81' and data[7] in (0xC3, 0xC2):
        return 'fieldget'
    # PUSH string_addr; CALL ShowError pattern (error handler)
    if data[0] == 0x68 and data[5] == 0xE8:
        return 'error'

    return 'real'


def is_trivial_function_type(ftype):
    return ftype in ('stub', 'ret0', 'ret1', 'retconst', 'fieldget', 'thunk')


# ── Vtable address collection ──────────────────────────────────────────────

def find_vtable_addrs_from_disasm(code_full_dir, rdata_min, rdata_max):
    """Find all vtable addresses from MOV patterns in disassembled constructors."""
    pat = re.compile(r'MOV dword ptr \[E[A-Z]+\],0x([0-9a-fA-F]+)', re.IGNORECASE)
    addrs = set()
    for fn in glob.glob(os.path.join(code_full_dir, '*.disassembled.txt')):
        with open(fn, 'r') as fh:
            for line in fh:
                m = pat.search(line)
                if m:
                    addr = int(m.group(1), 16)
                    if rdata_min <= addr < rdata_max:
                        addrs.add(addr)
    return addrs


def find_vtable_addrs_from_headers(src_dir, rdata_min, rdata_max):
    """Find vtable addresses from header comments (full game only)."""
    pat = re.compile(r'\bvtable(?:\s+address)?(?:\s+at)?[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)
    addrs = set()
    for hfile in glob.glob(os.path.join(src_dir, '*.h')):
        source, tree = parse_source_tree(hfile)
        for node in walk(tree.root_node):
            if node.type != 'comment':
                continue
            for match in pat.finditer(node_text(source, node)):
                addr = int(match.group(1), 16)
                if rdata_min <= addr < rdata_max:
                    addrs.add(addr)
    return addrs


# ── Header/source symbol parsing ───────────────────────────────────────────

def parse_header_classes(src_dir):
    hierarchy, class_header, class_methods, explicit_vtables, _ = parse_header_metadata(src_dir)
    return hierarchy, class_header, class_methods, explicit_vtables


def build_expected_vtable_slots(class_name, classes, class_methods, cache):
    if class_name in cache:
        return cache[class_name]

    parent = classes.get(class_name, {}).get('parent')
    if parent:
        slots = list(build_expected_vtable_slots(parent, classes, class_methods, cache))
    else:
        slots = []

    slot_index = {}
    for idx, slot in enumerate(slots):
        if slot is not None:
            slot_index[slot['signature_key']] = idx

    for decl in class_methods.get(class_name, []):
        method_name = decl['method_name']
        if method_name == class_name:
            continue

        explicit_slot = decl.get('slot_index')
        key = decl['signature_key']

        if explicit_slot is not None:
            while len(slots) <= explicit_slot:
                slots.append(None)
            slots[explicit_slot] = decl
            slot_index[key] = explicit_slot
        elif key in slot_index:
            slots[slot_index[key]] = decl
        elif decl['is_virtual']:
            slot_index[key] = len(slots)
            slots.append(decl)

    cache[class_name] = slots
    return slots


def find_source_function_symbols(src_dir):
    """
    Parse source files and map Function start comments to the following symbol.
    Returns dict: address -> list of {class_name, method_name, file, line_number}
    """
    function_symbols = {}

    def record_symbol(address, class_name, method_name, basename, lineno):
        new_info = {
            'class_name': class_name,
            'method_name': method_name,
            'file': basename,
            'line_number': lineno,
        }
        function_symbols.setdefault(address, [])
        if new_info not in function_symbols[address]:
            function_symbols[address].append(new_info)

    def split_symbol(name):
        if '::' not in name:
            return None, name
        class_name, method_name = name.rsplit('::', 1)
        return class_name, method_name

    for pattern in ['*.cpp', '*.h']:
        for srcfile in sorted(glob.glob(os.path.join(src_dir, pattern))):
            basename = os.path.basename(srcfile)
            source, tree = parse_source_tree(srcfile)
            class_ranges = []
            for class_node, name_node, body_node in iter_class_nodes(tree):
                class_ranges.append((body_node.start_byte, body_node.end_byte, node_text(source, name_node).strip()))

            for marker in parse_source_function_markers(srcfile, include_no_assembly=True):
                class_name, method_name = split_symbol(marker.name)
                if class_name is None:
                    for start, end, candidate_class in class_ranges:
                        if start <= marker.function_start < end:
                            class_name = candidate_class
                            break
                if class_name is None:
                    continue
                record_symbol(int(marker.address, 16), class_name, method_name, basename, marker.line)

    return function_symbols


# ── Source parsing ──────────────────────────────────────────────────────────

def parse_class_info(src_dir, code_full_dir, rdata_min, rdata_max):
    """
    Parse .h files for class hierarchy and vtable addresses.
    Uses constructor disassembly to fix incorrect/demo vtable addresses.
    """
    classes = {}

    this_mov_pat = re.compile(r'MOV (E[A-Z]+),ECX')
    mov_pat = re.compile(r'MOV dword ptr \[(E[A-Z]+)\],0x([0-9a-fA-F]+)', re.IGNORECASE)
    hierarchy, class_header, _, explicit_vtables, constructors = parse_header_metadata(src_dir)

    for cls_name, parent_name in hierarchy.items():
        if cls_name in classes:
            continue

        # Find vtable address from comments
        vtable_addr = explicit_vtables.get(cls_name)
        if vtable_addr is not None and not (rdata_min <= vtable_addr < rdata_max):
            vtable_addr = None

        # Find constructor address
        ctor_addr = constructors.get(cls_name)

        # Use constructor to find/verify vtable address
        if ctor_addr:
            ctor_file = os.path.join(code_full_dir, f"FUN_{ctor_addr:X}.disassembled.txt")
            if os.path.exists(ctor_file):
                with open(ctor_file, 'r') as cf:
                    this_reg = None
                    per_reg_vtables = {}
                    any_vtable = None
                    for line in cf:
                        tm = this_mov_pat.search(line)
                        if tm and this_reg is None:
                            this_reg = tm.group(1)
                        mm = mov_pat.search(line)
                        if mm:
                            reg = mm.group(1)
                            addr = int(mm.group(2), 16)
                            if rdata_min <= addr < rdata_max:
                                per_reg_vtables[reg] = addr
                                any_vtable = addr
                    chosen_vtable = per_reg_vtables.get(this_reg) if this_reg else None
                    if chosen_vtable is None:
                        chosen_vtable = any_vtable
                    if chosen_vtable:
                        vtable_addr = chosen_vtable

        if vtable_addr:
            classes[cls_name] = {
                'vtable_addr': vtable_addr,
                'parent': parent_name,
                'header': class_header.get(cls_name, '(unknown)'),
                'constructor': ctor_addr,
            }

    for item in VTABLE_MANUAL_CLASSES:
        name = item['name']
        if name not in classes:
            classes[name] = {
                'vtable_addr': parse_int(item['vtable_addr']),
                'parent': item.get('parent'),
                'header': item.get('header', '(unknown)'),
                'constructor': parse_int(item.get('constructor')),
            }

    for item in VTABLE_DUPLICATE_CLASS_DROPS:
        drop = item.get('drop')
        keep = item.get('keep')
        if drop in classes and keep in classes:
            if not item.get('when_same_vtable') or classes[drop]['vtable_addr'] == classes[keep]['vtable_addr']:
                del classes[drop]

    for class_name, override in VTABLE_CLASS_OVERRIDES.items():
        if class_name in classes:
            if 'vtable_addr' in override:
                classes[class_name]['vtable_addr'] = parse_int(override['vtable_addr'])
            if 'parent' in override:
                classes[class_name]['parent'] = override['parent']
            if 'header' in override:
                classes[class_name]['header'] = override['header']

    return classes


def find_constructor_parent_warnings(classes, code_full_dir, rdata_min, rdata_max, filter_class=None):
    """
    Check source-declared non-root parents against original constructor evidence.

    This catches cases where vtable slots look valid but the source hierarchy
    would force an intermediate base constructor/destructor that the original
    constructor never used.
    """
    call_pat = re.compile(r'\bCALL\s+0x([0-9a-fA-F]+)', re.IGNORECASE)
    mov_pat = re.compile(r'\bMOV\s+dword ptr \[[^\]]+\],0x([0-9a-fA-F]+)', re.IGNORECASE)
    instruction_pat = re.compile(r'^\s*(MOV|CALL|PUSH|POP|SUB|ADD|LEA|XOR|RET|JMP|CMP|TEST)\b', re.IGNORECASE | re.MULTILINE)
    warnings = []
    stats = {
        'checked': 0,
        'missing_constructor': 0,
        'missing_parent_evidence': 0,
        'missing_disassembly': 0,
        'empty_disassembly': 0,
    }

    for cls_name, info in sorted(classes.items()):
        if filter_class and cls_name != filter_class:
            continue

        parent_name = info.get('parent')
        ctor_addr = info.get('constructor')
        if not parent_name:
            continue
        if not ctor_addr:
            stats['missing_constructor'] += 1
            continue

        parent_info = classes.get(parent_name)
        if not parent_info:
            continue

        parent_ctor = parent_info.get('constructor')
        parent_vtable = parent_info.get('vtable_addr')
        if not parent_ctor and not parent_vtable:
            stats['missing_parent_evidence'] += 1
            continue

        ctor_file = os.path.join(code_full_dir, f"FUN_{ctor_addr:X}.disassembled.txt")
        if not os.path.exists(ctor_file):
            stats['missing_disassembly'] += 1
            continue

        with open(ctor_file, 'r') as fh:
            text = fh.read()

        if not instruction_pat.search(text):
            stats['empty_disassembly'] += 1
            continue

        stats['checked'] += 1
        calls = {int(match.group(1), 16) for match in call_pat.finditer(text)}
        vtable_writes = {
            int(match.group(1), 16)
            for match in mov_pat.finditer(text)
            if rdata_min <= int(match.group(1), 16) < rdata_max
        }

        if parent_ctor and parent_ctor in calls:
            continue
        if parent_vtable and parent_vtable in vtable_writes:
            continue

        warnings.append({
            'class_name': cls_name,
            'parent_name': parent_name,
            'constructor': ctor_addr,
            'parent_constructor': parent_ctor,
            'parent_vtable': parent_vtable,
        })

    return warnings, stats


def find_implemented_functions(src_dir):
    """
    Scan .cpp and .h files for /* Function start: 0xXXXXXX */ comments.
    Returns dict: address -> list of (file, line_number)
    """
    implementations = {}

    for pattern in ['*.cpp', '*.h']:
        for srcfile in sorted(glob.glob(os.path.join(src_dir, pattern))):
            basename = os.path.basename(srcfile)
            for marker in parse_source_function_comments(srcfile, include_no_assembly=True):
                addr = int(marker.address, 16)
                if addr not in implementations:
                    implementations[addr] = []
                implementations[addr].append((basename, marker.line))

    return implementations


# ── Main ────────────────────────────────────────────────────────────────────

def read_vtable_entries(f, sections, vtable_addr, code_start, code_end, max_addr):
    """Read vtable entries, stopping at non-code or next vtable boundary."""
    entries = []
    addr = vtable_addr
    while addr < max_addr:
        val = read_dword(f, sections, addr)
        if val is None or not (code_start <= val < code_end):
            break
        entries.append(val)
        addr += 4
    return entries


def main():
    ap = argparse.ArgumentParser(description='Verify vtables against source')
    add_config_argument(ap)
    ap.add_argument('--exe', help='Original executable to read vtables from')
    ap.add_argument('--src-dir', help='Source directory')
    ap.add_argument('--code-dir', help='Ghidra export/disassembly directory')
    ap.add_argument('--rdata-min', type=lambda value: int(value, 0), help='Fallback .rdata start address')
    ap.add_argument('--rdata-max', type=lambda value: int(value, 0), help='Fallback .rdata end address')
    ap.add_argument('--dump', action='store_true', help='Show full vtable dump')
    ap.add_argument('--class', dest='filter_class', help='Filter to specific class')
    args = ap.parse_args()

    try:
        config = load_config(args.config)
        configure_vtable_checker(config)
        paths = mode_paths(config, demo=False)
        vtables_config = config.get('vtables', {})
        exe_path = config_or_arg(args.exe, paths, 'exe', 'paths.full.exe')
        src_dir = config_or_arg(args.src_dir, paths, 'src_dir', 'paths.full.src_dir')
        code_dir = config_or_arg(args.code_dir, paths, 'code_dir', 'paths.full.code_dir')
        configured_rdata_min, configured_rdata_max = parse_range(vtables_config.get('rdata_range'), 'vtables.rdata_range')
        fallback_rdata_min = args.rdata_min if args.rdata_min is not None else configured_rdata_min
        fallback_rdata_max = args.rdata_max if args.rdata_max is not None else configured_rdata_max
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(2)

    if not os.path.exists(exe_path):
        print(f"Error: {exe_path} not found")
        sys.exit(1)

    _, sections = parse_pe(exe_path)
    text = sections['.text']
    rdata = sections.get('.rdata')
    code_start, code_end = text['va_start'], text['va_end']
    rdata_min = rdata['va_start'] if rdata else fallback_rdata_min
    rdata_max = rdata['va_end'] if rdata else fallback_rdata_max

    # Collect all vtable boundary addresses
    disasm_addrs = find_vtable_addrs_from_disasm(code_dir, rdata_min, rdata_max)
    hdr_addrs = find_vtable_addrs_from_headers(src_dir, rdata_min, rdata_max)
    all_vtable_addrs = sorted(disasm_addrs | hdr_addrs)

    # Parse classes and implementations
    classes = parse_class_info(src_dir, code_dir, rdata_min, rdata_max)
    invalid_parents = sorted(
        (cls_name, info['parent'])
        for cls_name, info in classes.items()
        if info.get('parent') and info['parent'] not in classes
    )
    constructor_parent_warnings, constructor_parent_stats = find_constructor_parent_warnings(
        classes, code_dir, rdata_min, rdata_max, args.filter_class
    )
    implementations = find_implemented_functions(src_dir)
    function_symbols = find_source_function_symbols(src_dir)
    _, _, class_methods, _ = parse_header_classes(src_dir)
    expected_slots = {}
    for cls_name in classes:
        build_expected_vtable_slots(cls_name, classes, class_methods, expected_slots)

    print(f"Binary:  {exe_path}")
    print(f"Code:    0x{code_start:08X}..0x{code_end:08X}")
    print(f"Rdata:   0x{rdata_min:08X}..0x{rdata_max:08X}")
    print(f"Vtables: {len(all_vtable_addrs)} unique addresses")
    print(f"Classes: {len(classes)} with vtable info")
    print(f"Parents: {len(invalid_parents)} invalid references")
    print(f"Ctor hierarchy: {constructor_parent_stats['checked']} checked, {len(constructor_parent_warnings)} warnings")
    skipped_ctor_checks = sum(
        count for key, count in constructor_parent_stats.items()
        if key != 'checked'
    )
    if skipped_ctor_checks:
        print(f"Ctor hierarchy skipped: {skipped_ctor_checks} without constructor evidence")
    print(f"Impls:   {len(implementations)} Function start comments")
    print()

    if invalid_parents:
        print("Invalid parent references:")
        for cls_name, parent in invalid_parents:
            print(f"  {cls_name} -> {parent}")
        print()

    if constructor_parent_warnings:
        print("Constructor hierarchy warnings:")
        for item in constructor_parent_warnings:
            parent_vtable = item['parent_vtable']
            parent_vtable_str = f"0x{parent_vtable:08X}" if parent_vtable else "(none)"
            print(
                f"  {item['class_name']} declares parent {item['parent_name']}, "
                f"but ctor 0x{item['constructor']:08X} neither calls parent ctor "
                f"0x{item['parent_constructor']:08X} nor writes parent vtable {parent_vtable_str}"
            )
        print()

    # Read vtables from binary
    vtables = {}
    func_types = {}  # cache: addr -> stub/ret0/real/...

    with open(exe_path, 'rb') as f:
        for cls_name, info in classes.items():
            addr = info['vtable_addr']
            idx = all_vtable_addrs.index(addr) if addr in all_vtable_addrs else -1
            max_addr = all_vtable_addrs[idx + 1] if (idx >= 0 and idx + 1 < len(all_vtable_addrs)) else addr + 0x100
            vtables[cls_name] = read_vtable_entries(f, sections, addr, code_start, code_end, max_addr)

        # Classify all functions referenced in vtables
        all_funcs = set()
        for entries in vtables.values():
            all_funcs.update(entries)
        for func_addr in all_funcs:
            func_types[func_addr] = classify_function(f, sections, func_addr)

    # Resolve parent vtable entries up the chain
    def get_parent_entries(cls_name, depth=0):
        if depth > 10:
            return []
        info = classes.get(cls_name)
        if not info or not info['parent']:
            return []
        parent = info['parent']
        if parent in vtables:
            return vtables[parent]
        return get_parent_entries(parent, depth + 1)

    # ── Verify ──
    total_slots = 0
    total_inherited = 0
    total_overrides = 0
    total_implemented = 0
    total_sdtors = 0
    total_stubs = 0
    total_missing_real = 0
    total_symbol_mismatch = 0
    missing_real = []
    missing_stubs = []

    sorted_classes = sorted(
        [c for c in vtables if not args.filter_class or c == args.filter_class],
        key=lambda c: classes[c]['vtable_addr']
    )

    if not args.dump:
        print("=" * 124)
        print(f"{'Class':<22} {'Vtable':<14} {'Parent':<18} {'#':<4} {'Inh':<4} {'Ovr':<4} {'OK':<4} {'Stub':<5} {'Miss':<5} {'Bad':<4} {'Status'}")
        print("=" * 124)

    for cls_name in sorted_classes:
        info = classes[cls_name]
        entries = vtables[cls_name]
        parent_entries = get_parent_entries(cls_name)

        # Determine slot names based on hierarchy
        slot_names = {}
        parent_chain = []
        cur = cls_name
        while cur and cur in classes:
            parent_chain.append(cur)
            cur = classes[cur].get('parent')
        for item in VTABLE_SLOT_NAME_SETS:
            if item['class_or_ancestor'] in parent_chain:
                slot_names = item['slots']
                break

        inherited = 0
        overrides = []
        for i, func_addr in enumerate(entries):
            if i < len(parent_entries) and parent_entries[i] == func_addr:
                inherited += 1
            else:
                overrides.append((i, func_addr))

        # Classify each override
        n_impl = 0
        n_sdtor = 0
        n_stub_missing = 0
        n_real_missing = 0
        n_symbol_mismatch = 0
        cls_missing_real = []
        cls_missing_stubs = []
        cls_symbol_mismatch = []
        slot_expectations = expected_slots.get(cls_name, [])

        def expected_for_slot(slot_idx):
            if slot_idx < len(slot_expectations):
                return slot_expectations[slot_idx]
            return None

        def slot_label(slot_idx):
            expected = expected_for_slot(slot_idx)
            if expected is not None:
                return expected['method_name']
            return slot_names.get(slot_idx, '')

        def is_sdtor_slot(slot_idx):
            expected = expected_for_slot(slot_idx)
            if expected is not None and expected['signature_key'] == '__dtor__':
                return True
            return slot_idx == 3

        def format_symbol_list(symbols):
            if not symbols:
                return "(unresolved)"
            return ', '.join(f"{sym['class_name']}::{sym['method_name']}" for sym in symbols)

        def matches_expected_symbol(slot_idx, expected, symbols):
            if expected is None:
                return True
            aliases = SLOT_SYMBOL_ALIASES.get((cls_name, slot_idx), set())
            for sym in symbols:
                candidate = (sym['class_name'], sym['method_name'])
                if candidate == (expected['class_name'], expected['method_name']):
                    return True
                if candidate in aliases:
                    return True
            return False

        for slot_idx, func_addr in overrides:
            is_impl = func_addr in implementations
            ftype = func_types.get(func_addr, 'unknown')
            expected = expected_for_slot(slot_idx)
            symbols = function_symbols.get(func_addr, [])

            if is_impl:
                n_impl += 1
                if expected is not None and expected['signature_key'] != '__dtor__' and not is_trivial_function_type(ftype):
                    matches_expected = matches_expected_symbol(slot_idx, expected, symbols)
                    if not matches_expected:
                        n_symbol_mismatch += 1
                        cls_symbol_mismatch.append((slot_idx, func_addr, expected, symbols))
            elif is_sdtor_slot(slot_idx):
                n_sdtor += 1  # sdtor, skip
            elif is_trivial_function_type(ftype):
                n_stub_missing += 1
                cls_missing_stubs.append((slot_idx, func_addr, ftype))
            else:
                n_real_missing += 1
                cls_missing_real.append((slot_idx, func_addr, ftype))

        total_slots += len(entries)
        total_inherited += inherited
        total_overrides += len(overrides)
        total_implemented += n_impl
        total_sdtors += n_sdtor
        total_stubs += n_stub_missing
        total_missing_real += n_real_missing
        total_symbol_mismatch += n_symbol_mismatch

        if args.dump:
            parent_str = info['parent'] or '(root)'
            print(f"\n{cls_name} (0x{info['vtable_addr']:08X}, parent={parent_str}, {info['header']}):")
            for i, func_addr in enumerate(entries):
                is_inherited = (i < len(parent_entries) and parent_entries[i] == func_addr)
                is_impl = func_addr in implementations
                ftype = func_types.get(func_addr, '?')
                slot_name = slot_label(i)
                expected = expected_for_slot(i)
                symbols = function_symbols.get(func_addr, [])

                marker = " " if is_inherited else "*"
                if is_inherited:
                    status = f"inherited ({parent_str})"
                elif is_impl:
                    locs = implementations[func_addr]
                    matches_expected = True
                    if expected is not None and expected['signature_key'] != '__dtor__' and not is_trivial_function_type(ftype):
                        matches_expected = matches_expected_symbol(i, expected, symbols)
                    if matches_expected:
                        status = f"OK  <- {', '.join(f'{fl}:{ln}' for fl, ln in locs)}"
                    else:
                        status = (
                            f"\033[31mMISMATCH\033[0m expected {expected['display_name']} "
                            f"got {format_symbol_list(symbols)}"
                        )
                elif is_sdtor_slot(i):
                    status = "sdtor (compiler-generated)"
                elif is_trivial_function_type(ftype):
                    tag = {
                        'stub': 'RET',
                        'ret0': 'return 0',
                        'ret1': 'return 1',
                        'retconst': 'return constant',
                        'fieldget': 'simple field getter',
                        'thunk': 'jump thunk',
                    }.get(ftype, ftype)
                    status = f"\033[33mMISSING ({tag})\033[0m"
                elif ftype == 'error':
                    status = f"\033[33mMISSING (ShowError call)\033[0m"
                else:
                    status = f"\033[31mMISSING\033[0m"

                name_str = f" {slot_name}" if slot_name else ""
                print(f"  {marker} [{i:2d}]{name_str:<18} 0x{func_addr:08X}  {status}")
        else:
            if n_real_missing > 0:
                status_str = f"\033[31m{n_real_missing} MISSING\033[0m"
                if n_symbol_mismatch > 0:
                    status_str += f" +{n_symbol_mismatch} bad slots"
                if n_stub_missing > 0:
                    status_str += f" +{n_stub_missing} stubs"
            elif n_symbol_mismatch > 0:
                status_str = f"\033[31m{n_symbol_mismatch} bad slots\033[0m"
                if n_stub_missing > 0:
                    status_str += f" +{n_stub_missing} stubs"
            elif n_stub_missing > 0:
                status_str = f"\033[33m{n_stub_missing} stubs only\033[0m"
            else:
                status_str = "\033[32mOK\033[0m"

            parent_str = info['parent'] or '(root)'
            print(f"{cls_name:<22} 0x{info['vtable_addr']:08X}   {parent_str:<18} "
                  f"{len(entries):<4} {inherited:<4} {len(overrides):<4} {n_impl:<4} "
                  f"{n_stub_missing:<5} {n_real_missing:<5} {n_symbol_mismatch:<4} {status_str}")

            for slot_idx, func_addr, ftype in cls_missing_real:
                slot_name = slot_label(slot_idx)
                disasm = os.path.exists(f"{code_dir}/FUN_{func_addr:X}.disassembled.txt")
                note = " (disasm)" if disasm else ""
                name_str = f" {slot_name}" if slot_name else ""
                missing_real.append((cls_name, slot_idx, func_addr, info['header'], slot_name))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  [{ftype}]{note}")

            for slot_idx, func_addr, ftype in cls_missing_stubs:
                slot_name = slot_label(slot_idx)
                tag = {
                    'stub': 'RET',
                    'ret0': 'return 0',
                    'ret1': 'return 1',
                    'retconst': 'return constant',
                    'fieldget': 'simple field getter',
                    'thunk': 'jump thunk',
                }.get(ftype, ftype)
                name_str = f" {slot_name}" if slot_name else ""
                missing_stubs.append((cls_name, slot_idx, func_addr, info['header'], slot_name, tag))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  \033[2m({tag})\033[0m")

            for slot_idx, func_addr, expected, symbols in cls_symbol_mismatch:
                print(
                    f"    [{slot_idx:2d}] {slot_label(slot_idx):<15} 0x{func_addr:08X}  "
                    f"expected {expected['display_name']} got {format_symbol_list(symbols)}"
                )

    if not args.dump:
        print("=" * 124)

    print()
    print(f"{'Total vtable slots:':<30} {total_slots}")
    print(f"{'  Inherited:':<30} {total_inherited}")
    print(f"{'  Overrides:':<30} {total_overrides}")
    print(f"{'    Implemented:':<30} {total_implemented}")
    print(f"{'    Sdtors (compiler):':<30} {total_sdtors}")
    print(f"{'    Trivial helpers/thunks:':<30} \033[33m{total_stubs}\033[0m")
    print(f"{'    Missing (real code):':<30} \033[31m{total_missing_real}\033[0m")
    print(f"{'    Implemented but wrong slot:':<30} \033[31m{total_symbol_mismatch}\033[0m")

    # Unmatched vtable addresses
    known_addrs = set(info['vtable_addr'] for info in classes.values())
    unmatched = sorted(set(all_vtable_addrs) - known_addrs)
    if unmatched:
        print(f"\nVtable addresses not matched to any class ({len(unmatched)}):")
        for addr in unmatched:
            print(f"  0x{addr:08X}")

    if invalid_parents or constructor_parent_warnings or total_missing_real or total_symbol_mismatch:
        sys.exit(1)


if __name__ == '__main__':
    main()
