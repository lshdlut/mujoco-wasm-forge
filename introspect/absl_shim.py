#!/usr/bin/env python3
"""Minimal absl.app/absl.flags shim for official introspect codegen.

The upstream MuJoCo introspection tools depend on `absl` for command-line
parsing, but we want to keep the forge pipeline self-contained without
introducing a hard dependency on absl-py in CI. This module implements the
subset of the API used by the code generators:

  - from absl import app      -> app.run, app.UsageError
  - from absl import flags    -> flags.DEFINE_string(...).value

It is intentionally tiny and only supports string flags and the "--flag value"
or "--flag=value" forms.
"""

from __future__ import annotations

import sys
from dataclasses import dataclass
from typing import Dict, List, Sequence


@dataclass
class _Flag:
    name: str
    value: str | None
    help: str


class _Flags:
    def __init__(self) -> None:
        self._registry: Dict[str, _Flag] = {}

    # API surface compatible with absl.flags.DEFINE_string.
    def DEFINE_string(self, name: str, default: str | None, help: str) -> _Flag:
        flag = _Flag(name=name, value=default, help=help)
        self._registry[name] = flag
        return flag

    def _parse(self, argv: Sequence[str]) -> List[str]:
        """Very small parser for --name value / --name=value."""
        remaining: List[str] = []
        i = 0
        while i < len(argv):
            arg = argv[i]
            if arg.startswith("--"):
                body = arg[2:]
                name: str
                val: str | None
                if "=" in body:
                    name, val = body.split("=", 1)
                else:
                    name = body
                    val = None
                if name in self._registry:
                    # Consume a separate value argument when needed.
                    if val is None and i + 1 < len(argv):
                        val = argv[i + 1]
                        i += 1
                    self._registry[name].value = val
                else:
                    remaining.append(arg)
            else:
                remaining.append(arg)
            i += 1
        return remaining


class _App:
    class UsageError(Exception):
        """Raised when the wrapped main decides the usage is invalid."""

    # API surface compatible with absl.app.run(main).
    def run(self, main) -> None:
        argv = sys.argv[1:]
        # Let flags shim consume flag-like args; pass the rest to main().
        parsed_argv = flags._parse(argv)
        # main() in the upstream code expects a Sequence[str] of remaining args.
        main(parsed_argv)


# Public shims that mimic `from absl import app, flags`.
flags = _Flags()
app = _App()

