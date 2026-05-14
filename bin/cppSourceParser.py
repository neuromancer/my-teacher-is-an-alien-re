#!/usr/bin/env python3

from dataclasses import dataclass
from bisect import bisect_right
from string import hexdigits

try:
    from tree_sitter import Language, Parser
    import tree_sitter_cpp
except ImportError as exc:
    raise SystemExit(
        "Missing tree-sitter dependencies. Run: python3 -m pip install -r requirements.txt"
    ) from exc


@dataclass(frozen=True)
class SourceFunction:
    address: str
    name: str
    line: int


@dataclass(frozen=True)
class SourceFunctionGroup:
    addresses: tuple
    name: str
    line: int


@dataclass(frozen=True)
class SourceFunctionMarker:
    address: str
    name: str
    line: int
    function_start: int


@dataclass(frozen=True)
class SourceCommentMarker:
    address: str
    line: int


def make_cpp_parser():
    parser = Parser()
    language = Language(tree_sitter_cpp.language())
    try:
        parser.language = language
    except AttributeError:
        parser.set_language(language)
    return parser


CPP_PARSER = make_cpp_parser()
CALLING_CONVENTIONS = (b"__cdecl", b"__fastcall", b"__stdcall", b"__thiscall")


def node_text(source, node):
    return source[node.start_byte:node.end_byte].decode("utf-8", errors="ignore")


def sanitize_source(source):
    sanitized = bytearray(source)
    for keyword in CALLING_CONVENTIONS:
        start = 0
        while True:
            index = source.find(keyword, start)
            if index < 0:
                break
            sanitized[index:index + len(keyword)] = b" " * len(keyword)
            start = index + len(keyword)
    return bytes(sanitized)


def walk(node):
    yield node
    for child in node.children:
        yield from walk(child)


def parse_function_start_comment(text, line_text, include_no_assembly=False):
    if not include_no_assembly and "No assembly extracted" in line_text:
        return None
    if "Function start:" not in text:
        return None

    start = text.find("0x")
    if start < 0:
        return None

    address = []
    for ch in text[start + 2:]:
        if ch in hexdigits:
            address.append(ch.upper())
        elif address:
            break

    if not address:
        return None
    return "".join(address)


def find_function_declarator(node):
    if node.type == "function_declarator":
        return node
    for child in node.children:
        found = find_function_declarator(child)
        if found is not None:
            return found
    return None


def function_name_from_definition(source, function_node):
    declarator = function_node.child_by_field_name("declarator")
    if declarator is None:
        declarator = function_node

    function_declarator = find_function_declarator(declarator)
    if function_declarator is None:
        return None

    name_node = function_declarator.child_by_field_name("declarator")
    if name_node is None:
        return None

    return node_text(source, name_node).strip()


def first_parameter_type_name(source, function_node):
    declarator = function_node.child_by_field_name("declarator")
    if declarator is None:
        declarator = function_node

    function_declarator = find_function_declarator(declarator)
    if function_declarator is None:
        return None

    parameter_list = function_declarator.child_by_field_name("parameters")
    if parameter_list is None:
        for child in function_declarator.children:
            if child.type == "parameter_list":
                parameter_list = child
                break
    if parameter_list is None:
        return None

    parameter = None
    for child in parameter_list.children:
        if child.type == "parameter_declaration":
            parameter = child
            break
    if parameter is None:
        return None

    for child in walk(parameter):
        if child.type in ("type_identifier", "qualified_identifier"):
            return node_text(source, child).strip()
    return None


def function_name_for_call_checker(source, function_node):
    name = function_name_from_definition(source, function_node)
    if name in ("TimedEventPool::Pop", "TimedEventPool::PopSafe"):
        first_param_type = first_parameter_type_name(source, function_node)
        if first_param_type in ("SC_MessageParser", "SpriteAction"):
            return f"{name}({first_param_type}*)"
    return name


def effective_function_start(function_node):
    parent = function_node.parent
    if parent is not None and parent.type == "linkage_specification":
        body = parent.child_by_field_name("body")
        if (
            body is not None
            and body.type == function_node.type
            and body.start_byte == function_node.start_byte
            and body.end_byte == function_node.end_byte
        ):
            return parent.start_byte
    return function_node.start_byte


def gap_is_only_comments_or_whitespace(source, start, end, comment_spans, span_starts):
    current = start

    while current < end:
        span_idx = bisect_right(span_starts, current) - 1
        if span_idx >= 0:
            span_start, span_end = comment_spans[span_idx]
            if span_start <= current < span_end:
                current = span_end
                continue

        next_span_idx = span_idx + 1
        if next_span_idx < len(comment_spans) and comment_spans[next_span_idx][0] == current:
            current = comment_spans[next_span_idx][1]
            continue

        if chr(source[current]).isspace():
            current += 1
            continue

        return False

    return True


def parse_source_function_markers(path, include_no_assembly=False, disambiguate_overloads=False):
    with open(path, "rb") as f:
        source = f.read()

    tree = CPP_PARSER.parse(sanitize_source(source))
    lines = source.splitlines()
    comment_spans = []
    comments = []
    functions = []

    for node in walk(tree.root_node):
        if node.type == "comment":
            comment_spans.append((node.start_byte, node.end_byte))
            line = node.start_point.row
            line_text = lines[line].decode("utf-8", errors="ignore") if line < len(lines) else ""
            address = parse_function_start_comment(node_text(source, node), line_text, include_no_assembly)
            if address is not None:
                comments.append((node.start_byte, node.end_byte, node.start_point.row + 1, address))
        elif node.type == "function_definition":
            if disambiguate_overloads:
                name = function_name_for_call_checker(source, node)
            else:
                name = function_name_from_definition(source, node)
            if name is not None:
                functions.append((effective_function_start(node), name))

    comments.sort()
    functions.sort()
    comment_spans.sort()
    function_starts = [start for start, _ in functions]
    comment_starts = [start for start, _ in comment_spans]
    markers = []

    for _, comment_end, line, address in comments:
        function_idx = bisect_right(function_starts, comment_end - 1)
        if function_idx >= len(functions):
            continue

        function_start, name = functions[function_idx]
        if gap_is_only_comments_or_whitespace(source, comment_end, function_start, comment_spans, comment_starts):
            markers.append(SourceFunctionMarker(address=address, name=name, line=line, function_start=function_start))

    return markers


def parse_source_function_comments(path, include_no_assembly=False):
    with open(path, "rb") as f:
        source = f.read()

    tree = CPP_PARSER.parse(sanitize_source(source))
    lines = source.splitlines()
    markers = []

    for node in walk(tree.root_node):
        if node.type != "comment":
            continue
        line = node.start_point.row
        line_text = lines[line].decode("utf-8", errors="ignore") if line < len(lines) else ""
        address = parse_function_start_comment(node_text(source, node), line_text, include_no_assembly)
        if address is not None:
            markers.append(SourceCommentMarker(address=address, line=line + 1))

    return markers


def parse_source_functions(path, include_no_assembly=False, disambiguate_overloads=False):
    return [
        SourceFunction(address=marker.address, name=marker.name, line=marker.line)
        for marker in parse_source_function_markers(path, include_no_assembly, disambiguate_overloads)
    ]


def parse_source_function_groups(path, include_no_assembly=False, disambiguate_overloads=False):
    groups = []
    current = None

    for marker in parse_source_function_markers(path, include_no_assembly, disambiguate_overloads):
        group_key = (marker.function_start, marker.name)
        if current is None or current["key"] != group_key:
            if current is not None:
                groups.append(SourceFunctionGroup(
                    addresses=tuple(current["addresses"]),
                    name=current["name"],
                    line=current["line"],
                ))
            current = {
                "key": group_key,
                "addresses": [],
                "name": marker.name,
                "line": marker.line,
            }
        current["addresses"].append(marker.address)

    if current is not None:
        groups.append(SourceFunctionGroup(
            addresses=tuple(current["addresses"]),
            name=current["name"],
            line=current["line"],
        ))

    return groups
