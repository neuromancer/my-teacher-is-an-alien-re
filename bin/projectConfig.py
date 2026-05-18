#!/usr/bin/env python3
"""Shared configuration helpers for verification/report scripts."""

import json
import os


DEFAULT_CONFIG_PATH = "config/binary-comp.json"


class ConfigError(RuntimeError):
    pass


def add_config_argument(parser):
    parser.add_argument(
        "--config",
        default=os.environ.get("VERIFY_CONFIG", DEFAULT_CONFIG_PATH),
        help=f"Verification config JSON (default: {DEFAULT_CONFIG_PATH}, or VERIFY_CONFIG).",
    )


def load_config(path):
    if not path:
        return {}
    if not os.path.exists(path):
        raise ConfigError(f"config file not found: {path}")
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def require(value, label):
    if value in (None, ""):
        raise ConfigError(f"missing required configuration value: {label}")
    return value


def get_section(config, name):
    section = config.get(name, {})
    if section is None:
        return {}
    if not isinstance(section, dict):
        raise ConfigError(f"config section must be an object: {name}")
    return section


def mode_paths(config, demo=False):
    paths = get_section(config, "paths")
    mode = "demo" if demo else "full"
    result = paths.get(mode, {})
    if not isinstance(result, dict):
        raise ConfigError(f"paths.{mode} must be an object")
    return result


def parse_int(value):
    if value is None:
        return None
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise ConfigError(f"expected integer or integer string, got {value!r}")


def parse_int_map(mapping, value_parser=lambda value: value):
    if not mapping:
        return {}
    return {parse_int(key): value_parser(value) for key, value in mapping.items()}


def parse_int_value_map(mapping):
    return parse_int_map(mapping, parse_int)


def parse_range(value, label):
    if not isinstance(value, list) or len(value) != 2:
        raise ConfigError(f"{label} must be a two-item list")
    return parse_int(value[0]), parse_int(value[1])


def parse_slot_map(mapping):
    if not mapping:
        return {}
    return {int(key): value for key, value in mapping.items()}


def config_or_arg(arg_value, config, key, label=None):
    value = arg_value if arg_value not in (None, "") else config.get(key)
    return require(value, label or key)
