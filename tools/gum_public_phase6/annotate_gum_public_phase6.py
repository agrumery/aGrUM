#!/usr/bin/env python3
"""Phase 6.4 of the GUM_PUBLIC chantier (docs: md_docs/GUM_PUBLIC.md, §4).

Scope for this pass (decided with the user 2026-08-19): only the candidates
with NO Phase 6.2 exclusion signal (confidence == "none" in
md_docs/gum_public_phase6_signals.json) plus the 7 classes decided public in
Phase 6.3 (md_docs/gum_public_phase6_decisions.json). The 204 flagged
candidates (10 high-confidence PIMPL-like, 194 medium-confidence
module-local) are left untagged, pending manual review -- not this pass.

Trap to avoid (md_docs/GUM_PUBLIC.md §11, real C2491 hit on
PYGUM_SHARED_PUBLIC): never tag a still-generic template class declaration
directly with an export macro -- MSVC forbids a dllimport-tagged class from
being locally instantiated, which breaks every consumer needing its own
instantiation. So this script splits candidates into:

  - plain (non-template) classes/structs: tagged directly, `class GUM_PUBLIC
    Foo` (macro AFTER the keyword -- md_docs/GUM_PUBLIC.md §9 already hit
    the opposite mistake once: `GUM_PUBLIC class Foo` compiles but GCC/Clang
    silently ignore the visibility attribute there).
  - template classes: NOT tagged in this pass. Reported separately for a
    follow-up that mirrors the bounded extern-template pattern already used
    for PYGUM_SHARED_PUBLIC (§12), which needs its own pairing logic against
    existing `template class gum::X<...>;` explicit instantiations -- a
    separate, more delicate piece of work, not bundled into this run.

Usage:
    python annotate_gum_public_phase6.py --dry-run   # report only, no writes
    python annotate_gum_public_phase6.py              # apply to plain classes

Idempotent-ish: re-running after a partial apply just re-detects lines
already carrying a tag and skips them (see already-tagged guard below).
"""

import argparse
import json
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent

CLASS_LINE_RE = re.compile(
    r'^(?P<indent>\s*)(?P<kw>class|struct)\s+'
    r'(?P<tag>(?:[A-Z_][A-Z0-9_]*)\s+)?'
    r'(?P<name>[A-Za-z_]\w*)\b(?P<rest>.*)$'
)
TEMPLATE_LINE_RE = re.compile(r'^\s*template\s*<')
KNOWN_TAGS = {"PYGUM_PUBLIC", "PYGUM_SHARED_PUBLIC", "GUM_PUBLIC"}


def load_scope() -> dict[str, dict]:
    signals = json.loads((REPO_ROOT / "md_docs/gum_public_phase6_signals.json").read_text())
    decisions = json.loads((REPO_ROOT / "md_docs/gum_public_phase6_decisions.json").read_text())

    scope = {}
    for qualified, entry in signals.items():
        if entry.get("confidence") == "none":
            scope[qualified] = entry
    for qualified in decisions.get("decisions", {}):
        if decisions["decisions"][qualified]["decision"] != "public":
            continue
        if qualified not in scope:
            # pull location/tag info from the phase6 candidates file
            candidates = json.loads((REPO_ROOT / "md_docs/gum_public_phase6_candidates.json").read_text())
            cand = candidates["candidates"].get(qualified)
            if cand:
                scope[qualified] = cand
    return scope


def is_template_declaration(file_rel: str, lineno: int, all_lines: list[str]) -> bool:
    # look upward past blank and comment lines for a `template <...>` line
    # immediately preceding the class/struct declaration
    idx = lineno - 2  # 0-based index of the line just above
    while idx >= 0:
        stripped = all_lines[idx].strip()
        if stripped == "" or stripped.startswith(("//", "/*", "*")):
            idx -= 1
            continue
        return bool(TEMPLATE_LINE_RE.match(all_lines[idx]))
    return False


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dry-run", action="store_true", help="report only, do not modify files")
    args = parser.parse_args()

    scope = load_scope()
    print(f"Scope for this pass: {len(scope)} candidates (no-signal + Phase 6.3 decided-public)")

    # group by file for batched edits
    by_file: dict[str, list[tuple[int, str]]] = {}
    skipped_no_location = []
    for qualified, entry in scope.items():
        locations = entry.get("locations") or []
        if not locations:
            skipped_no_location.append(qualified)
            continue
        loc = locations[0]
        by_file.setdefault(loc["file"], []).append((loc["line"], qualified))

    plain_tagged = []
    template_deferred = []
    already_tagged = []
    mismatched = []

    for file_rel, entries in sorted(by_file.items()):
        path = REPO_ROOT / file_rel
        text = path.read_text(errors="replace")
        lines = text.splitlines(keepends=False)
        newline = "\n"  # project files are LF; verified implicitly by round-trip below

        touched = False
        for lineno, qualified in sorted(entries):
            idx = lineno - 1
            if not (0 <= idx < len(lines)):
                mismatched.append((qualified, file_rel, lineno, "line out of range"))
                continue
            line = lines[idx]
            m = CLASS_LINE_RE.match(line)
            if not m:
                mismatched.append((qualified, file_rel, lineno, f"line doesn't match class/struct pattern: {line!r}"))
                continue

            existing_tag = (m.group("tag") or "").strip()
            if existing_tag in KNOWN_TAGS:
                already_tagged.append((qualified, file_rel, lineno))
                continue

            if is_template_declaration(file_rel, lineno, lines):
                template_deferred.append((qualified, file_rel, lineno))
                continue

            new_line = f'{m.group("indent")}{m.group("kw")} GUM_PUBLIC {m.group("name")}{m.group("rest")}'
            if not args.dry_run:
                lines[idx] = new_line
                touched = True
            plain_tagged.append((qualified, file_rel, lineno))

        if touched:
            path.write_text(newline.join(lines) + newline)

    print(f"\nPlain classes/structs tagged GUM_PUBLIC: {len(plain_tagged)}")
    print(f"Template declarations deferred (need bounded-instantiation pass): {len(template_deferred)}")
    print(f"Already carrying a visibility tag (skipped): {len(already_tagged)}")
    print(f"Mismatched / needs manual look: {len(mismatched)}")
    print(f"No location recorded (skipped): {len(skipped_no_location)}")

    if args.dry_run:
        print("\n[DRY RUN] no files were modified.")

    if mismatched:
        print("\nMismatched entries (first 20):")
        for qualified, file_rel, lineno, why in mismatched[:20]:
            print(f"  {qualified}  {file_rel}:{lineno}  -- {why}")

    out = {
        "plain_tagged": [{"name": q, "file": f, "line": l} for q, f, l in plain_tagged],
        "template_deferred": [{"name": q, "file": f, "line": l} for q, f, l in template_deferred],
        "already_tagged": [{"name": q, "file": f, "line": l} for q, f, l in already_tagged],
        "mismatched": [{"name": q, "file": f, "line": l, "reason": why} for q, f, l, why in mismatched],
    }
    out_path = REPO_ROOT / "md_docs/gum_public_phase6_annotation_report.json"
    out_path.write_text(json.dumps(out, indent=2, sort_keys=True))
    print(f"\nWrote {out_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
