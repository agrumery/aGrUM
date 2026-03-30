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

import re
import sys
import time
import unittest

PREFIX = "🍈  "

_C_MODULE = "\033[36m"   # cyan   — module name (inside brackets)
_C_SUITE  = "\033[32m"   # green  — SuiteName
_C_METHOD = "\033[33m"   # yellow — method_name
_C_ERROR  = "\033[31m"   # red    — error count
_C_RESET  = "\033[0m"

# Mapping of unittest trailing status strings to left-side emoji.
# Order matters: longer matches first.
_STATUS_SUFFIXES = [
  (" ... unexpected success", "💥"),
  (" ... expected failure",   "✅"),
  (" ... ok",                 "✅"),
  (" ... FAIL",               "💥"),
  (" ... ERROR",              "🔴"),
]
_SKIPPED_RE = re.compile(r" \.\.\. skipped '.*'$")
_TEST_LINE_RE = re.compile(r"^(\S+)\s+\(([^)]+)\)$")


def _reformat(name: str, module_map: dict[str, str]) -> str:
  """Reformat 'method (path.Suite.Class.method)' as '[mod] Suite➜method'."""
  m = _TEST_LINE_RE.match(name)
  if not m:
    return name
  method, path = m.group(1), m.group(2)
  parts = path.split(".")
  suite = parts[1] if len(parts) > 1 else parts[0]
  mod_name = "main"
  for part in reversed(parts):
    mod = module_map.get(part, "")
    if mod:
      mod_name = mod
      break
  return f"[{_C_MODULE}{mod_name}{_C_RESET}] {_C_SUITE}{suite}{_C_RESET}➜{_C_METHOD}{method}{_C_RESET}"


def _format_test_line(line: str, module_map: dict[str, str]) -> str:
  """Replace trailing unittest status with a leading emoji and compact name."""
  for suffix, emoji in _STATUS_SUFFIXES:
    if line.endswith(suffix):
      return f"{emoji} {_reformat(line[: -len(suffix)], module_map)}"
  clean, n = _SKIPPED_RE.subn("", line)
  if n:
    return f"⏭  {_reformat(clean, module_map)}"
  return line


def notif(s: str = "") -> None:
  print(f"{PREFIX}{s}")


def warn(s: str) -> None:
  print(f"{PREFIX}⚠️  {s}")


def error(s: str) -> None:
  print(f"{PREFIX}💥 {s}")


class ModuleAwareResult(unittest.TextTestResult):
  """TextTestResult that tracks run/error counts per pyAgrum module."""

  def __init__(self, stream, descriptions, verbosity, module_map: dict[str, str]):
    super().__init__(stream, descriptions, verbosity)
    self._module_map = module_map
    self.module_stats: dict[str, tuple[int, int]] = {}  # {module: (runs, errors)}

  def _mod(self, test) -> str:
    return self._module_map.get(type(test).__name__, "main")

  def _count(self, test, is_error: bool = False) -> None:
    mod = self._mod(test)
    runs, errs = self.module_stats.get(mod, (0, 0))
    self.module_stats[mod] = (runs + 1, errs + (1 if is_error else 0))

  def addSuccess(self, test):
    super().addSuccess(test)
    self._count(test)

  def addFailure(self, test, err):
    super().addFailure(test, err)
    self._count(test, True)

  def addError(self, test, err):
    super().addError(test, err)
    self._count(test, True)

  def addExpectedFailure(self, test, err):
    super().addExpectedFailure(test, err)
    self._count(test)

  def addUnexpectedSuccess(self, test):
    super().addUnexpectedSuccess(test)
    self._count(test, True)


def notif_module_stats(module_stats: dict[str, tuple[int, int]]) -> None:
  """Print per-module error/total counts, right-aligned."""
  if not module_stats:
    return
  max_mod   = max(len(mod) for mod in module_stats)
  max_count = max(len(f"{errs}/{run}") for run, errs in module_stats.values())
  for mod, (run, errs) in sorted(module_stats.items()):
    label   = f"[{_C_MODULE}{mod}{_C_RESET}]"
    padding = " " * (max_mod - len(mod) + 1)
    count   = f"{errs}/{run}".rjust(max_count)
    if errs > 0:
      count = f"{_C_ERROR}{count}{_C_RESET} 💥"
    notif(f"{label}{padding}{count}")


class Timer:
  """Context manager to measure elapsed wall time.

  Usage::

      with Timer() as t:
          do_work()
      print(t.elapsed)  # seconds as float
  """

  def __enter__(self):
    self._start = time.monotonic()
    return self

  def __exit__(self, *_):
    self.elapsed = time.monotonic() - self._start


class PrefixStream:
  """Stream wrapper that prepends PREFIX to every output line.

  Buffers partial writes so that lines built across multiple write()
  calls are prefixed once, at the newline boundary.
  """

  def __init__(self, module_map: dict[str, str] | None = None):
    self._stream = sys.stdout
    self._buf = ""
    self._module_map = module_map or {}

  def write(self, s: str) -> None:
    self._buf += s
    while "\n" in self._buf:
      line, self._buf = self._buf.split("\n", 1)
      self._stream.write(f"{PREFIX}{_format_test_line(line, self._module_map)}\n")

  def flush(self) -> None:
    self._stream.flush()
