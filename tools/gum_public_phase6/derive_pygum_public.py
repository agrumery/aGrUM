#!/usr/bin/env python3
"""Phase 1 of the GUM_PUBLIC chantier (docs: md_docs/GUM_PUBLIC.md, §4).

Question this answers: which `gum::` C++ symbols (classes/structs and free
functions) does pyAgrum actually reference, across every SWIG extension
module (_pyagrum, _mrf, _cn, _id, _prm, _cm)? That set is the candidate
`PYGUM_PUBLIC` list -- before the inheritance-closure pass (Phase 2) and the
mechanical annotation pass (Phase 3).

Ground truth used: the SWIG-*generated* wrapper `.cxx` files already sitting
in the build tree (`build/pyAgrum/release/wrappers/pyagrum/raw/*PYTHON_wrap.cxx`),
not a re-parse of the `.i` sources. SWIG has already resolved every
`%ignore`/`%import`/`%include`/`%template`/namespace-matching rule for us by
the time these files exist -- reimplementing that resolution logic with
regexes on the `.i` files would be re-doing SWIG's job, badly. Concretely:

  - Classes: every generated wrap file has a `TYPES TABLE` with lines like
    `static swig_type_info _swigt__p_gum__BayesNetT_double_t =
    {"_p_gum__BayesNetT_double_t", "gum::BayesNet< double > *", ...};`
    -- the second string is SWIG's own resolved, human-readable C++ type
    name. We collect every such string that mentions `gum::` from every
    module's wrap file. This is deliberately over-inclusive (a type used
    only as an opaque pointer parameter is captured alongside a type with a
    full Python proxy class) -- see md_docs/GUM_PUBLIC.md §5: a symbol
    exported that didn't strictly need to be is harmless, a missing one is
    not, so Phase 1 is intentionally generous. Phase 6 (differed) is where
    precision matters, for the pure-C++-API decision, not here.

  - Free functions: heuristic. This codebase's convention is PascalCase
    classes / camelCase functions (confirmed throughout src/agrum/), so
    `gum::lowerCamel(` call sites in the generated wrap files are extracted
    as free-function candidates. This is best-effort -- expect to refine the
    list by hand in Phase 3, not a precise source of truth like the class
    extraction above.

Each found symbol is then cross-referenced against a from-scratch index of
`src/agrum/**/*.h` (flat headers only, matching the project convention that
`_inl.h`/`_tpl.h` never carry new declarations -- md_docs/GUM_PUBLIC.md §2.1)
to attach a declaring header + line number, for Phase 3 to use later.

Usage:
    python derive_pygum_public.py
    python derive_pygum_public.py --build-dir build/pyAgrum/release/wrappers/pyagrum/raw
    python derive_pygum_public.py --json-out /tmp/pygum_public.json

Requires a pyAgrum build already done (`act install release pyAgrum`) so the
generated `*PYTHON_wrap.cxx` files exist. Nothing here modifies the build or
the source tree -- read-only analysis.
"""

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
DEFAULT_BUILD_DIR = REPO_ROOT / "build/pyAgrum/release/wrappers/pyagrum/raw"
AGRUM_SRC = REPO_ROOT / "src/agrum"

SWIG_TYPE_RE = re.compile(r'static swig_type_info _swigt__\w+\s*=\s*\{"[^"]*",\s*"([^"]*)"')
GUM_QUALIFIED_RE = re.compile(r'gum::(?:[A-Za-z_]\w*::)*[A-Za-z_]\w*')
FREE_FUNC_CALL_RE = re.compile(r'gum::((?:[a-z_]\w*::)*)([a-z_]\w*)\s*(?:<[^;()]*>)?\s*\(')

CLASS_DECL_RE = re.compile(
    r'^\s*(?:template\s*<[^>]*>\s*)?(?:class|struct)\s+(?:[A-Z_][A-Z0-9_]*\s+)?([A-Za-z_]\w*)\s*(?:final\s*)?[:{;<]'
)
NAMESPACE_OPEN_RE = re.compile(r'\bnamespace\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*\{')
NAMESPACE_ANON_OPEN_RE = re.compile(r'\bnamespace\s*\{')


def strip_template_args(qualified_name: str) -> str:
    """'gum::BayesNet<double>' -> 'gum::BayesNet' (leave non-templated names untouched)."""
    idx = qualified_name.find("<")
    return qualified_name[:idx] if idx != -1 else qualified_name


def normalize_type_string(raw: str) -> list[str]:
    """Extract every `gum::...` qualified identifier chain mentioned in a SWIG
    type string, e.g. 'gum::Set< gum::NodeId > *' -> ['gum::Set', 'gum::NodeId'].
    Angle-bracket contents are ignored for the *base* name but nested gum::
    identifiers inside them are still picked up separately."""
    names = set()
    for m in GUM_QUALIFIED_RE.finditer(raw):
        names.add(m.group(0))
    return sorted(names)


def collect_from_wrap_file(path: Path) -> tuple[set[str], set[str]]:
    text = path.read_text(errors="replace")
    classes = set()
    for m in SWIG_TYPE_RE.finditer(text):
        for name in normalize_type_string(m.group(1)):
            classes.add(strip_template_args(name))

    functions = set()
    for m in FREE_FUNC_CALL_RE.finditer(text):
        prefix, funcname = m.group(1), m.group(2)
        functions.add(f"gum::{prefix}{funcname}")

    return classes, functions


def build_header_index(src_dir: Path) -> dict[str, list[tuple[str, int]]]:
    """qualified name (e.g. 'gum::learning::BNLearner') -> [(relpath, lineno), ...]"""
    index: dict[str, list[tuple[str, int]]] = defaultdict(list)
    for header in sorted(src_dir.rglob("*.h")):
        name = header.name
        if name.endswith("_inl.h") or name.endswith("_tpl.h"):
            continue
        try:
            lines = header.read_text(errors="replace").splitlines()
        except OSError:
            continue

        ns_stack: list[str] = []
        depth_stack: list[int] = []
        depth = 0
        for lineno, raw_line in enumerate(lines, start=1):
            line = raw_line.split("//", 1)[0]

            ns_m = NAMESPACE_OPEN_RE.search(line)
            if ns_m:
                for part in ns_m.group(1).split("::"):
                    ns_stack.append(part)
                    depth_stack.append(depth)
            elif NAMESPACE_ANON_OPEN_RE.search(line):
                ns_stack.append("<anon>")
                depth_stack.append(depth)

            cls_m = CLASS_DECL_RE.match(line)
            if cls_m and ns_stack and ns_stack[0] == "gum":
                qualified = "::".join(ns_stack + [cls_m.group(1)])
                relpath = str(header.relative_to(REPO_ROOT))
                index[qualified].append((relpath, lineno))

            depth += line.count("{") - line.count("}")
            while depth_stack and depth < depth_stack[-1] + 1 and depth <= depth_stack[-1]:
                # namespace block closed
                depth_stack.pop()
                ns_stack.pop()

    return index


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR,
                         help="directory containing *PYTHON_wrap.cxx (default: %(default)s)")
    parser.add_argument("--json-out", type=Path, default=REPO_ROOT / "md_docs/gum_public_phase1_candidates.json",
                         help="where to write the machine-readable result")
    args = parser.parse_args()

    wrap_files = sorted(args.build_dir.glob("*PYTHON_wrap.cxx"))
    if not wrap_files:
        print(f"No *PYTHON_wrap.cxx found in {args.build_dir}", file=sys.stderr)
        print("Run `act install release pyAgrum` first (or pass --build-dir).", file=sys.stderr)
        return 1

    print(f"Reading {len(wrap_files)} generated SWIG wrap files from {args.build_dir}")

    classes_by_module: dict[str, set[str]] = {}
    functions_by_module: dict[str, set[str]] = {}
    for wf in wrap_files:
        module = wf.name.removesuffix("PYTHON_wrap.cxx")
        classes, functions = collect_from_wrap_file(wf)
        classes_by_module[module] = classes
        functions_by_module[module] = functions
        print(f"  {module:10s}: {len(classes):4d} gum:: types, {len(functions):3d} candidate free functions")

    all_classes: dict[str, set[str]] = defaultdict(set)
    for module, classes in classes_by_module.items():
        for c in classes:
            all_classes[c].add(module)

    all_functions: dict[str, set[str]] = defaultdict(set)
    for module, functions in functions_by_module.items():
        for f in functions:
            all_functions[f].add(module)

    print(f"\nTotal distinct gum:: types referenced across all modules: {len(all_classes)}")
    print(f"Total distinct candidate free functions: {len(all_functions)}")

    print(f"\nIndexing headers under {AGRUM_SRC} ...")
    header_index = build_header_index(AGRUM_SRC)
    print(f"  {len(header_index)} qualified class/struct names indexed")

    resolved = {}
    unresolved = []
    for name, modules in sorted(all_classes.items()):
        locations = header_index.get(name)
        if locations:
            resolved[name] = {
                "modules": sorted(modules),
                "locations": [{"file": f, "line": l} for f, l in locations],
            }
        else:
            unresolved.append({"name": name, "modules": sorted(modules)})

    print(f"\nResolved to a header declaration: {len(resolved)}")
    print(f"Unresolved (typedef/alias/nested-type/parser gap -- needs manual look): {len(unresolved)}")

    result = {
        "modules": sorted(classes_by_module.keys()),
        "classes": {
            "resolved": resolved,
            "unresolved": unresolved,
        },
        "functions": {
            name: sorted(modules) for name, modules in sorted(all_functions.items())
        },
    }
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(result, indent=2, sort_keys=True))
    print(f"\nWrote {args.json_out}")

    if unresolved:
        print("\nUnresolved names (first 40):")
        for entry in unresolved[:40]:
            print(f"  {entry['name']}  (used by: {', '.join(entry['modules'])})")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
