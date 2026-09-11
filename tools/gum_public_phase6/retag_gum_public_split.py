#!/usr/bin/env python3
"""Retag plain `GUM_PUBLIC` -> `GUM_PUBLIC_<MODULE>` (docs: md_docs/GUM_PUBLIC.md, §16
onwards, producer/consumer split).

Phase 6.4 (annotate_gum_public_phase6.py) tagged 117 non-template classes/structs
with a single, unconditional-dllexport `GUM_PUBLIC` macro. aGrUM's pure C++ build
links its 8 modules as separate shared libraries by default, so a single macro name
can't distinguish "this module owns the symbol" from "this module merely includes
the header that declares it" -- this script performs the mechanical part of the fix:
renaming each `GUM_PUBLIC` tag to the module-specific macro
(`GUM_PUBLIC_BASE`, `GUM_PUBLIC_BN`, ...) config.h.in and Modules.agrum.cmake now
define, per the module that owns the declaring header (src/modules.txt's
`${MODULE}_DIRS`).

A pure token substitution: doesn't touch tag placement (already corrected in Phase
6.4, GUM_PUBLIC.md §9), doesn't retarget which symbols are tagged, doesn't touch
`PYGUM_PUBLIC`/`PYGUM_SHARED_PUBLIC` (different macro family, out of scope).

Usage:
    python retag_gum_public_split.py --dry-run              # report only
    python retag_gum_public_split.py --dry-run --module BASE
    python retag_gum_public_split.py --module BASE           # apply, BASE only
    python retag_gum_public_split.py                         # apply, all modules
"""

import argparse
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent
MODULES_TXT = REPO_ROOT / "src/modules.txt"
AGRUM_SRC = REPO_ROOT / "src/agrum"

CLASS_LINE_RE = re.compile(
    r'^(?P<indent>\s*)(?P<kw>class|struct)\s+'
    r'(?P<tag>GUM_PUBLIC)\s+'
    r'(?P<name>[A-Za-z_]\w*)\b(?P<rest>.*)$'
)
DIR_SET_RE = re.compile(r'^\s*set\(\s*(?P<module>\w+)_DIRS\s+"(?P<dir>[^"]+)"\s*\)')

# BASE has no dependencies of its own (BASE_DEPS is empty, src/modules.txt) -- it
# never consumes another module's tagged symbols, only produces its own, so a
# single unqualified name is safe for it (mirrors PYGUM_SHARED_PUBLIC's BASE/BN
# special case). This does NOT generalize to other modules: BN, for instance, both
# produces its own symbols and consumes BASE's in the same TUs, so it needs its own
# distinct name, not this one, when its turn in the rollout comes -- see
# config.h.in's GUM_SHARED_PUBLIC comment for the full reasoning.
MACRO_NAME_OVERRIDES = {"BASE": "GUM_SHARED_PUBLIC"}


def load_dir_to_module() -> dict[str, str]:
    dir_to_module: dict[str, str] = {}
    for line in MODULES_TXT.read_text().splitlines():
        m = DIR_SET_RE.match(line)
        if m:
            dir_to_module[m.group("dir")] = m.group("module")
    return dir_to_module


def module_for(path: Path, dir_to_module: dict[str, str]) -> str | None:
    rel = path.relative_to(AGRUM_SRC)
    top_dir = rel.parts[0]
    return dir_to_module.get(top_dir)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dry-run", action="store_true", help="report only, do not modify files")
    parser.add_argument("--module", help="restrict to a single module (e.g. BASE); default: all modules")
    args = parser.parse_args()

    dir_to_module = load_dir_to_module()
    if args.module and args.module not in dir_to_module.values():
        parser.error(f"unknown module {args.module!r}; known: {sorted(set(dir_to_module.values()))}")

    retagged: list[tuple[str, int, str]] = []
    unmapped: list[Path] = []

    for path in sorted(AGRUM_SRC.rglob("*.h")):
        module = module_for(path, dir_to_module)
        if module is None:
            continue
        if args.module and module != args.module:
            continue

        text = path.read_text(errors="replace")
        lines = text.splitlines(keepends=False)
        touched = False

        for idx, line in enumerate(lines):
            m = CLASS_LINE_RE.match(line)
            if not m:
                continue
            new_tag = MACRO_NAME_OVERRIDES.get(module, f"GUM_PUBLIC_{module}")
            new_line = f'{m.group("indent")}{m.group("kw")} {new_tag} {m.group("name")}{m.group("rest")}'
            retagged.append((str(path.relative_to(REPO_ROOT)), idx + 1, new_tag))
            if not args.dry_run:
                lines[idx] = new_line
                touched = True

        if touched:
            path.write_text("\n".join(lines) + "\n")

    print(f"Retagged {len(retagged)} occurrence(s)" + (" [DRY RUN]" if args.dry_run else "") + ":")
    for file_rel, lineno, new_tag in retagged:
        print(f"  {file_rel}:{lineno}  -> {new_tag}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
