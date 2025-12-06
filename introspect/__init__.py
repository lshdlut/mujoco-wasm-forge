"""
Forge-facing entry point for MuJoCo's official introspect package.

The layout is:
  - introspect.official: verbatim copy of upstream MuJoCo introspect
  - introspect.forge: forge-specific tooling that consumes introspect

To keep upstream code unmodified, this shim re-exports the official
modules under the top-level `introspect` name so that imports such as
`from introspect import ast_nodes` continue to work exactly as in the
original package.
"""

from importlib import import_module as _import_module

# Re-export official submodules with the same names as upstream.
official_ast_nodes = _import_module(".official.ast_nodes", __name__)
official_type_parsing = _import_module(".official.type_parsing", __name__)
official_enums = _import_module(".official.enums", __name__)
official_functions = _import_module(".official.functions", __name__)
official_structs = _import_module(".official.structs", __name__)

ast_nodes = official_ast_nodes
type_parsing = official_type_parsing
enums = official_enums
functions = official_functions
structs = official_structs

# Expose the official codegen package for modules that import
# `from introspect import codegen`.
codegen = _import_module(".official.codegen", __name__)

__all__ = [
    "ast_nodes",
    "type_parsing",
    "enums",
    "functions",
    "structs",
    "codegen",
]

