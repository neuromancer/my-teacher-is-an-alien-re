#!/usr/bin/env python3

import argparse
import ast
import os
import re
import struct
import sys
from bisect import bisect_right
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Sequence, Set, Tuple

try:
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
except ImportError:
    Cs = None
    CS_ARCH_X86 = CS_MODE_32 = None
    X86_OP_IMM = X86_OP_MEM = X86_OP_REG = None

from cppSourceParser import CPP_PARSER, node_text, parse_source_functions, sanitize_source, walk
from projectConfig import (
    ConfigError,
    add_config_argument,
    config_or_arg,
    get_section,
    load_config,
    mode_paths,
    parse_int,
    parse_int_value_map,
)


BASE_TYPE_SIZES = {
    "char": 1,
    "signed char": 1,
    "unsigned char": 1,
    "BYTE": 1,
    "short": 2,
    "signed short": 2,
    "unsigned short": 2,
    "WORD": 2,
    "int": 4,
    "signed int": 4,
    "unsigned int": 4,
    "long": 4,
    "unsigned long": 4,
    "DWORD": 4,
    "COLORREF": 4,
    "INT": 4,
    "LONG": 4,
    "UINT": 4,
    "WPARAM": 4,
    "LPARAM": 4,
    "BOOL": 4,
    "HANDLE": 4,
    "HGLOBAL": 4,
    "HWND": 4,
    "HCURSOR": 4,
    "HPALETTE": 4,
    "HINSTANCE": 4,
    "HACCEL": 4,
    "HMENU": 4,
    "SIZE_T": 4,
    "float": 4,
    "double": 8,
}

TYPE_SIZES = dict(BASE_TYPE_SIZES)
STRUCT_FORMATS = {}
SYMBOLIC_STRUCT_ARRAYS = {}


@dataclass
class Section:
    name: str
    va_start: int
    va_end: int
    rawptr: int
    rawsize: int


@dataclass
class GlobalDecl:
    address: int
    name: str
    statement: str
    line: int
    type_text: str
    dims: List[str]
    has_initializer: bool
    initializer: Optional[str]
    size: Optional[int] = None
    source_bytes: Optional[bytes] = None
    source_note: str = ""


@dataclass
class CodeGlobal:
    address: int
    name: str
    size: int
    line: int
    kind: str


@dataclass
class Issue:
    category: str
    address: int
    name: str
    line: Optional[int]
    size: int
    original: bytes
    source: Optional[bytes]
    detail: str


@dataclass
class AddressWarning:
    path: str
    line: int
    name: str
    statement: str


@dataclass
class AutoEntry:
    address: int
    notes: Set[str]


@dataclass
class AutoGlobalRange:
    start: int
    end: int
    name: str
    line: int


@dataclass
class AutoGlobalFact:
    category: str
    instr_address: int
    address: int
    text: str
    symbol: str


class PEImage:
    def __init__(self, filename: str):
        self.filename = filename
        with open(filename, "rb") as f:
            self.data = f.read()
        self.image_base, self.sections = self._parse()

    def _u16(self, offset: int) -> int:
        return struct.unpack_from("<H", self.data, offset)[0]

    def _u32(self, offset: int) -> int:
        return struct.unpack_from("<I", self.data, offset)[0]

    def _parse(self) -> Tuple[int, List[Section]]:
        if self.data[:2] != b"MZ":
            raise ValueError(f"{self.filename}: not an MZ executable")
        pe_off = self._u32(0x3C)
        if self.data[pe_off:pe_off + 4] != b"PE\0\0":
            raise ValueError(f"{self.filename}: not a PE executable")
        num_sections = self._u16(pe_off + 6)
        opt_size = self._u16(pe_off + 20)
        image_base = self._u32(pe_off + 52)
        section_start = pe_off + 24 + opt_size
        sections: List[Section] = []
        for i in range(num_sections):
            off = section_start + i * 40
            name = self.data[off:off + 8].rstrip(b"\0").decode("ascii", errors="replace")
            vsize = self._u32(off + 8)
            va = self._u32(off + 12)
            rawsize = self._u32(off + 16)
            rawptr = self._u32(off + 20)
            span = max(vsize, rawsize)
            sections.append(Section(name, image_base + va, image_base + va + span, rawptr, rawsize))
        return image_base, sections

    def section_for_va(self, va: int) -> Optional[Section]:
        for section in self.sections:
            if section.va_start <= va < section.va_end:
                return section
        return None

    def read(self, va: int, size: int) -> Optional[bytes]:
        out = bytearray()
        cursor = va
        remaining = size
        while remaining > 0:
            section = self.section_for_va(cursor)
            if section is None:
                return None
            chunk = min(remaining, section.va_end - cursor)
            section_off = cursor - section.va_start
            raw_available = max(0, section.rawsize - section_off)
            raw_chunk = min(chunk, raw_available)
            if raw_chunk > 0:
                raw_off = section.rawptr + section_off
                out.extend(self.data[raw_off:raw_off + raw_chunk])
            if raw_chunk < chunk:
                out.extend(b"\0" * (chunk - raw_chunk))
            cursor += chunk
            remaining -= chunk
        return bytes(out)


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//.*", "", text)
    return text


def address_from_encoded_suffix(name: str) -> Optional[int]:
    m = re.search(r"_([0-9A-Fa-f]{8}|[0-9A-Fa-f]{6})$", name)
    if not m:
        return None
    return int(m.group(1), 16)


def address_from_comments(text: str) -> Optional[int]:
    comments = re.findall(r"/\*.*?\*/|//[^\n]*", text, flags=re.S)
    if not comments:
        return None
    joined = "\n".join(comments)
    for pattern in (
        r"\b(?:PTR_)?DAT_([0-9A-Fa-f]{8})\b",
        r"\b0x([0-9A-Fa-f]{8}|[0-9A-Fa-f]{6})\b",
        r"_([0-9A-Fa-f]{8}|[0-9A-Fa-f]{6})\b",
    ):
        m = re.search(pattern, joined)
        if m:
            return int(m.group(1), 16)
    return None


def address_for_declaration(name: str, leading: str, statement: str, trailing: str) -> Optional[int]:
    address = address_from_encoded_suffix(name)
    if address is not None:
        return address
    return address_from_comments("\n".join([leading, statement, trailing]))


def has_no_address_annotation(text: str) -> bool:
    return bool(re.search(r"\b(no-address|no original address|not address-mapped|addressless)\b",
                          text, flags=re.I))


GLOBAL_DECLARATION_EXCLUDED_ANCESTORS = {
    "function_definition",
    "compound_statement",
    "field_declaration_list",
    "class_specifier",
    "struct_specifier",
    "enum_specifier",
}


def is_global_declaration_node(node) -> bool:
    parent = node.parent
    while parent is not None:
        if parent.type in GLOBAL_DECLARATION_EXCLUDED_ANCESTORS:
            return False
        parent = parent.parent
    return True


def declaration_has_storage(source: bytes, node, storage: str) -> bool:
    for child in node.children:
        if child.type == "storage_class_specifier" and node_text(source, child).strip() == storage:
            return True
    return False


def declaration_value_and_declarator(node):
    declarator = node.child_by_field_name("declarator")
    if declarator is None:
        return None, None
    if declarator.type == "init_declarator":
        return declarator.child_by_field_name("value"), declarator.child_by_field_name("declarator")
    return None, declarator


def declarator_identifier_node(node):
    if node is None:
        return None
    if node.type == "identifier":
        return node

    declarator = node.child_by_field_name("declarator")
    if declarator is not None:
        found = declarator_identifier_node(declarator)
        if found is not None:
            return found

    for child in node.children:
        if child.type in ("parameter_list", "argument_list"):
            continue
        found = declarator_identifier_node(child)
        if found is not None:
            return found
    return None


def declarator_pointer_depth(node) -> int:
    if node is None:
        return 0
    depth = 1 if node.type == "pointer_declarator" else 0
    for child in node.children:
        if child.type in ("parameter_list", "argument_list"):
            continue
        depth += declarator_pointer_depth(child)
    return depth


def declarator_array_dims(source: bytes, node) -> List[str]:
    if node is None:
        return []
    if node.type == "array_declarator":
        dims = declarator_array_dims(source, node.child_by_field_name("declarator"))
        size = node.child_by_field_name("size")
        dims.append(node_text(source, size).strip() if size is not None else "")
        return dims

    declarator = node.child_by_field_name("declarator")
    if declarator is not None:
        return declarator_array_dims(source, declarator)
    return []


def declaration_base_type_text(source: bytes, node) -> str:
    parts: List[str] = []
    for i, child in enumerate(node.children):
        if node.field_name_for_child(i) == "declarator":
            break
        if child.type == ";":
            break
        text = node_text(source, child).strip()
        if text and text != "extern":
            parts.append(text)
    return " ".join(parts)


def declaration_type_text(source: bytes, node, declarator) -> str:
    base_type = declaration_base_type_text(source, node)
    pointer_depth = declarator_pointer_depth(declarator)
    if pointer_depth == 0:
        return base_type
    if declarator.type == "function_declarator":
        return f"{base_type} {'*' * pointer_depth}"
    return f"{base_type}{'*' * pointer_depth}"


def is_plain_function_declaration(declarator) -> bool:
    return (
        declarator is not None
        and declarator.type == "function_declarator"
        and declarator_pointer_depth(declarator) == 0
    )


def line_start_before(source: bytes, index: int) -> int:
    return source.rfind(b"\n", 0, index) + 1


def line_end_after(source: bytes, index: int) -> int:
    end = source.find(b"\n", index)
    return len(source) if end < 0 else end


def leading_comments_before(source: bytes, start: int) -> str:
    comments: List[str] = []
    cursor = start

    while True:
        while cursor > 0 and source[cursor - 1] in b" \t\r\n":
            cursor -= 1
        if cursor <= 0:
            break

        line_start = line_start_before(source, cursor - 1)
        line = source[line_start:cursor]
        if line.lstrip().startswith(b"#"):
            cursor = line_start
            continue

        stripped = line.rstrip()
        line_comment = stripped.rfind(b"//")
        if line_comment >= 0:
            comment_start = line_start + line_comment
            if source[line_start:comment_start].strip():
                break
            comments.append(source[comment_start:cursor].decode("utf-8", errors="replace"))
            cursor = line_start
            continue

        if cursor >= 2 and source[cursor - 2:cursor] == b"*/":
            comment_start = source.rfind(b"/*", 0, cursor - 2)
            if comment_start < 0:
                break
            if source[line_start_before(source, comment_start):comment_start].strip():
                break
            comments.append(source[comment_start:cursor].decode("utf-8", errors="replace"))
            cursor = comment_start
            continue

        break

    comments.reverse()
    return "\n".join(comments)


def trailing_comments_after(source: bytes, end: int) -> str:
    comments: List[str] = []
    cursor = end

    while cursor < len(source):
        while cursor < len(source) and source[cursor] in b" \t\r":
            cursor += 1
        if source.startswith(b"//", cursor):
            comment_end = line_end_after(source, cursor)
            comments.append(source[cursor:comment_end].decode("utf-8", errors="replace"))
            return "\n".join(comments)
        if source.startswith(b"/*", cursor):
            comment_end = source.find(b"*/", cursor + 2)
            if comment_end < 0:
                comment_end = len(source) - 2
            comments.append(source[cursor:comment_end + 2].decode("utf-8", errors="replace"))
            cursor = comment_end + 2
            continue
        break
    return "\n".join(comments)


def parse_globals_file(path: str,
                       require_extern: bool,
                       address_warnings: Optional[List[AddressWarning]] = None) -> List[GlobalDecl]:
    with open(path, "rb") as f:
        source = f.read()
    tree = CPP_PARSER.parse(sanitize_source(source))
    decls: List[GlobalDecl] = []
    for node in walk(tree.root_node):
        if node.type != "declaration" or not is_global_declaration_node(node):
            continue

        initializer_node, declarator = declaration_value_and_declarator(node)
        if declarator is None or is_plain_function_declaration(declarator):
            continue

        is_extern = declaration_has_storage(source, node, "extern")
        if require_extern and not is_extern:
            continue
        if not require_extern and is_extern and initializer_node is None:
            continue

        name_node = declarator_identifier_node(declarator)
        if name_node is None:
            continue

        name = node_text(source, name_node).strip()
        statement = node_text(source, node).strip()
        leading = leading_comments_before(source, node.start_byte)
        trailing = trailing_comments_after(source, node.end_byte)
        address = address_for_declaration(name, leading, statement, trailing)
        if address is None:
            if address_warnings is not None:
                context = "\n".join([leading, statement, trailing])
                if not has_no_address_annotation(context):
                    address_warnings.append(AddressWarning(
                        path=path,
                        line=node.start_point.row + 1,
                        name=name,
                        statement=statement,
                    ))
            continue

        decls.append(GlobalDecl(
            address=address,
            name=name,
            statement=statement,
            line=node.start_point.row + 1,
            type_text=declaration_type_text(source, node, declarator),
            dims=declarator_array_dims(source, declarator),
            has_initializer=initializer_node is not None,
            initializer=node_text(source, initializer_node).strip() if initializer_node is not None else None,
        ))
    return decls


def parse_globals_source(path: str,
                         address_warnings: Optional[List[AddressWarning]] = None) -> List[GlobalDecl]:
    return parse_globals_file(path, require_extern=False, address_warnings=address_warnings)


def parse_globals_header(path: str,
                         address_warnings: Optional[List[AddressWarning]] = None) -> List[GlobalDecl]:
    return parse_globals_file(path, require_extern=True, address_warnings=address_warnings)


def parse_code_globals(path: str) -> List[CodeGlobal]:
    if not path or not os.path.exists(path):
        return []
    out: List[CodeGlobal] = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        lines = f.readlines()
    for lineno, line in enumerate(lines, 1):
        m = re.search(r"//\s*([A-Za-z_][A-Za-z0-9_]*)\s+at\s+0x([0-9A-Fa-f]+)\s+\(string,\s+([0-9]+)\s+bytes\)", line)
        if m:
            out.append(CodeGlobal(int(m.group(2), 16), m.group(1), int(m.group(3)), lineno, "string"))
            continue
        m = re.search(r"\b(undefined|byte|word|dword|pointer)([1248]?)\s+([A-Za-z_][A-Za-z0-9_:]*)\s*=", line)
        if m:
            kind = m.group(1) + m.group(2)
            name = m.group(3)
            addr_m = re.search(r"_([0-9A-Fa-f]{8}|[0-9A-Fa-f]{6})\b", name)
            if not addr_m:
                continue
            size = {"undefined1": 1, "undefined2": 2, "undefined4": 4, "undefined8": 8,
                    "byte": 1, "word": 2, "dword": 4, "pointer": 4}.get(kind, 4)
            out.append(CodeGlobal(int(addr_m.group(1), 16), name, size, lineno, kind))
    return out


def parse_function_symbols(src_dir: str) -> Dict[str, int]:
    symbols: Dict[str, int] = {}
    if not src_dir or not os.path.isdir(src_dir):
        return symbols
    for root, _, files in os.walk(src_dir):
        for filename in files:
            if not filename.endswith((".c", ".C", ".cpp")):
                continue
            path = os.path.join(root, filename)
            for function in parse_source_functions(path):
                symbols[function.name] = int(function.address, 16)
    return symbols


def parse_defines(paths: Iterable[str]) -> Dict[str, int]:
    constants: Dict[str, int] = {
        "MAX_PATH": 260,
        "NULL": 0,
    }
    define_re = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+([0-9A-Fa-fxXuUlL()+\-*/ ]+)\s*$")
    for path in paths:
        if not os.path.exists(path):
            continue
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                m = define_re.match(line)
                if not m:
                    continue
                try:
                    constants[m.group(1)] = eval_int_expr(m.group(2), constants)
                except Exception:
                    pass
    return constants


def eval_int_expr(expr: str, constants: Dict[str, int]) -> int:
    expr = expr.strip()
    expr = re.sub(r"\b(0x[0-9A-Fa-f]+)[uUlL]+\b", r"\1", expr)
    expr = re.sub(r"\b([0-9]+)[uUlL]+\b", r"\1", expr)
    for name, value in constants.items():
        expr = re.sub(rf"\b{re.escape(name)}\b", str(value), expr)
    node = ast.parse(expr, mode="eval")

    def visit(n):
        if isinstance(n, ast.Expression):
            return visit(n.body)
        if isinstance(n, ast.Constant) and isinstance(n.value, int):
            return n.value
        if isinstance(n, ast.UnaryOp) and isinstance(n.op, (ast.USub, ast.UAdd)):
            val = visit(n.operand)
            return -val if isinstance(n.op, ast.USub) else val
        if isinstance(n, ast.BinOp) and isinstance(n.op, (ast.Add, ast.Sub, ast.Mult, ast.FloorDiv, ast.Div, ast.LShift, ast.RShift, ast.BitOr, ast.BitAnd)):
            left = visit(n.left)
            right = visit(n.right)
            if isinstance(n.op, ast.Add):
                return left + right
            if isinstance(n.op, ast.Sub):
                return left - right
            if isinstance(n.op, ast.Mult):
                return left * right
            if isinstance(n.op, (ast.FloorDiv, ast.Div)):
                return left // right
            if isinstance(n.op, ast.LShift):
                return left << right
            if isinstance(n.op, ast.RShift):
                return left >> right
            if isinstance(n.op, ast.BitOr):
                return left | right
            if isinstance(n.op, ast.BitAnd):
                return left & right
        raise ValueError(expr)

    return int(visit(node))


def normalize_type(type_text: str) -> str:
    type_text = re.sub(r"\b(const|volatile|extern|static)\b", "", type_text)
    type_text = re.sub(r"\s+", " ", type_text).strip()
    type_text = type_text.replace(" *", "*").replace("* ", "*")
    return type_text


def configure_globals(globals_config: Dict) -> Dict[int, int]:
    global TYPE_SIZES, STRUCT_FORMATS, SYMBOLIC_STRUCT_ARRAYS

    TYPE_SIZES = dict(BASE_TYPE_SIZES)
    for name, size in globals_config.get("type_sizes", {}).items():
        TYPE_SIZES[name] = parse_int(size)

    STRUCT_FORMATS = {}
    for name, spec in globals_config.get("struct_formats", {}).items():
        aliases = spec.get("aliases") or [name]
        for alias in aliases:
            STRUCT_FORMATS[normalize_type(alias)] = spec["format"]

    SYMBOLIC_STRUCT_ARRAYS = {}
    for name, spec in globals_config.get("symbolic_struct_arrays", {}).items():
        aliases = spec.get("aliases") or [name]
        for alias in aliases:
            SYMBOLIC_STRUCT_ARRAYS[normalize_type(alias)] = spec.get("fields", [])

    return parse_int_value_map(globals_config.get("runtime_seeded_globals", {}))


def base_type_size(type_text: str) -> Optional[int]:
    normalized = normalize_type(type_text)
    if "*" in normalized:
        return 4
    if normalized in TYPE_SIZES:
        return TYPE_SIZES[normalized]
    if normalized.startswith("struct "):
        return TYPE_SIZES.get(normalized)
    return None


def infer_size(decl: GlobalDecl, constants: Dict[str, int], next_addr: Optional[int]) -> Optional[int]:
    elem = base_type_size(decl.type_text)
    if elem is None:
        return next_addr - decl.address if next_addr and next_addr > decl.address else None
    count = 1
    for dim in decl.dims:
        if dim.strip() == "":
            if decl.initializer and decl.initializer.strip().startswith('"'):
                try:
                    count *= len(parse_c_string_bytes(decl.initializer.strip())) + 1
                except Exception:
                    return next_addr - decl.address if next_addr and next_addr > decl.address else None
            else:
                return next_addr - decl.address if next_addr and next_addr > decl.address else None
        else:
            try:
                count *= eval_int_expr(dim, constants)
            except Exception:
                return next_addr - decl.address if next_addr and next_addr > decl.address else None
    return elem * count


def parse_c_string_bytes(literal: str) -> bytes:
    literal = literal.strip()
    parts = re.findall(r'"(?:\\.|[^"\\])*"', literal)
    if not parts:
        raise ValueError(literal)
    out = bytearray()
    for part in parts:
        value = ast.literal_eval("b" + part)
        out.extend(value)
    return bytes(out)


def parse_c_char_value(token: str) -> int:
    value = ast.literal_eval("b" + token.strip())
    if len(value) != 1:
        raise ValueError(token)
    return value[0]


def strip_outer_casts(expr: str) -> str:
    expr = expr.strip()
    while True:
        m = re.match(r"^\([A-Za-z_][A-Za-z0-9_\s\*]*\)\s*(.+)$", expr)
        if not m:
            return expr
        expr = m.group(1).strip()


def scalar_bytes(type_text: str, initializer: str, constants: Dict[str, int]) -> Optional[bytes]:
    init = strip_outer_casts(initializer.rstrip(";"))
    normalized = normalize_type(type_text)
    if init.startswith("'"):
        val = parse_c_char_value(init)
    elif normalized == "float":
        try:
            return struct.pack("<f", float(re.sub(r"[fF]\s*$", "", init)))
        except Exception:
            return None
    elif normalized == "double":
        try:
            return struct.pack("<d", float(init))
        except Exception:
            return None
    else:
        numeric_check = re.sub(r"\b(0x[0-9A-Fa-f]+)[uUlL]+\b", r"\1", init)
        numeric_check = re.sub(r"\b([0-9]+)[uUlL]+\b", r"\1", numeric_check)
        if re.search(r"[A-Za-z_]", re.sub(r"0x[0-9A-Fa-f]+", "", numeric_check)):
            return None
        try:
            val = eval_int_expr(init, constants)
        except Exception:
            return None
    size = base_type_size(type_text)
    if size == 1:
        return struct.pack("<B", val & 0xFF)
    if size == 2:
        return struct.pack("<H", val & 0xFFFF)
    if size == 4:
        return struct.pack("<I", val & 0xFFFFFFFF)
    return None


def split_top_level_commas(text: str) -> List[str]:
    out: List[str] = []
    start = 0
    brace_depth = 0
    paren_depth = 0
    in_string = False
    in_char = False
    escape = False
    for i, ch in enumerate(text):
        if escape:
            escape = False
            continue
        if ch == "\\" and (in_string or in_char):
            escape = True
            continue
        if ch == '"' and not in_char:
            in_string = not in_string
            continue
        if ch == "'" and not in_string:
            in_char = not in_char
            continue
        if in_string or in_char:
            continue
        if ch == "{":
            brace_depth += 1
        elif ch == "}":
            brace_depth = max(0, brace_depth - 1)
        elif ch == "(":
            paren_depth += 1
        elif ch == ")":
            paren_depth = max(0, paren_depth - 1)
        elif ch == "," and brace_depth == 0 and paren_depth == 0:
            out.append(text[start:i].strip())
            start = i + 1
    tail = text[start:].strip()
    if tail:
        out.append(tail)
    return out


def numeric_array_bytes(decl: GlobalDecl, constants: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None:
        return None, ""
    init = strip_comments(decl.initializer).strip()
    if init.startswith('"') and base_type_size(decl.type_text) == 1:
        data = parse_c_string_bytes(init) + b"\0"
        return data, ""
    if not (init.startswith("{") and init.endswith("}")):
        return None, ""
    if "{" in init[1:-1] or "&" in init:
        return None, "nested/symbolic initializer"
    body = init[1:-1]
    elem_size = base_type_size(decl.type_text)
    if elem_size not in (1, 2, 4):
        return None, "unsupported element type"
    out = bytearray()
    for token in split_top_level_commas(body):
        if not token:
            continue
        token = strip_outer_casts(token)
        try:
            if token.startswith("'"):
                value = parse_c_char_value(token)
            else:
                if re.search(r"[A-Za-z_]", re.sub(r"0x[0-9A-Fa-f]+", "", token)):
                    return None, "symbolic initializer"
                value = eval_int_expr(token, constants)
        except Exception:
            return None, "unparsed initializer"
        if elem_size == 1:
            out.extend(struct.pack("<B", value & 0xFF))
        elif elem_size == 2:
            out.extend(struct.pack("<H", value & 0xFFFF))
        else:
            out.extend(struct.pack("<I", value & 0xFFFFFFFF))
    return bytes(out), ""


def pack_struct_format(fmt: str, values: List[int]) -> Optional[bytes]:
    try:
        return struct.pack(fmt, *values)
    except struct.error:
        return None


def struct_bytes(decl: GlobalDecl, constants: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None:
        return None, ""
    typ = normalize_type(decl.type_text)
    fmt = STRUCT_FORMATS.get(typ)
    if fmt is None:
        return None, "unsupported struct initializer"
    init = strip_comments(decl.initializer).strip()
    if not (init.startswith("{") and init.endswith("}")):
        return None, ""
    body = init[1:-1]
    if "{" in body or "&" in body:
        return None, "nested/symbolic initializer"
    fields = split_top_level_commas(body)
    try:
        values = [eval_int_expr(strip_outer_casts(x), constants) for x in fields]
    except Exception:
        return None, "unparsed struct initializer"
    packed = pack_struct_format(fmt, values)
    if packed is None:
        return None, "unparsed struct initializer"
    return packed, ""


def resolve_symbolic_pointer(expr: str,
                             constants: Dict[str, int],
                             data_symbols: Dict[str, GlobalDecl],
                             function_symbols: Dict[str, int]) -> Optional[int]:
    expr = strip_outer_casts(expr).strip()
    if expr in ("0", "NULL"):
        return 0
    if expr in function_symbols:
        return function_symbols[expr]
    if expr in data_symbols:
        return data_symbols[expr].address
    if expr.startswith("&"):
        target = expr[1:].strip()
        m = re.match(r"^([A-Za-z_][A-Za-z0-9_]*)(?:\s*\[\s*(.*?)\s*\])?$", target)
        if not m:
            return None
        name = m.group(1)
        decl = data_symbols.get(name)
        if decl is None:
            return None
        index = 0
        if m.group(2) is not None:
            try:
                index = eval_int_expr(m.group(2), constants)
            except Exception:
                return None
        elem_size = base_type_size(decl.type_text)
        if elem_size is None:
            return None
        return decl.address + index * elem_size
    numeric = re.sub(r"\b(0x[0-9A-Fa-f]+)[uUlL]+\b", r"\1", expr)
    numeric = re.sub(r"\b([0-9]+)[uUlL]+\b", r"\1", numeric)
    if re.search(r"[A-Za-z_]", re.sub(r"0x[0-9A-Fa-f]+", "", numeric)):
        return None
    try:
        return eval_int_expr(expr, constants)
    except Exception:
        return None


def symbolic_struct_array_bytes(decl: GlobalDecl,
                                constants: Dict[str, int],
                                data_symbols: Dict[str, GlobalDecl],
                                function_symbols: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None:
        return None, ""
    typ = normalize_type(decl.type_text)
    field_specs = SYMBOLIC_STRUCT_ARRAYS.get(typ)
    if field_specs is None:
        return None, ""
    init = strip_comments(decl.initializer).strip()
    if not (init.startswith("{") and init.endswith("}")):
        return None, ""
    rows = split_top_level_commas(init[1:-1])
    out = bytearray()
    for row in rows:
        row = row.strip()
        if not (row.startswith("{") and row.endswith("}")):
            return None, "unparsed symbolic struct initializer"
        fields = split_top_level_commas(row[1:-1])
        if len(fields) != len(field_specs):
            return None, f"unexpected {typ} field count"
        try:
            for field, spec in zip(fields, field_specs):
                kind = spec.get("kind")
                if kind == "uint16":
                    value = eval_int_expr(strip_outer_casts(field), constants)
                    out.extend(struct.pack("<H", value & 0xFFFF))
                elif kind == "uint32":
                    value = eval_int_expr(strip_outer_casts(field), constants)
                    out.extend(struct.pack("<I", value & 0xFFFFFFFF))
                elif kind == "pointer":
                    ptr = resolve_symbolic_pointer(field, constants, data_symbols, function_symbols)
                    if ptr is None:
                        return None, f"unresolved {typ} pointer"
                    out.extend(struct.pack("<I", ptr & 0xFFFFFFFF))
                else:
                    return None, f"unsupported {typ} field kind: {kind}"
        except Exception:
            return None, "unparsed symbolic struct initializer"
    return bytes(out), ""


def symbolic_pointer_bytes(decl: GlobalDecl,
                           constants: Dict[str, int],
                           data_symbols: Dict[str, GlobalDecl],
                           function_symbols: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None or decl.dims or "*" not in normalize_type(decl.type_text):
        return None, ""
    ptr = resolve_symbolic_pointer(decl.initializer, constants, data_symbols, function_symbols)
    if ptr is None:
        return None, "unresolved symbolic pointer"
    return struct.pack("<I", ptr & 0xFFFFFFFF), ""


def symbolic_pointer_array_bytes(decl: GlobalDecl,
                                 constants: Dict[str, int],
                                 data_symbols: Dict[str, GlobalDecl],
                                 function_symbols: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None or not decl.dims or "*" not in normalize_type(decl.type_text):
        return None, ""
    init = strip_comments(decl.initializer).strip()
    if not (init.startswith("{") and init.endswith("}")):
        return None, ""
    body = init[1:-1]
    if "{" in body:
        return None, "nested symbolic initializer"
    out = bytearray()
    for token in split_top_level_commas(body):
        if not token:
            continue
        ptr = resolve_symbolic_pointer(token, constants, data_symbols, function_symbols)
        if ptr is None:
            return None, "unresolved symbolic pointer"
        out.extend(struct.pack("<I", ptr & 0xFFFFFFFF))
    return bytes(out), ""


def infer_source_bytes(decl: GlobalDecl,
                       constants: Dict[str, int],
                       data_symbols: Dict[str, GlobalDecl],
                       function_symbols: Dict[str, int]) -> None:
    if decl.size is None:
        return
    if not decl.has_initializer:
        decl.source_bytes = b"\0" * decl.size
        decl.source_note = "implicit zero"
        return
    if decl.initializer is None:
        return
    if not decl.dims:
        data = scalar_bytes(decl.type_text, decl.initializer, constants)
        if data is not None:
            decl.source_bytes = data.ljust(decl.size, b"\0")[:decl.size]
            return
        data, note = symbolic_pointer_bytes(decl, constants, data_symbols, function_symbols)
        if data is not None:
            decl.source_bytes = data.ljust(decl.size, b"\0")[:decl.size]
            return
    data, note = struct_bytes(decl, constants)
    if data is None:
        data, note = symbolic_struct_array_bytes(decl, constants, data_symbols, function_symbols)
    if data is None:
        data, note = symbolic_pointer_array_bytes(decl, constants, data_symbols, function_symbols)
    if data is None:
        data, note = numeric_array_bytes(decl, constants)
    if data is not None:
        decl.source_bytes = data.ljust(decl.size, b"\0")[:decl.size]
    else:
        decl.source_note = note or "unsupported initializer"


def hexdump_short(data: bytes, limit: int = 16) -> str:
    shown = data[:limit]
    suffix = "" if len(data) <= limit else " ..."
    return " ".join(f"{b:02x}" for b in shown) + suffix


def ascii_hint(data: bytes) -> str:
    out = []
    for b in data[:32]:
        if 32 <= b < 127:
            out.append(chr(b))
        elif b == 0:
            out.append(".")
        else:
            out.append("?")
    return "".join(out)


def ranges_for(decls: List[GlobalDecl]) -> List[Tuple[int, int, GlobalDecl]]:
    ranges = []
    for decl in decls:
        if decl.size and decl.size > 0:
            ranges.append((decl.address, decl.address + decl.size, decl))
    return ranges


def in_any_range(address: int, ranges: List[Tuple[int, int, GlobalDecl]]) -> bool:
    return any(start <= address < end for start, end, _ in ranges)


AUTO_WRITE_MNEMONICS = {
    "adc",
    "add",
    "and",
    "dec",
    "inc",
    "mov",
    "neg",
    "not",
    "or",
    "sbb",
    "sub",
    "xchg",
    "xor",
}


def load_auto_complete(path: str) -> Dict[int, AutoEntry]:
    entries: Dict[int, AutoEntry] = {}
    active_note = ""
    pending_comments: List[str] = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for raw in f:
            line = raw.strip()
            if not line:
                pending_comments = []
                continue
            if line.startswith("#"):
                comment = line[1:].strip()
                if comment:
                    pending_comments.append(comment)
                continue
            try:
                address = int(line, 16)
            except ValueError:
                continue
            if pending_comments:
                active_note = " / ".join(pending_comments)
                pending_comments = []
            entry = entries.setdefault(address, AutoEntry(address, set()))
            if active_note:
                entry.notes.add(active_note)
    return entries


def parse_known_vector_helpers(config: Dict, mode: str) -> Set[int]:
    helpers: Set[int] = set()
    known_crt = get_section(get_section(config, "calls"), "known_crt").get(mode, {})
    if isinstance(known_crt, dict):
        for key, name in known_crt.items():
            if not isinstance(name, str):
                continue
            lowered = name.lower()
            if "vec" in lowered or "arrayunwind" in lowered:
                helpers.add(parse_int(key))
    return helpers


def reviewed_auto_complete_map(config: Dict, mode: str) -> Dict[int, str]:
    section = get_section(config, "auto_complete_global_effects")
    reviewed = get_section(section, "reviewed")
    mode_reviewed = reviewed.get(mode, {})
    if not isinstance(mode_reviewed, dict):
        return {}
    return {parse_int(key): str(value) for key, value in mode_reviewed.items()}


def section_ranges_by_name(pe: PEImage, names: Sequence[str]) -> List[Tuple[int, int]]:
    selected = {name.lower() for name in names}
    return [
        (section.va_start, section.va_end)
        for section in pe.sections
        if section.name.lower() in selected
    ]


def address_in_ranges(address: int, ranges: Sequence[Tuple[int, int]]) -> bool:
    return any(start <= address < end for start, end in ranges)


def text_section_bounds(pe: PEImage) -> Tuple[int, int]:
    for section in pe.sections:
        if section.name.lower() == ".text":
            return section.va_start, section.va_end
    raise ConfigError("original executable has no .text section")


def count_disassembly_instructions(path: str) -> int:
    """Use Ghidra text only as an extent hint; operands are decoded with Capstone."""
    count = 0
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for raw in f:
            line = raw.strip()
            if (
                not line
                or line.startswith("Function:")
                or line.startswith("Address:")
                or line.endswith(":")
                or line.startswith(";")
            ):
                continue
            count += 1
    return count


def capstone_function_hints(code_dir: str, code_start: int, code_end: int) -> Tuple[List[int], Dict[int, int]]:
    if not code_dir or not os.path.isdir(code_dir):
        return [], {}
    starts: Set[int] = set()
    instruction_counts: Dict[int, int] = {}
    for filename in os.listdir(code_dir):
        match = re.match(r"FUN_([0-9A-Fa-f]+)\.disassembled\.txt$", filename)
        if not match:
            continue
        address = int(match.group(1), 16)
        if code_start <= address < code_end:
            starts.add(address)
            instruction_counts[address] = count_disassembly_instructions(os.path.join(code_dir, filename))
    return sorted(starts), instruction_counts


def next_auto_function_boundary(starts: Sequence[int], start: int, code_end: int) -> int:
    index = bisect_right(starts, start)
    if index < len(starts):
        return starts[index]
    return code_end


def disassemble_auto_function(pe: PEImage,
                              start: int,
                              starts: Sequence[int],
                              instruction_counts: Dict[int, int],
                              code_end: int,
                              max_bytes: int) -> List:
    end = min(next_auto_function_boundary(starts, start, code_end), code_end, start + max_bytes)
    if end <= start:
        return []
    data = pe.read(start, end - start)
    if not data:
        return []
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    instructions = []
    expected_count = instruction_counts.get(start)
    for insn in md.disasm(data, start):
        instructions.append(insn)
        if expected_count is not None and len(instructions) >= expected_count:
            break
    return instructions


def instruction_text(insn) -> str:
    return f"{insn.mnemonic.upper()} {insn.op_str}".strip()


def operand_immediate(operand) -> Optional[int]:
    if operand.type != X86_OP_IMM:
        return None
    return operand.imm & 0xFFFFFFFF


def operand_absolute_memory(operand) -> Optional[int]:
    if operand.type != X86_OP_MEM:
        return None
    if operand.mem.base or operand.mem.index:
        return None
    return operand.mem.disp & 0xFFFFFFFF


def operand_register_name(insn, operand) -> str:
    if operand.type != X86_OP_REG:
        return ""
    return insn.reg_name(operand.reg).lower()


def next_branch_target(instructions: Sequence, index: int, max_lookahead: int = 8) -> Optional[int]:
    stop = min(len(instructions), index + max_lookahead + 1)
    for cursor in range(index + 1, stop):
        insn = instructions[cursor]
        mnemonic = insn.mnemonic.lower()
        if mnemonic in ("call", "jmp"):
            if insn.operands:
                return operand_immediate(insn.operands[0])
            return None
        if mnemonic.startswith("j") or mnemonic.startswith("ret"):
            return None
    return None


def auto_global_ranges(decls: Sequence[GlobalDecl]) -> List[AutoGlobalRange]:
    ranges = []
    for decl in decls:
        if decl.size and decl.size > 0:
            ranges.append(AutoGlobalRange(decl.address, decl.address + decl.size, decl.name, decl.line))
    return sorted(ranges, key=lambda item: item.start)


def symbol_for_auto_global(address: int,
                           global_starts: Sequence[int],
                           global_ranges: Sequence[AutoGlobalRange]) -> str:
    index = bisect_right(global_starts, address) - 1
    if index >= 0:
        item = global_ranges[index]
        if item.start <= address < item.end:
            offset = address - item.start
            return item.name if offset == 0 else f"{item.name}+0x{offset:x}"
    return f"0x{address:08x}"


def auto_complete_facts(instructions: Sequence,
                        data_ranges: Sequence[Tuple[int, int]],
                        global_ranges: Sequence[AutoGlobalRange],
                        vector_helpers: Set[int],
                        include_data_args: bool,
                        include_this_calls: bool) -> List[AutoGlobalFact]:
    global_starts = [item.start for item in global_ranges]
    facts: List[AutoGlobalFact] = []
    for index, insn in enumerate(instructions):
        mnemonic = insn.mnemonic.lower()
        if mnemonic in AUTO_WRITE_MNEMONICS and insn.operands:
            address = operand_absolute_memory(insn.operands[0])
            if address is not None and address_in_ranges(address, data_ranges):
                facts.append(AutoGlobalFact(
                    "DIRECT_WRITE",
                    insn.address,
                    address,
                    instruction_text(insn),
                    symbol_for_auto_global(address, global_starts, global_ranges),
                ))

        if mnemonic == "push" and insn.operands:
            address = operand_immediate(insn.operands[0])
            if address is not None and address_in_ranges(address, data_ranges):
                target = next_branch_target(instructions, index)
                if target in vector_helpers:
                    category = "VECTOR_HELPER_ARG"
                elif include_data_args:
                    category = "DATA_ARG"
                else:
                    category = ""
                if category:
                    facts.append(AutoGlobalFact(
                        category,
                        insn.address,
                        address,
                        instruction_text(insn),
                        symbol_for_auto_global(address, global_starts, global_ranges),
                    ))

        if include_this_calls and mnemonic == "mov" and len(insn.operands) >= 2:
            dst, src = insn.operands[0], insn.operands[1]
            address = operand_immediate(src)
            if (
                operand_register_name(insn, dst) == "ecx"
                and address is not None
                and address_in_ranges(address, data_ranges)
                and next_branch_target(instructions, index, max_lookahead=3) is not None
            ):
                facts.append(AutoGlobalFact(
                    "GLOBAL_THIS_CALL",
                    insn.address,
                    address,
                    instruction_text(insn),
                    symbol_for_auto_global(address, global_starts, global_ranges),
                ))
    return facts


def build_auto_complete_global_effects(pe: PEImage,
                                       decls: Sequence[GlobalDecl],
                                       config: Dict,
                                       mode: str,
                                       args) -> List[Tuple[AutoEntry, List[AutoGlobalFact]]]:
    if args.no_auto_complete_global_effects:
        return []
    if Cs is None:
        raise ConfigError("capstone is required for auto-complete global-effect auditing")
    if not args.auto_complete or not os.path.exists(args.auto_complete):
        raise ConfigError(f"auto_complete file not found: {args.auto_complete}")

    code_start, code_end = text_section_bounds(pe)
    data_ranges = section_ranges_by_name(pe, args.data_sections or [".data"])
    if not data_ranges:
        raise ConfigError(f"no matching writable data sections found in {args.exe}")

    entries = load_auto_complete(args.auto_complete)
    hint_starts, instruction_counts = capstone_function_hints(args.code_dir, code_start, code_end)
    starts = set(hint_starts)
    starts.update(address for address in entries if code_start <= address < code_end)
    sorted_starts = sorted(starts)
    global_ranges = auto_global_ranges(decls)
    vector_helpers = parse_known_vector_helpers(config, mode)

    results: List[Tuple[AutoEntry, List[AutoGlobalFact]]] = []
    for address in sorted(entries):
        if not (code_start <= address < code_end):
            continue
        instructions = disassemble_auto_function(
            pe,
            address,
            sorted_starts,
            instruction_counts,
            code_end,
            args.auto_complete_max_function_bytes,
        )
        facts = auto_complete_facts(
            instructions,
            data_ranges,
            global_ranges,
            vector_helpers,
            include_data_args=args.include_auto_complete_data_args,
            include_this_calls=not args.no_auto_complete_this_calls,
        )
        if facts:
            results.append((entries[address], facts))
    return results


def format_auto_notes(entry: AutoEntry) -> str:
    notes = sorted(note for note in entry.notes if note)
    return "; ".join(notes) if notes else "(no auto_complete note)"


def decl_matches_original(pe: PEImage, decl: GlobalDecl) -> bool:
    if decl.size is None or decl.size <= 0 or decl.source_bytes is None:
        return False
    original = pe.read(decl.address, decl.size)
    return original is not None and original[:decl.size] == decl.source_bytes[:decl.size]


def compatible_alias_overlap(pe: PEImage, decl: GlobalDecl, other: GlobalDecl) -> bool:
    if decl.size is None or other.size is None:
        return False
    overlap_start = max(decl.address, other.address)
    overlap_end = min(decl.address + decl.size, other.address + other.size)
    if overlap_start >= overlap_end:
        return False
    if decl.source_bytes is None or other.source_bytes is None:
        return False
    decl_off = overlap_start - decl.address
    other_off = overlap_start - other.address
    overlap_size = overlap_end - overlap_start
    if decl.source_bytes[decl_off:decl_off + overlap_size] != other.source_bytes[other_off:other_off + overlap_size]:
        return False
    return decl_matches_original(pe, decl) and decl_matches_original(pe, other)


def build_source_order_issues(decls: List[GlobalDecl],
                              min_address: int,
                              include_initialized: bool) -> List[Issue]:
    issues: List[Issue] = []
    previous: Optional[GlobalDecl] = None
    for decl in decls:
        if decl.address < min_address:
            continue
        if not include_initialized and decl.has_initializer:
            continue
        if previous is not None and decl.address < previous.address:
            issues.append(Issue("SOURCE_ORDER_DECREASE", decl.address, decl.name, decl.line,
                                decl.size or 0, b"", None,
                                f"declared after {previous.name} at 0x{previous.address:08x} "
                                f"line {previous.line}; source order should follow address order"))
        previous = decl
    return issues


def is_cpp_vtable_placeholder(name: str) -> bool:
    return "_vtable" in name.lower()


def build_issues(pe: PEImage,
                 decls: List[GlobalDecl],
                 header_decls: List[GlobalDecl],
                 code_globals: List[CodeGlobal],
                 function_symbols: Dict[str, int],
                 constants: Dict[str, int],
                 runtime_seeded_globals: Dict[int, int],
                 min_address: int,
                 include_code_globals: bool,
                 include_symbolic: bool,
                 check_source_order: bool,
                 source_order_all: bool) -> List[Issue]:
    decls = [decl for decl in decls if not is_cpp_vtable_placeholder(decl.name)]
    header_decls = [decl for decl in header_decls if not is_cpp_vtable_placeholder(decl.name)]

    by_addr = sorted(decls, key=lambda d: d.address)
    next_by_addr: Dict[int, Optional[int]] = {}
    for i, decl in enumerate(by_addr):
        next_addr = by_addr[i + 1].address if i + 1 < len(by_addr) else None
        next_by_addr[id(decl)] = next_addr

    data_symbols = {decl.name: decl for decl in decls}
    for decl in decls:
        decl.size = infer_size(decl, constants, next_by_addr[id(decl)])
        if decl.size and decl.size > 0x10000:
            decl.size = None
        infer_source_bytes(decl, constants, data_symbols, function_symbols)

    issues: List[Issue] = []
    if check_source_order:
        issues.extend(build_source_order_issues(decls, min_address, source_order_all))

    for i, decl in enumerate(by_addr):
        if decl.address < min_address or decl.size is None or decl.size <= 0:
            continue
        end = decl.address + decl.size
        for other in by_addr[i + 1:]:
            if other.address >= end:
                break
            if other.address < min_address:
                continue
            if compatible_alias_overlap(pe, decl, other):
                continue
            issues.append(Issue("SOURCE_RANGE_OVERLAP", decl.address, decl.name, decl.line,
                                decl.size, b"", None,
                                f"covers {other.name} at 0x{other.address:08x}; "
                                f"range ends at 0x{end:08x}"))

    for decl in decls:
        if decl.address < min_address or decl.size is None or decl.size <= 0:
            continue
        original = pe.read(decl.address, decl.size)
        if original is None:
            continue
        if decl.address in runtime_seeded_globals:
            expected = struct.pack("<I", runtime_seeded_globals[decl.address])
            if decl.size != len(expected):
                issues.append(Issue("RUNTIME_SEED_SIZE", decl.address, decl.name, decl.line,
                                    decl.size, original[:min(decl.size, 32)], decl.source_bytes,
                                    f"expected 4-byte runtime seed {hexdump_short(expected)}"))
            elif decl.source_bytes is None:
                issues.append(Issue("RUNTIME_SEED_UNPARSED", decl.address, decl.name, decl.line,
                                    decl.size, original[:decl.size], None,
                                    f"expected runtime seed {hexdump_short(expected)}"))
            elif decl.source_bytes[:decl.size] != expected:
                issues.append(Issue("RUNTIME_SEED_MISMATCH", decl.address, decl.name, decl.line,
                                    decl.size, original[:decl.size], decl.source_bytes[:decl.size],
                                    f"expected runtime seed {hexdump_short(expected)}"))
            continue
        if decl.source_bytes is None:
            if include_symbolic and any(original):
                issues.append(Issue("SYMBOLIC_OR_UNPARSED_INIT", decl.address, decl.name, decl.line,
                                    min(decl.size, 32), original[:min(decl.size, 32)], None,
                                    decl.source_note or "cannot compare source initializer"))
            continue
        if original[:decl.size] != decl.source_bytes[:decl.size]:
            if decl.address in runtime_seeded_globals:
                continue
            category = "NONZERO_NO_INIT" if not decl.has_initializer and any(original) else "INIT_MISMATCH"
            if category == "INIT_MISMATCH" or any(original):
                issues.append(Issue(category, decl.address, decl.name, decl.line, decl.size,
                                    original[:decl.size], decl.source_bytes[:decl.size],
                                    decl.source_note))

    defined_names = {decl.name for decl in decls}
    for decl in header_decls:
        if decl.name in defined_names or decl.address < min_address:
            continue
        size = infer_size(decl, constants, None) or 4
        if size > 0x1000:
            size = 32
        original = pe.read(decl.address, size)
        if original is not None and any(original):
            issues.append(Issue("HEADER_EXTERN_NO_GLOBALS_C_DEF", decl.address, decl.name, decl.line,
                                size, original, None, "extern in globals.h has no source definition"))

    source_ranges = ranges_for(decls)
    if include_code_globals:
        seen_code = set()
        for entry in code_globals:
            if entry.address < min_address or entry.address in seen_code:
                continue
            seen_code.add(entry.address)
            if in_any_range(entry.address, source_ranges):
                continue
            original = pe.read(entry.address, entry.size)
            if original is not None and any(original):
                issues.append(Issue("CODE_GLOBAL_NOT_IN_SRC", entry.address, entry.name, entry.line,
                                    entry.size, original, None,
                                    f"{entry.kind} from code/globals.h not covered by globals source"))
    return sorted(issues, key=lambda x: (x.address, x.category, x.name))


def print_report(issues: List[Issue],
                 address_warnings: List[AddressWarning],
                 total_defs: int,
                 auto_results: List[Tuple[AutoEntry, List[AutoGlobalFact]]],
                 auto_reviewed: Dict[int, str],
                 args) -> None:
    auto_reviewed_count = sum(1 for entry, _facts in auto_results if entry.address in auto_reviewed)
    auto_unreviewed = [
        (entry, facts) for entry, facts in auto_results
        if entry.address not in auto_reviewed
    ]

    print("Global initialization/layout audit")
    print(f"  original exe: {args.exe}")
    print(f"  source:       {args.globals_source}")
    print(f"  definitions:  {total_defs}")
    print(f"  issues:       {len(issues)}")
    print(f"  warnings:     {len(address_warnings)}")
    if not args.no_auto_complete_global_effects:
        print(
            "  auto-complete global side effects: "
            f"{len(auto_results)} ({auto_reviewed_count} reviewed, {len(auto_unreviewed)} unreviewed)"
        )
    print()

    if (
        not issues
        and not address_warnings
        and not auto_unreviewed
        and not args.show_auto_complete_reviewed
    ):
        print("No suspicious global initializer/layout issues found.")
        return

    if issues:
        max_issues = args.max_issues
        selected = issues if max_issues == 0 else issues[:max_issues]
        for issue in selected:
            line = f":{issue.line}" if issue.line else ""
            print(f"{issue.category:31} 0x{issue.address:08x} {issue.name}{line} size={issue.size}")
            if issue.original:
                print(f"  original: {hexdump_short(issue.original)}  {ascii_hint(issue.original)}")
            if issue.source is not None:
                print(f"  source:   {hexdump_short(issue.source)}  {ascii_hint(issue.source)}")
            if issue.detail:
                print(f"  note:     {issue.detail}")
        if max_issues and len(issues) > max_issues:
            print()
            print(f"... {len(issues) - max_issues} more issues omitted; rerun with --max-issues 0 for all.")

    if address_warnings:
        if issues:
            print()
        print("Warnings: globals without associated addresses")
        print("  Add an _004xxxxx suffix, a // 0x004xxxxx comment, or a no-address note.")
        for warning in address_warnings:
            first_line = warning.statement.splitlines()[0]
            if len(first_line) > 100:
                first_line = first_line[:97] + "..."
            print(f"  {warning.path}:{warning.line}: {warning.name}: {first_line}")

    auto_to_show = auto_results if args.show_auto_complete_reviewed else auto_unreviewed
    if auto_to_show:
        if issues or address_warnings:
            print()
        heading = "Auto-complete global side effects"
        if not args.show_auto_complete_reviewed:
            heading += " (unreviewed)"
        print(heading)
        for entry, facts in auto_to_show:
            status = "REVIEWED" if entry.address in auto_reviewed else "UNREVIEWED"
            print(f"{status:10} 0x{entry.address:08x}")
            print(f"  note: {format_auto_notes(entry)}")
            if entry.address in auto_reviewed:
                print(f"  review: {auto_reviewed[entry.address]}")
            shown = facts if args.max_auto_facts == 0 else facts[:args.max_auto_facts]
            for fact in shown:
                print(
                    f"  {fact.category:17} 0x{fact.instr_address:08x} "
                    f"{fact.symbol:<36} {fact.text}"
                )
            if args.max_auto_facts and len(facts) > args.max_auto_facts:
                print(f"  ... {len(facts) - args.max_auto_facts} more fact(s)")


def parse_int_auto(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser(description="Audit globals source initializers against original PE bytes.")
    add_config_argument(parser)
    parser.add_argument("--exe")
    parser.add_argument("--globals-source", "--globals-c", dest="globals_source")
    parser.add_argument("--globals-h")
    parser.add_argument("--code-globals-h")
    parser.add_argument("--code-dir",
                        help="Ghidra output directory used only for Capstone function-boundary hints")
    parser.add_argument("--auto-complete",
                        help="auto_complete.txt path for compiler/CRT global side-effect auditing")
    parser.add_argument("--data-section", action="append", dest="data_sections",
                        help="writable section to scan for auto-complete side effects; defaults to .data")
    parser.add_argument("--min-address", type=parse_int_auto,
                        help="ignore lower addresses, useful for skipping import tables")
    parser.add_argument("--max-issues", type=int, default=200,
                        help="maximum issues to print; 0 prints all")
    parser.add_argument("--max-auto-facts", type=int, default=12,
                        help="maximum auto-complete facts to print per function; 0 prints all")
    parser.add_argument("--auto-complete-max-function-bytes", type=int, default=4096,
                        help="maximum bytes to disassemble per auto-complete function")
    parser.add_argument("--include-code-globals", action="store_true",
                        help="also report nonzero code/globals.h entries not covered by globals source")
    parser.add_argument("--include-symbolic", action="store_true",
                        help="report nonzero globals whose source initializer contains symbols/pointers")
    parser.add_argument("--include-auto-complete-data-args", action="store_true",
                        help="also report generic PUSH data-address arguments in auto-complete functions")
    parser.add_argument("--no-auto-complete-this-calls", action="store_true",
                        help="do not report MOV ECX,global followed by CALL/JMP in auto-complete functions")
    parser.add_argument("--no-auto-complete-global-effects", action="store_true",
                        help="disable the auto-complete global side-effect audit")
    parser.add_argument("--show-auto-complete-reviewed", action="store_true",
                        help="print reviewed auto-complete global side-effect details as well as unreviewed ones")
    parser.add_argument("--no-source-order", action="store_true",
                        help="disable source-order decrease warnings for implicit-zero globals")
    parser.add_argument("--source-order-all", action="store_true",
                        help="also warn on source-order decreases for initialized globals")
    parser.add_argument("--fail-on-issues", action="store_true",
                        help="exit 1 when suspicious issues are found")
    parser.add_argument("--fail-on-warnings", action="store_true",
                        help="exit 1 when globals without address annotations are found")
    args = parser.parse_args()

    try:
        config = load_config(args.config)
        path_config = mode_paths(config, demo=False)
        globals_config = get_section(config, "globals")

        args.exe = config_or_arg(args.exe, path_config, "exe", "paths.full.exe")
        args.globals_source = config_or_arg(
            args.globals_source, path_config, "globals_source", "paths.full.globals_source"
        )
        args.globals_h = config_or_arg(args.globals_h, path_config, "globals_header", "paths.full.globals_header")
        args.code_globals_h = config_or_arg(
            args.code_globals_h, path_config, "code_globals_header", "paths.full.code_globals_header"
        )
        args.code_dir = args.code_dir or path_config.get("code_dir", "")
        args.auto_complete = (
            args.auto_complete
            or path_config.get("auto_complete")
            or os.path.join(path_config.get("src_dir", "src"), "auto_complete.txt")
        )
        if args.min_address is None:
            args.min_address = parse_int(config_or_arg(None, globals_config, "min_address", "globals.min_address"))
        runtime_seeded_globals = configure_globals(globals_config)
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    constants = parse_defines([args.globals_h])
    pe = PEImage(args.exe)
    address_warnings: List[AddressWarning] = []
    decls = parse_globals_source(args.globals_source, address_warnings)
    header_decls = parse_globals_header(args.globals_h, address_warnings)
    code_globals = parse_code_globals(args.code_globals_h)
    function_symbols = parse_function_symbols(os.path.dirname(args.globals_source) or ".")
    issues = build_issues(pe, decls, header_decls, code_globals, function_symbols,
                          constants,
                          runtime_seeded_globals,
                          args.min_address, args.include_code_globals, args.include_symbolic,
                          not args.no_source_order, args.source_order_all)
    try:
        auto_reviewed = reviewed_auto_complete_map(config, "full")
        auto_results = build_auto_complete_global_effects(pe, decls, config, "full", args)
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    print_report(issues, address_warnings, len(decls), auto_results, auto_reviewed, args)
    auto_unreviewed = any(entry.address not in auto_reviewed for entry, _facts in auto_results)
    if args.fail_on_issues and (issues or auto_unreviewed):
        return 1
    if args.fail_on_warnings and address_warnings:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
