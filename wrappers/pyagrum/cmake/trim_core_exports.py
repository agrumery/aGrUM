############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Discovers, from the already-built leaf pyAgrum extension modules, exactly
which BASE/BN symbols the core actually needs to keep exported -- instead of
trusting the PYGUM_SHARED_PUBLIC source annotations (which control what
*can* be exported, not what a whole-archived core actually keeps once dead
code removal is enabled). Emits a platform-specific export list consumed by
a second, restricted link of the core in wrappers/pyagrum/CMakeLists.txt.
"""

import argparse
import re
import subprocess
import sys


def _run(cmd):
  return subprocess.run(cmd, capture_output=True, text=True, check=True).stdout


def apple_defined_symbols(path):
  out = _run(["nm", "-gU", path])
  syms = set()
  for line in out.splitlines():
    parts = line.split()
    if len(parts) >= 3:
      syms.add(parts[-1])
  return syms


def apple_undefined_symbols(path):
  out = _run(["nm", "-gu", path])
  return {line.strip() for line in out.splitlines() if line.strip()}


def gnu_defined_symbols(path):
  # -D: dynamic symbol table (what a .so actually exposes at load time),
  # not the full static symbol table nm shows by default.
  out = _run(["nm", "-D", "--defined-only", path])
  syms = set()
  for line in out.splitlines():
    parts = line.split()
    if len(parts) >= 3:
      syms.add(parts[-1])
  return syms


def gnu_undefined_symbols(path):
  out = _run(["nm", "-D", "--undefined-only", path])
  syms = set()
  for line in out.splitlines():
    parts = line.split()
    if parts:
      syms.add(parts[-1])
  return syms


_DUMPBIN_EXPORT_RE = re.compile(r"^\s*\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+(\S+)")
_DUMPBIN_IMPORT_RE = re.compile(r"^\s+[0-9A-Fa-f]+\s+(\S+)\s*$")


def msvc_defined_symbols(path):
  out = _run(["dumpbin", "/EXPORTS", path])
  syms = set()
  in_table = False
  for line in out.splitlines():
    if "ordinal" in line and "name" in line:
      in_table = True
      continue
    if not in_table:
      continue
    m = _DUMPBIN_EXPORT_RE.match(line)
    if m:
      syms.add(m.group(1))
  return syms


def msvc_undefined_symbols_from_core(path, core_basename):
  # dumpbin /IMPORTS lists one block per DLL this binary imports from; we
  # only want the block for the core, not msvcrt/python3xx/etc.
  out = _run(["dumpbin", "/IMPORTS", path])
  syms = set()
  in_block = False
  for line in out.splitlines():
    stripped = line.strip()
    if stripped.lower() == core_basename.lower():
      in_block = True
      continue
    if in_block:
      if stripped == "" or stripped.endswith(".dll") or stripped.endswith(".pyd"):
        # blank line or a new DLL block name ends the current block
        if stripped != "":
          in_block = False
        continue
      m = _DUMPBIN_IMPORT_RE.match(line)
      if m:
        syms.add(m.group(1))
  return syms


def main():
  p = argparse.ArgumentParser()
  p.add_argument("--core", required=True)
  p.add_argument("--leaf", action="append", default=[])
  p.add_argument("--keep", action="append", default=[])
  p.add_argument("--format", required=True, choices=["apple", "gnu", "msvc"])
  p.add_argument("--core-basename", default="", help="core DLL file name, msvc format only")
  p.add_argument("--out", required=True)
  args = p.parse_args()

  if args.format == "apple":
    core_defined = apple_defined_symbols(args.core)
    undefined_per_leaf = [apple_undefined_symbols(leaf) for leaf in args.leaf]
    defined_per_leaf = [apple_defined_symbols(leaf) for leaf in args.leaf]
  elif args.format == "gnu":
    core_defined = gnu_defined_symbols(args.core)
    undefined_per_leaf = [gnu_undefined_symbols(leaf) for leaf in args.leaf]
    defined_per_leaf = [gnu_defined_symbols(leaf) for leaf in args.leaf]
  else:
    core_defined = msvc_defined_symbols(args.core)
    undefined_per_leaf = [
      msvc_undefined_symbols_from_core(leaf, args.core_basename) for leaf in args.leaf
    ]
    defined_per_leaf = [msvc_defined_symbols(leaf) for leaf in args.leaf]

  # --keep values are given as plain (undecorated) C symbol names. Apple's
  # Mach-O convention prefixes every C symbol with a leading underscore
  # (already present on everything nm reports below, hence already in
  # core_defined/undefined_per_leaf) -- apply the same prefix here so a
  # manually-specified "always keep" name like the module's Python init
  # function matches what the linker actually expects. ELF (gnu) and
  # MSVC/COFF x64 use undecorated names, so left as-is there.
  if args.format == "apple":
    keep = {f"_{name}" if not name.startswith("_") else name for name in args.keep}
  else:
    keep = set(args.keep)

  needed = set(keep)
  for undefined in undefined_per_leaf:
    needed |= undefined & core_defined
  # A symbol independently DEFINED in both a leaf and the core can only
  # happen for a weak/COMDAT definition: an inline function, a template
  # instantiation, or the vtable/typeinfo of a class with no out-of-line
  # "key function" (every gum:: exception class in exceptions.h -- see
  # GUM_MAKE_ERROR -- is exactly that: header-only, all methods inline).
  # Each shared library that uses such a class gets its own weak copy; the
  # dynamic linker coalesces them into a single one at load time, but only
  # if core's copy stays exported. A leaf never references a symbol it
  # already defines itself as an *undefined* symbol, so the scan above can
  # never see this dependency -- silently losing it breaks any cross-.so
  # virtual dispatch or RTTI that must resolve through the coalesced
  # symbol (e.g. a leaf catching a core-thrown gum::Exception subclass and
  # calling its pythonClassName_() override to translate it to Python).
  for defined in defined_per_leaf:
    needed |= defined & core_defined

  if not needed:
    print(
      "trim_core_exports: no symbols found -- refusing to write an empty export list "
      "(this would strip the core down to nothing)",
      file=sys.stderr,
    )
    sys.exit(1)

  with open(args.out, "w", encoding="utf-8") as f:
    if args.format == "gnu":
      f.write("{\n  global:\n")
      for sym in sorted(needed):
        f.write(f"    {sym};\n")
      f.write("  local:\n    *;\n};\n")
    elif args.format == "msvc":
      f.write("EXPORTS\n")
      for sym in sorted(needed):
        f.write(f"    {sym}\n")
    else:
      for sym in sorted(needed):
        f.write(sym + "\n")

  print(f"trim_core_exports: kept {len(needed)}/{len(core_defined)} core symbols -> {args.out}")


if __name__ == "__main__":
  main()
