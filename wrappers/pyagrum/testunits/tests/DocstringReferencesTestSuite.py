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

"""The references to pyagrum objects in the documented docstrings follow the numpydoc convention.

In the prose of a docstring, a pyagrum object is written `pyagrum.X` (single backticks: Sphinx links it); double
backticks are for literal code only (``pyagrum.config['learning', 'alpha']``). The type lines of the numpydoc
sections (``bn : pyagrum.BayesNet``) stay plain: Sphinx links them too.
"""

import importlib
import inspect
import re
import sys
import unittest
from pathlib import Path

import pyagrum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase

_DOC = Path(__file__).resolve().parents[2] / "doc"
_EXCLUDED = {"_build", "site", "Web"}  # outputs of the documentation builds, inside the Sphinx sources
_SECTIONS = {
  "Parameters",
  "Returns",
  "Yields",
  "Raises",
  "Warns",
  "Attributes",
  "Other Parameters",
  "Receives",
  "See Also",
}
_ADMONITION = re.compile(
  r"\.\. (note|warning|seealso|tip|important|caution|attention|hint|danger|error|deprecated|"
  r"versionadded|versionchanged|admonition)::"
)
_NAME = r"[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*"
_SPAN = re.compile(r":[\w:+-]+:`[^`]*`|\*\*[^*]+\*\*|\*[^*\s][^*]*\*|``[^`]+``|`[^`]+`_{1,2}|`[^`]+`")
_PLAIN = re.compile(rf"(?<![\w.`/\-])(?:pyagrum|gum)\.{_NAME}(?![\w(\[]|\.\w)")  # an expression is code
_LITERAL = re.compile(rf"``((?:pyagrum|gum)\.{_NAME})(?:\(\))?``")
_REFERENCE = re.compile(rf"(?<!`)`((?:pyagrum|gum)\.{_NAME})`(?!`)")


def _proseLines(doc: str):
  """The lines of a numpydoc docstring written in prose: no title, type line, code, directive content."""
  lines = doc.splitlines()
  section, base, code = None, None, None
  for i, line in enumerate(lines):
    s = line.strip()
    ind = len(line) - len(line.lstrip())
    nxt = lines[i + 1].strip() if i + 1 < len(lines) else ""
    if code is not None:
      if s and ind <= code:
        code = None
      else:
        continue
    if not s or len(set(s)) == 1 and s[0] in "=-~^*#+\"'":
      continue
    if nxt and len(set(nxt)) == 1 and nxt[0] in "=-~^*#+\"'":  # a title
      section, base = (s, None) if nxt[0] == "-" else (section, base)
      continue
    if s.startswith(".. "):
      if not _ADMONITION.match(s):
        code = ind
      continue
    if s.startswith((">>>", "...")) or re.match(r":[\w-]+:( |$)", s):
      continue
    if s.endswith("::"):
      code = ind
    if section in _SECTIONS:
      base = ind if base is None else base
      if ind <= base:
        continue
    yield s


def _resolves(name: str) -> bool:
  obj = pyagrum
  for part in name.split(".")[1:]:
    try:
      obj = getattr(obj, part)
    except AttributeError:
      try:
        obj = importlib.import_module(f"{obj.__name__}.{part}")
      except ImportError:
        return False
  return True


class TestDocstringReferences(pyAgrumTestCase):
  @classmethod
  def setUpClass(cls):
    sys.path.append(str(_DOC / "sphinx"))  # appended: no shadowing of an installed module
    from pyagrum_apiinventory import Inventory, resolve, scan  # noqa: PLC0415 (module of the Sphinx sources)

    inventory = Inventory(scan(_DOC / "sphinx", lambda rel: rel.split("/")[0] in _EXCLUDED))
    names = set(inventory.pageNames()) | set(inventory.classes)
    cls.docs = {}
    for name in sorted(names):
      try:
        obj = resolve(name)
      except Exception:  # optional dependency missing here
        continue
      doc = obj.__doc__ if inspect.ismodule(obj) else inspect.getdoc(obj)
      if doc:
        cls.docs[name] = doc

  def _check(self, finder, message):
    found = {}
    for name, doc in self.docs.items():
      for line in _proseLines(doc):
        hits = finder(_SPAN.sub(" ", line), line)
        if hits:
          found.setdefault(name, []).extend(hits)
    self.assertFalse(found, message + ":\n  " + "\n  ".join(f"{k}: {v}" for k, v in sorted(found.items())))

  def testNoPlainReference(self):
    self._check(lambda text, _: _PLAIN.findall(text), "write `pyagrum.X` (single backticks), not a plain name")

  def testNoLiteralReference(self):
    self._check(
      lambda _, line: [m.group(0) for m in _LITERAL.finditer(line) if _resolves(m.group(1))],  # not a file name
      "write `pyagrum.X` (single backticks), not ``pyagrum.X``",
    )

  def testReferencesExist(self):
    def wrong(_, line):
      return [r for r in _REFERENCE.findall(line) if r.startswith("gum.") or not _resolves(r)]

    self._check(wrong, "`pyagrum.X` must name an existing pyagrum object (and not use the gum alias)")


ts = unittest.TestSuite()
addTests(ts, TestDocstringReferences)
