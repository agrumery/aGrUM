#!/usr/bin/env python3
"""Phase 6.2 of the GUM_PUBLIC chantier (docs: md_docs/GUM_PUBLIC.md, §4).

Goal: shrink the 538-candidate set produced by Phase 6.1
(md_docs/gum_public_phase6_candidates.json) down to the subset that
actually needs manual eyes, by computing two automatic exclusion *signals*
-- not decisions, cf. md_docs/GUM_PUBLIC.md §4 Phase 6.2 ("pas une décision
automatique"). No @internal Doxygen marker exists to lean on (confirmed
§2.4), so:

  - Signal A (high confidence): PIMPL-like -- every location Phase 6.1
    recorded for this candidate is a forward declaration ("class X;"), i.e.
    no header anywhere in src/agrum shows an actual class body. A class
    genuinely defined only in a .cpp file cannot sanely be handed a public
    C++ API contract (nothing to instantiate/inherit against from outside
    its own translation unit).
  - Signal B (medium confidence): the header declaring this candidate is
    never #include-d from any file outside its own containing directory
    (a purely textual reverse-include graph, built from
    `#include <agrum/...>` lines -- the project convention per CLAUDE.md,
    no relative includes to worry about). Weak on its own (plenty of
    legitimately-public classes are only used within their own module) --
    kept as "flag for review", not "exclude".

Candidates that trip neither signal are left as-is under the public-by-default
policy (md_docs/GUM_PUBLIC.md §6 point 4): no manual review needed for them
specifically, unless a later pass finds a reason to.

Usage:
    python signal_gum_public_phase6.py
    python signal_gum_public_phase6.py --candidates-json md_docs/gum_public_phase6_candidates.json

Read-only.
"""

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
AGRUM_SRC = REPO_ROOT / "src/agrum"

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*<agrum/([^>]+)>', re.MULTILINE)


def line_terminator(file_rel: str, lineno: int) -> str | None:
    """Return the CLASS_DECL_RE terminator char ('{', ';', or ':') found on
    this line, ignoring trailing comments/whitespace -- None if the line
    can't be read or doesn't end as expected."""
    path = REPO_ROOT / file_rel
    try:
        lines = path.read_text(errors="replace").splitlines()
    except OSError:
        return None
    if not (1 <= lineno <= len(lines)):
        return None
    line = lines[lineno - 1].split("//", 1)[0].rstrip()
    if not line:
        return None
    return line[-1] if line[-1] in "{;:<" else None


def build_reverse_include_graph() -> dict[str, set[str]]:
    """relpath (e.g. 'base/foo/bar.h') -> set of includer relpaths that
    #include it via <agrum/...>."""
    includers: dict[str, set[str]] = defaultdict(set)
    for header in AGRUM_SRC.rglob("*.h"):
        rel_from_src = str(header.relative_to(AGRUM_SRC.parent))  # 'agrum/...'
        includer_rel = "src/" + rel_from_src  # matches candidate location "file" format
        try:
            text = header.read_text(errors="replace")
        except OSError:
            continue
        for m in INCLUDE_RE.finditer(text):
            included = m.group(1)  # path after 'agrum/'
            includers[f"src/agrum/{included}"].add(includer_rel)
    for cpp in AGRUM_SRC.rglob("*.cpp"):
        includer_rel = "src/" + str(cpp.relative_to(AGRUM_SRC.parent))
        try:
            text = cpp.read_text(errors="replace")
        except OSError:
            continue
        for m in INCLUDE_RE.finditer(text):
            included = m.group(1)
            includers[f"src/agrum/{included}"].add(includer_rel)
    return includers


def own_dir(relpath: str) -> str:
    return str(Path(relpath).parent)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--candidates-json", type=Path,
                         default=REPO_ROOT / "md_docs/gum_public_phase6_candidates.json")
    parser.add_argument("--json-out", type=Path,
                         default=REPO_ROOT / "md_docs/gum_public_phase6_signals.json")
    args = parser.parse_args()

    data = json.loads(args.candidates_json.read_text())
    candidates = data["candidates"]
    print(f"Loaded {len(candidates)} Phase 6.1 candidates from {args.candidates_json}")

    print("Building reverse #include graph (src/agrum/**/*.h + *.cpp) ...")
    includers = build_reverse_include_graph()
    print(f"  {len(includers)} headers have at least one includer recorded")

    signals: dict[str, dict] = {}
    counts = {"high": 0, "medium": 0, "none": 0}

    for qualified, entry in candidates.items():
        locations = entry["locations"]
        if not locations:
            signals[qualified] = {"confidence": "none", "reasons": ["no location recorded"], **entry}
            counts["none"] += 1
            continue

        terminators = [line_terminator(loc["file"], loc["line"]) for loc in locations]
        pimpl_like = bool(terminators) and all(t == ";" for t in terminators if t is not None) \
            and any(t is not None for t in terminators)

        decl_file = locations[0]["file"]
        decl_dir = own_dir(decl_file)
        file_includers = includers.get(decl_file, set())
        included_outside_own_dir = any(own_dir(inc) != decl_dir for inc in file_includers)

        reasons = []
        if pimpl_like:
            confidence = "high"
            reasons.append("pimpl-like: every recorded location is a forward declaration, no body found in headers")
        elif not included_outside_own_dir:
            confidence = "medium"
            reasons.append(f"header {decl_file!r} never #include-d outside its own directory ({decl_dir!r})")
        else:
            confidence = "none"

        counts[confidence] += 1
        signals[qualified] = {"confidence": confidence, "reasons": reasons, **entry}

    print(f"\nSignal results across {len(candidates)} candidates:")
    print(f"  high confidence exclude (PIMPL-like):        {counts['high']:4d}")
    print(f"  medium confidence exclude (module-local only): {counts['medium']:4d}")
    print(f"  no signal (public-by-default, no review needed): {counts['none']:4d}")
    print(f"\n  => manual review shortlist: {counts['high'] + counts['medium']} "
          f"(down from {len(candidates)}, {counts['none']} auto-cleared for GUM_PUBLIC)")

    args.json_out.write_text(json.dumps(signals, indent=2, sort_keys=True))
    print(f"\nWrote {args.json_out}")

    print("\nHigh-confidence shortlist:")
    for qualified, s in sorted(signals.items()):
        if s["confidence"] == "high":
            loc = s["locations"][0] if s["locations"] else {}
            print(f"  {qualified:60s} {loc.get('file', '?')}:{loc.get('line', '?')}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
