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

"""The science fragments of the classes (doc/science/<class name>.rst) and the bibliography they cite (refs.bib)."""

import re
import sys
import unittest
from pathlib import Path

from .pyAgrumTestSuite import addTests, pyAgrumTestCase

_DOC = Path(__file__).resolve().parents[2] / "doc"
_EXCLUDED = {"_build", "site", "Web"}  # outputs of the documentation builds, inside the Sphinx sources
_ENTRY = re.compile(r"^@(?P<type>\w+)\s*\{\s*(?P<key>[^,\s]+)\s*,", re.M)


def _bibEntries() -> dict[str, str]:
  """Key -> text of each entry of refs.bib."""
  text = (_DOC / "sphinx" / "refs.bib").read_text(encoding="utf-8")
  starts = list(_ENTRY.finditer(text))
  ends = [m.start() for m in starts[1:]] + [len(text)]
  return {m.group("key"): text[m.start() : end] for m, end in zip(starts, ends)}


class TestScience(pyAgrumTestCase):
  @classmethod
  def setUpClass(cls):
    sys.path.append(str(_DOC / "sphinx"))  # appended: no shadowing of an installed module
    from pyagrum_apiinventory import Inventory, citedKeysOfText, scan  # noqa: PLC0415 (module of the Sphinx sources)

    cls.inventory = Inventory(scan(_DOC / "sphinx", lambda rel: rel.split("/")[0] in _EXCLUDED))
    cls.citedKeysOfText = staticmethod(citedKeysOfText)
    cls.files = {path.stem: path for path in (_DOC / "science").glob("*.rst")}
    cls.entries = _bibEntries()

  def testNoOrphanScience(self):
    unavailable = self.inventory.unavailable  # optional dependency missing here
    orphans = sorted(stem for stem in self.files if stem not in self.inventory.classes and stem not in unavailable)
    self.assertFalse(orphans, "science fragments of no documented class:\n  " + "\n  ".join(orphans))

  def testCitedKeysAreInTheBibliography(self):
    for stem, path in sorted(self.files.items()):
      with self.subTest(science=stem):
        keys = self.citedKeysOfText(path.read_text(encoding="utf-8"))
        self.assertTrue(keys, "a science fragment cites at least one reference")
        self.assertFalse(sorted(keys - set(self.entries)), "keys missing from refs.bib")

  def testEveryReferenceHasAnOnlineVersion(self):
    for key, entry in sorted(self.entries.items()):
      with self.subTest(reference=key):
        self.assertRegex(entry, r"(?im)^\s*(url|doi)\s*=", "no url nor doi")

  def testNoDuplicateKey(self):
    keys = [m.group("key") for m in _ENTRY.finditer((_DOC / "sphinx" / "refs.bib").read_text(encoding="utf-8"))]
    self.assertEqual(sorted(k for k in set(keys) if keys.count(k) > 1), [])


ts = unittest.TestSuite()
addTests(ts, TestScience)
