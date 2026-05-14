#!/usr/bin/env python3

import argparse
import ast
import os
import re
import struct
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple


DEFAULT_EXE = "data/full/TEACHER.ORI.EXE"
DEFAULT_GLOBALS_SOURCE = "src/globals.cpp"
DEFAULT_GLOBALS_H = "src/globals.h"
DEFAULT_CODE_GLOBALS_H = "code-full/globals.h"

EXPECTED_RUNTIME_SEEDED_GLOBALS = {}

RUNTIME_SEEDED_GLOBALS = set(EXPECTED_RUNTIME_SEEDED_GLOBALS)


BUILTIN_TYPE_SIZES = {
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
    "ArthurStateHandler": 4,
    "ArthurElapsedTimer": 8,
    "ArthurPackedArchiveSlot": 268,
    "ArthurShapeSetSlot": 24,
    "ArthurRandomResourceTable": 6,
    "ArthurOptionVariantResourceTable": 18,
    "ArthurMainScreenStateEntry": 6,
    "ArthurDeferredMessage": 16,
    "ArthurSoundChannelRuntime": 22,
    "ArthurSoundChannelSlot": 34,
    "struct ArthurWaveFormatLayout": 18,
    "ArthurWaveFormatLayout": 18,
    "struct ArthurDisplayRectBlock": 12,
    "ArthurDisplayRectBlock": 12,
}


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


def line_for_offset(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def read_statement_after(text: str, start: int) -> Tuple[str, int, int]:
    i = start
    while i < len(text) and text[i].isspace():
        i += 1
    begin = i
    brace_depth = 0
    in_string = False
    in_char = False
    escape = False
    while i < len(text):
        ch = text[i]
        if escape:
            escape = False
        elif ch == "\\" and (in_string or in_char):
            escape = True
        elif ch == '"' and not in_char:
            in_string = not in_string
        elif ch == "'" and not in_string:
            in_char = not in_char
        elif not in_string and not in_char:
            if ch == "{":
                brace_depth += 1
            elif ch == "}":
                brace_depth = max(0, brace_depth - 1)
            elif ch == ";" and brace_depth == 0:
                return text[begin:i + 1], begin, i + 1
        i += 1
    return text[begin:], begin, len(text)


def read_trailing_comments(text: str, start: int) -> Tuple[str, int]:
    i = start
    comments: List[str] = []
    while i < len(text):
        while i < len(text) and text[i] in " \t\r":
            i += 1
        if text.startswith("//", i):
            end = text.find("\n", i)
            if end == -1:
                end = len(text)
            comments.append(text[i:end])
            return "\n".join(comments), end
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            if end == -1:
                end = len(text) - 2
            comments.append(text[i:end + 2])
            i = end + 2
            continue
        break
    return "\n".join(comments), i


def skip_leading_trivia(text: str, start: int) -> Tuple[int, str]:
    i = start
    trivia: List[str] = []
    while i < len(text):
        while i < len(text) and text[i].isspace():
            i += 1
        if text.startswith("//", i):
            end = text.find("\n", i)
            if end == -1:
                end = len(text)
            trivia.append(text[i:end])
            i = end
            continue
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            if end == -1:
                end = len(text) - 2
            trivia.append(text[i:end + 2])
            i = end + 2
            continue
        if i < len(text) and text[i] == "#":
            end = text.find("\n", i)
            if end == -1:
                end = len(text)
            i = end
            continue
        break
    return i, "\n".join(trivia)


def iter_statements(text: str) -> Iterable[Tuple[str, int, int, str, str]]:
    cursor = 0
    while cursor < len(text):
        cursor, leading = skip_leading_trivia(text, cursor)
        if cursor >= len(text):
            break
        statement, stmt_start, stmt_end = read_statement_after(text, cursor)
        if not statement.strip():
            break
        trailing, cursor = read_trailing_comments(text, stmt_end)
        yield statement, stmt_start, stmt_end, leading, trailing


def split_initializer(statement: str) -> Tuple[str, Optional[str]]:
    brace_depth = 0
    paren_depth = 0
    in_string = False
    in_char = False
    escape = False
    for i, ch in enumerate(statement):
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
        elif ch == "=" and brace_depth == 0 and paren_depth == 0:
            return statement[:i].strip(), statement[i + 1:].strip().rstrip(";").strip()
    return statement.strip().rstrip(";").strip(), None


def parse_name_and_type(declaration: str) -> Optional[Tuple[str, str, List[str]]]:
    declaration = strip_comments(declaration).strip().rstrip(";").strip()
    declaration = re.sub(r"\bextern\b", "", declaration).strip()
    declaration = re.sub(r"\s+", " ", declaration)
    fp = re.search(r"\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)", declaration)
    if fp:
        name = fp.group(1)
        type_text = declaration[:fp.start()].strip() + " *"
        return name, type_text.strip(), []

    m = re.search(r"([A-Za-z_][A-Za-z0-9_]*)(\s*(?:\[[^\]]*\]\s*)*)$", declaration)
    if not m:
        return None
    name = m.group(1)
    dims_text = m.group(2) or ""
    type_text = declaration[:m.start(1)].strip()
    dims = re.findall(r"\[([^\]]*)\]", dims_text)
    return name, type_text, dims


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


def is_non_variable_statement(statement: str) -> bool:
    stripped = strip_comments(statement).strip()
    if not stripped:
        return True
    return bool(re.match(r"^(typedef|struct|class|enum|using|namespace|template)\b", stripped))


def parse_globals_file(path: str,
                       require_extern: bool,
                       address_warnings: Optional[List[AddressWarning]] = None) -> List[GlobalDecl]:
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        text = f.read()
    decls: List[GlobalDecl] = []
    for statement, stmt_start, _stmt_end, leading, trailing in iter_statements(text):
        if is_non_variable_statement(statement):
            continue
        before, initializer = split_initializer(statement)
        if require_extern and "extern" not in before:
            continue
        if not require_extern and re.search(r"\bextern\b", before) and initializer is None:
            continue
        parsed = parse_name_and_type(before)
        if parsed is None:
            continue
        name, type_text, dims = parsed
        address = address_for_declaration(name, leading, statement, trailing)
        if address is None:
            if address_warnings is not None:
                context = "\n".join([leading, statement, trailing])
                if not has_no_address_annotation(context):
                    address_warnings.append(AddressWarning(
                        path=path,
                        line=line_for_offset(text, stmt_start),
                        name=name,
                        statement=statement.strip(),
                    ))
            continue
        decls.append(GlobalDecl(
            address=address,
            name=name,
            statement=statement.strip(),
            line=line_for_offset(text, stmt_start),
            type_text=type_text,
            dims=dims,
            has_initializer=initializer is not None,
            initializer=initializer,
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
            with open(path, "r", encoding="utf-8", errors="replace") as f:
                text = f.read()
            for match in re.finditer(r"/\*\s*Function start:\s*0x([0-9A-Fa-f]+)\s*\*/", text):
                address = int(match.group(1), 16)
                chunk = text[match.end():match.end() + 800]
                chunk = "\n".join(
                    line for line in chunk.splitlines()
                    if not line.lstrip().startswith("#")
                )
                chunk = strip_comments(chunk)
                fn = re.search(r"([A-Za-z_][A-Za-z0-9_:~]*)\s*\([^;{}]*\)\s*\{", chunk, re.S)
                if fn:
                    symbols[fn.group(1)] = address
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


def base_type_size(type_text: str) -> Optional[int]:
    normalized = normalize_type(type_text)
    if "*" in normalized:
        return 4
    if normalized in BUILTIN_TYPE_SIZES:
        return BUILTIN_TYPE_SIZES[normalized]
    if normalized.startswith("struct "):
        return BUILTIN_TYPE_SIZES.get(normalized)
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


def struct_bytes(decl: GlobalDecl, constants: Dict[str, int]) -> Tuple[Optional[bytes], str]:
    if decl.initializer is None:
        return None, ""
    typ = normalize_type(decl.type_text)
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
    if typ in ("struct ArthurDisplayRectBlock", "ArthurDisplayRectBlock") and len(values) >= 4:
        return struct.pack("<IIhh", values[0] & 0xFFFFFFFF, values[1] & 0xFFFFFFFF,
                           values[2] & 0xFFFF, values[3] & 0xFFFF), ""
    if typ in ("struct ArthurWaveFormatLayout", "ArthurWaveFormatLayout") and len(values) >= 7:
        return struct.pack("<HHIIHHH", values[0] & 0xFFFF, values[1] & 0xFFFF,
                           values[2] & 0xFFFFFFFF, values[3] & 0xFFFFFFFF,
                           values[4] & 0xFFFF, values[5] & 0xFFFF, values[6] & 0xFFFF), ""
    return None, "unsupported struct initializer"


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
    if typ not in ("ArthurRandomResourceTable",
                   "ArthurOptionVariantResourceTable",
                   "ArthurMainScreenStateEntry"):
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
        try:
            if typ == "ArthurRandomResourceTable":
                if len(fields) != 2:
                    return None, "unexpected ArthurRandomResourceTable field count"
                count = eval_int_expr(strip_outer_casts(fields[0]), constants)
                ptr = resolve_symbolic_pointer(fields[1], constants, data_symbols, function_symbols)
                if ptr is None:
                    return None, "unresolved ArthurRandomResourceTable pointer"
                out.extend(struct.pack("<HI", count & 0xFFFF, ptr & 0xFFFFFFFF))
            elif typ == "ArthurOptionVariantResourceTable":
                if len(fields) != 6:
                    return None, "unexpected ArthurOptionVariantResourceTable field count"
                values = []
                for i in range(0, 6, 2):
                    count = eval_int_expr(strip_outer_casts(fields[i]), constants)
                    ptr = resolve_symbolic_pointer(fields[i + 1], constants, data_symbols, function_symbols)
                    if ptr is None:
                        return None, "unresolved ArthurOptionVariantResourceTable pointer"
                    values.extend([count & 0xFFFF, ptr & 0xFFFFFFFF])
                out.extend(struct.pack("<HIHIHI", values[0], values[1],
                                       values[2], values[3],
                                       values[4], values[5]))
            elif typ == "ArthurMainScreenStateEntry":
                if len(fields) != 2:
                    return None, "unexpected ArthurMainScreenStateEntry field count"
                available = eval_int_expr(strip_outer_casts(fields[0]), constants)
                ptr = resolve_symbolic_pointer(fields[1], constants, data_symbols, function_symbols)
                if ptr is None:
                    return None, "unresolved ArthurMainScreenStateEntry handler"
                out.extend(struct.pack("<HI", available & 0xFFFF, ptr & 0xFFFFFFFF))
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
        if decl.address in EXPECTED_RUNTIME_SEEDED_GLOBALS:
            expected = struct.pack("<I", EXPECTED_RUNTIME_SEEDED_GLOBALS[decl.address])
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
            if decl.address in RUNTIME_SEEDED_GLOBALS:
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


def print_report(issues: List[Issue], address_warnings: List[AddressWarning], total_defs: int, args) -> None:
    print("Global initialization/layout audit")
    print(f"  original exe: {args.exe}")
    print(f"  source:       {args.globals_source}")
    print(f"  definitions:  {total_defs}")
    print(f"  issues:       {len(issues)}")
    print(f"  warnings:     {len(address_warnings)}")
    print()

    if not issues and not address_warnings:
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


def parse_int_auto(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser(description="Audit globals source initializers against original PE bytes.")
    parser.add_argument("--exe", default=DEFAULT_EXE)
    parser.add_argument("--globals-source", "--globals-c", dest="globals_source", default=DEFAULT_GLOBALS_SOURCE)
    parser.add_argument("--globals-h", default=DEFAULT_GLOBALS_H)
    parser.add_argument("--code-globals-h", default=DEFAULT_CODE_GLOBALS_H)
    parser.add_argument("--min-address", type=parse_int_auto, default=0x00468000,
                        help="ignore lower addresses, useful for skipping import tables")
    parser.add_argument("--max-issues", type=int, default=200,
                        help="maximum issues to print; 0 prints all")
    parser.add_argument("--include-code-globals", action="store_true",
                        help="also report nonzero code/globals.h entries not covered by globals source")
    parser.add_argument("--include-symbolic", action="store_true",
                        help="report nonzero globals whose source initializer contains symbols/pointers")
    parser.add_argument("--no-source-order", action="store_true",
                        help="disable source-order decrease warnings for implicit-zero globals")
    parser.add_argument("--source-order-all", action="store_true",
                        help="also warn on source-order decreases for initialized globals")
    parser.add_argument("--fail-on-issues", action="store_true",
                        help="exit 1 when suspicious issues are found")
    parser.add_argument("--fail-on-warnings", action="store_true",
                        help="exit 1 when globals without address annotations are found")
    args = parser.parse_args()

    constants = parse_defines([args.globals_h])
    pe = PEImage(args.exe)
    address_warnings: List[AddressWarning] = []
    decls = parse_globals_source(args.globals_source, address_warnings)
    header_decls = parse_globals_header(args.globals_h, address_warnings)
    code_globals = parse_code_globals(args.code_globals_h)
    function_symbols = parse_function_symbols(os.path.dirname(args.globals_source) or ".")
    issues = build_issues(pe, decls, header_decls, code_globals, function_symbols,
                          constants,
                          args.min_address, args.include_code_globals, args.include_symbolic,
                          not args.no_source_order, args.source_order_all)
    print_report(issues, address_warnings, len(decls), args)
    if args.fail_on_issues and issues:
        return 1
    if args.fail_on_warnings and address_warnings:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
