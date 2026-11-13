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

"""Every example (doc/examples/<page name>.py, or doc/examples/<class name>.class.py for a documented class) is right
and belongs to a documented member, function or class. That every documented member and function has an example is
checked by `act guideline` (check examples)."""

import sys
import tempfile
import unittest
from pathlib import Path

from .examplesRunner import missingRequirements, runExampleFile
from .pyAgrumTestSuite import addTests, pyAgrumTestCase

_DOC = Path(__file__).resolve().parents[2] / "doc"
_EXCLUDED = {"_build", "site", "Web"}  # outputs of the documentation builds, inside the Sphinx sources


def _apiinventory():
  sys.path.append(str(_DOC / "sphinx"))  # appended: no shadowing of an installed module
  import pyagrum_apiinventory  # noqa: PLC0415 (module of the Sphinx sources)

  return pyagrum_apiinventory


def _inventory():
  api = _apiinventory()
  return api.Inventory(api.scan(_DOC / "sphinx", lambda rel: rel.split("/")[0] in _EXCLUDED))


class TestExamples(pyAgrumTestCase):
  @classmethod
  def setUpClass(cls):
    cls.inventory = _inventory()
    cls.names = cls.inventory.exampleNames()
    cls.files = {path.stem: path for path in (_DOC / "examples").glob("*.py")}

  def testNoOrphanExample(self):
    unavailable = self.inventory.unavailable  # optional dependency missing here
    prefixes = tuple(f"{name}." for name in unavailable)
    orphans = sorted(
      stem for stem in set(self.files) - self.names if stem not in unavailable and not stem.startswith(prefixes)
    )
    self.assertFalse(
      orphans,
      "examples of no documented member, function or class (renamed? removed?):\n  " + "\n  ".join(orphans),
    )

  def testNoNamesDifferingOnlyByCase(self):
    # on a case-insensitive file system (macOS, Windows), such examples would be the same file
    byLowerCase = {}
    for name in self.names:
      byLowerCase.setdefault(name.lower(), []).append(name)
    clashes = sorted(sorted(names) for names in byLowerCase.values() if len(names) > 1)
    self.assertFalse(clashes, f"example names differing only by case: {clashes}")

  def testUnavailableFunctions(self):
    # a module or a function of pyagrum-functions that cannot be imported is reported, it does not stop the build
    api = _apiinventory()
    with tempfile.TemporaryDirectory() as tmp:
      Path(tmp, "page.rst").write_text(
        ".. pyagrum-functions:: pyagrum.noSuchModule\n\n.. pyagrum-functions::\n\n   pyagrum.noSuchFunction\n",
        encoding="utf-8",
      )
      inventory = api.Inventory(api.scan(Path(tmp), lambda rel: False))
    self.assertEqual(inventory.unavailable, {"pyagrum.noSuchModule", "pyagrum.noSuchFunction"})
    self.assertEqual(len(inventory.problems), 2)
    self.assertEqual(inventory.functions, {})

  def testExamples(self):
    for stem, path in sorted(self.files.items()):
      with self.subTest(example=stem):
        if missing := missingRequirements(path.read_text(encoding="utf-8")):
          self.skipTest(f"requires {', '.join(missing)}")  # optional dependency missing here
        runExampleFile(path)


ts = unittest.TestSuite()
addTests(ts, TestExamples)
