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

"""Run an example of the documentation (doc/examples/<page name>.py) and check the results it shows.

An example is a Python script. A comment block right after an expression shows its result: ``# <repr of the value>``
(``#`` alone for an empty line), or the printed text for a ``print(...)`` call. Other comments must not directly
follow an expression. A first line ``# requires: module1, module2`` names optional modules needed by the example.

Usable as a script to check examples while writing them: ``python examplesRunner.py file1.py file2.py ...``.
"""

import ast
import contextlib
import importlib.util
import io
import os
import sys
import tempfile
import warnings
from pathlib import Path

os.environ.setdefault("MPLBACKEND", "Agg")  # no window for the examples drawing with matplotlib


def requirements(source: str) -> list[str]:
  """The optional modules needed by an example (its ``# requires:`` lines)."""
  return [
    module.strip()
    for line in source.splitlines()
    if line.startswith("# requires:")
    for module in line.removeprefix("# requires:").split(",")
    if module.strip()
  ]


def missingRequirements(source: str) -> list[str]:
  return [module for module in requirements(source) if importlib.util.find_spec(module) is None]


def _shown(lines: list[str], start: int) -> list[str] | None:
  """The result shown by the comment block beginning at line index start, if any."""
  shown = []
  for line in lines[start:]:
    if line == "#":
      shown.append("")
    elif line.startswith("# ") and not line.startswith("# requires:"):
      shown.append(line[2:])
    else:
      break
  return shown or None


def _trimmed(text: str) -> str:
  """Trailing spaces are not compared: a comment cannot keep them (formatters remove them)."""
  return "\n".join(line.rstrip() for line in text.splitlines())


def runExample(source: str, name: str = "<example>") -> None:
  """Execute an example; raise AssertionError if a result differs from the one shown."""
  tree = ast.parse(source, name)
  lines = source.splitlines()
  env = {"__name__": "__main__"}
  cwd = os.getcwd()
  with tempfile.TemporaryDirectory() as tmp, warnings.catch_warnings():
    warnings.simplefilter("ignore")  # deprecated members are documented too
    os.chdir(tmp)  # the files written by the example
    try:
      for stmt in tree.body:
        code = ast.get_source_segment(source, stmt)
        shown = _shown(lines, stmt.end_lineno) if isinstance(stmt, ast.Expr) else None
        if shown is None:
          with contextlib.redirect_stdout(io.StringIO()):  # unchecked output (displays...): not in the test log
            exec(compile(ast.Module([stmt], []), name, "exec"), env)
          continue
        call = stmt.value
        if isinstance(call, ast.Call) and isinstance(call.func, ast.Name) and call.func.id == "print":
          out = io.StringIO()
          with contextlib.redirect_stdout(out):
            exec(compile(ast.Module([stmt], []), name, "exec"), env)
          result = out.getvalue().rstrip("\n")
        else:
          result = repr(eval(compile(ast.Expression(call), name, "eval"), env))
        expected = "\n".join(shown)
        if _trimmed(result) != _trimmed(expected):
          raise AssertionError(f"{name}:{stmt.lineno}: {code}\n  shown : {expected!r}\n  actual: {result!r}")
    finally:
      os.chdir(cwd)


def runExampleFile(path: Path) -> None:
  runExample(path.read_text(encoding="utf-8"), path.name)


if __name__ == "__main__":
  failures = 0
  for arg in sys.argv[1:]:
    path = Path(arg)
    source = path.read_text(encoding="utf-8")
    if missing := missingRequirements(source):
      print(f"SKIP {path.name}: requires {', '.join(missing)}")
      continue
    try:
      runExampleFile(path)
      print(f"OK   {path.name}")
    except Exception as e:  # noqa: BLE001 (report every failing example)
      failures += 1
      print(f"FAIL {path.name}: {type(e).__name__}: {e}")
  sys.exit(1 if failures else 0)
