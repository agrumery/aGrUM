#!/usr/bin/env python3
"""Scan src/agrum/ for visibility-macro tags and generate a Doxygen summary page.

Doxygen's C++ parser silently drops unknown macro-like tokens placed right
before a class/struct/function declaration (GUM_SHARED_PUBLIC, GUM_PUBLIC_BN,
PYGUM_PUBLIC, ...), so none of them show up anywhere in the generated
documentation. This script scans every plain header under src/agrum/ (skipping
_inl.h/_tpl.h, generated parsers and vendored code, same exclusions as
tools/gum_public_phase6/derive_gum_public_phase6.py) for each known tag and
writes one markdown table per tag into a generated .dox page under
src/docs/modules/, so \\defgroup modularization_group has a live, clickable
summary of which class/struct/function currently carries which tag.

Entries are written as plain (non-code-span) text so Doxygen's autolink
resolves them to the real class/function page whenever it recognizes the
name -- a namespace-depth scan (brace counting, not a real parser, same
"right order of magnitude" precision as the rest of this project's Phase 6
tooling) qualifies each name with its enclosing namespace so autolink has a
fair chance even for common short names.

Usage:
    python tools/generate_visibility_tags_doc.py
    python tools/generate_visibility_tags_doc.py --check   # exit 1 if the
                                                             # generated file
                                                             # would change
"""

import argparse
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
AGRUM_SRC = REPO_ROOT / "src/agrum"
OUT_PATH = REPO_ROOT / "src/docs/modules/visibility_tags_summary.dox"

EXCLUDE_DIR_PARTS = {"external", "cocoR"}

TAGS = [
    "GUM_SHARED_PUBLIC",
    "GUM_PUBLIC_BN",
    "GUM_PUBLIC_PRM",
    "GUM_PUBLIC_MRF",
    "GUM_PUBLIC_CN",
    "GUM_PUBLIC_FMDP",
    "GUM_PUBLIC_ID",
    "GUM_PUBLIC_CM",
    "PYGUM_SHARED_PUBLIC",
    "PYGUM_PUBLIC",
]

# class/struct declarations: `class TAG Name`, `struct TAG Name< ... >`,
# stopping at the inheritance colon or the opening brace.
CLASS_RE = {
    tag: re.compile(
        rf"^\s*(?:class|struct)\s+{tag}\s+(.+?)"
        r"(?:\s*:\s*(?:public|private|protected|virtual)\b|\s*\{|\s*$)"
    )
    for tag in TAGS
}
# free function / friend operator declarations: `TAG <rest of declaration>`
FREE_RE = {tag: re.compile(rf"^\s*(?:friend\s+)?{tag}\s+(.+)$") for tag in TAGS}

NAMESPACE_OPEN_RE = re.compile(r"^\s*namespace\s+(\w+)\s*\{")


def strip_line_comments(line: str, in_block: bool) -> tuple[str, bool]:
    """Strip `//` and `/* ... */` comments from `line` (ported from
    tools/gum_public_phase6/derive_gum_public_phase6.py: without this, braces
    or macro-looking text inside a comment throw off both the namespace-depth
    count and the tag regexes)."""
    out = []
    i, n = 0, len(line)
    while i < n:
        if in_block:
            end = line.find("*/", i)
            if end == -1:
                return "".join(out), True
            in_block = False
            i = end + 2
            continue
        if line[i : i + 2] == "/*":
            end = line.find("*/", i + 2)
            if end == -1:
                return "".join(out), True
            i = end + 2
            continue
        if line[i : i + 2] == "//":
            break
        out.append(line[i])
        i += 1
    return "".join(out), in_block


def iter_plain_headers():
    for path in sorted(AGRUM_SRC.rglob("*.h")):
        if path.name.endswith(("_inl.h", "_tpl.h")):
            continue
        if EXCLUDE_DIR_PARTS & set(path.parts):
            continue
        yield path


def collect() -> dict[str, list[tuple[str, str, int]]]:
    """Returns {tag: [(qualified_name, file_rel, line), ...]}."""
    results: dict[str, list[tuple[str, str, int]]] = {tag: [] for tag in TAGS}
    for path in iter_plain_headers():
        rel = str(path.relative_to(REPO_ROOT))
        raw_lines = path.read_text(errors="replace").splitlines()

        ns_stack: list[str] = []
        ns_depths: list[int] = []
        depth = 0
        in_block_comment = False

        for lineno, raw_line in enumerate(raw_lines, start=1):
            line, in_block_comment = strip_line_comments(raw_line, in_block_comment)

            ns_match = NAMESPACE_OPEN_RE.match(line)
            if ns_match:
                ns_stack.append(ns_match.group(1))
                ns_depths.append(depth)

            depth += line.count("{") - line.count("}")
            while ns_depths and depth <= ns_depths[-1]:
                ns_stack.pop()
                ns_depths.pop()

            prefix = "::".join(ns_stack) + "::" if ns_stack else ""
            for tag in TAGS:
                m = CLASS_RE[tag].match(line)
                if m:
                    results[tag].append((prefix + m.group(1).strip().rstrip(":").strip(), rel, lineno))
                    break
                m = FREE_RE[tag].match(line)
                if m and not re.match(r"^(?:class|struct)\b", m.group(1)):
                    sig = m.group(1).strip().rstrip("{").strip()
                    results[tag].append((sig, rel, lineno))
                    break
    return results


def render(results: dict[str, list[tuple[str, str, int]]]) -> str:
    header = REPO_ROOT / "tools" / "_dox_license_header.txt"
    license_block = header.read_text() if header.exists() else ""

    body = [
        license_block,
        "",
        "/**",
        " * @file",
        " * @brief Auto-generated summary of visibility-macro tags (do not edit by",
        " * hand -- regenerate with `python tools/generate_visibility_tags_doc.py`).",
        " */",
        "",
        "/*!",
        " * \\page visibility_tags_summary Visibility tags",
        " *",
        " * Auto-generated by `tools/generate_visibility_tags_doc.py` (also run",
        " * automatically by `act doc release aGrUM`) -- names are plain text so",
        " * Doxygen's autolink resolves them to the real class/function page when it",
        " * recognizes them. See \\ref modularization_group for what each tag means.",
        " *",
        " * Still-generic templates (`template <typename T> class Foo`) are never",
        " * tagged and so never listed below: a template is re-instantiated locally",
        " * in every consumer from its header definition, so it never needs to cross",
        " * a binary boundary as a distinct symbol.",
        " *",
    ]

    total = sum(len(v) for v in results.values())
    body.append(f" * {total} tagged symbols across {len(TAGS)} tags, as of the last run.")
    body.append(" *")

    for tag in TAGS:
        entries = sorted(results[tag], key=lambda e: e[0].lower())
        anchor = tag.lower()
        body.append(f" * \\section tagsum_{anchor} `{tag}` ({len(entries)})")
        body.append(" *")
        if not entries:
            body.append(" * _None currently tagged._")
            body.append(" *")
            continue
        body.append(" * | Symbol | Location |")
        body.append(" * |---|---|")
        for name, rel, lineno in entries:
            escaped = name.replace("|", "\\|")
            body.append(f" * | {escaped} | `{rel}:{lineno}` |")
        body.append(" *")

    body.append(" */")
    return "\n".join(body) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--check", action="store_true", help="exit 1 if the file would change, do not write")
    args = parser.parse_args()

    results = collect()
    new_content = render(results)

    if args.check:
        old_content = OUT_PATH.read_text() if OUT_PATH.exists() else ""
        if old_content != new_content:
            print(f"{OUT_PATH} is stale, run without --check to refresh")
            return 1
        print(f"{OUT_PATH} is up to date")
        return 0

    OUT_PATH.write_text(new_content)
    total = sum(len(v) for v in results.values())
    print(f"Wrote {OUT_PATH} ({total} tagged symbols across {len(TAGS)} tags)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
