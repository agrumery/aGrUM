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

import inspect
import os
import subprocess
import sys
import unittest

import pyagrum as gum

from .pyAgrumTestSuite import pyAgrumTestCase, addTests
import importlib.util

# Check if the required libraries are installed
res = importlib.util.find_spec("sklearn")
sklearnFound = res is not None
res = importlib.util.find_spec("pandas")
pandasFound = res is not None
res = importlib.util.find_spec("matplotlib")
matplotlibFound = res is not None
res = importlib.util.find_spec("ipython")
ipythonFound = res is not None


class TestImport(pyAgrumTestCase):
  def testImport(self):
    try:
      if matplotlibFound and ipythonFound:
        pass
      else:
        self.log.warning("ipython and matplotlib are needed for modules lib.ipython and lib.notebook")

      if matplotlibFound:
        pass
      else:
        self.log.warning("matplotlib is needed for modules lib.ipython and lib.notebook")

      if sklearnFound:
        pass
      else:
        self.log.warning("sklearn is needed for modules lib.classifier")

    except Exception as e:
      self.assertFalse(False, "Import error : " + str(e))


class TestLazySubmodules(pyAgrumTestCase):
  """
  pyagrum exposes its compiled submodules (markov_random_field, credal_net, ...) lazily, through the
  module-level __getattr__ of wrappers/pyagrum/cmake/__init__.in.py. The name -> submodule table
  (_LAZY_SUBMODULE_ATTRS) is maintained by hand: these tests keep it in sync with the submodules.
  """

  # Public names of a submodule deliberately not reachable from the pyagrum namespace.
  UNLISTED_ON_PURPOSE = {
    # C++ interface class, only useful through its concrete MarkovRandomField
    "markov_random_field": {"IMarkovRandomField"},
  }

  @staticmethod
  def _submodules():
    return sorted(set(gum._LAZY_SUBMODULE_ATTRS.values()))

  def _importSubmodule(self, name):
    try:
      return importlib.import_module(f"pyagrum.{name}")
    except ModuleNotFoundError:
      self.skipTest(f"pyagrum.{name} is not part of this build")

  def testImportPyagrumLoadsNoSubmodule(self):
    # in a fresh interpreter: this process may already have imported some submodules
    code = (
      "import sys, pyagrum\n"
      f"names = {self._submodules()!r}\n"
      "print(sorted(n for n in names if f'pyagrum.{n}' in sys.modules))"
    )
    env = dict(os.environ, PYTHONPATH=os.pathsep.join(p for p in sys.path if p))
    res = subprocess.run([sys.executable, "-c", code], capture_output=True, text=True, env=env)
    self.assertEqual(res.returncode, 0, res.stderr)
    self.assertEqual(res.stdout.strip(), "[]")

  def testListedNamesResolveToTheirSubmodule(self):
    for name, submodule in gum._LAZY_SUBMODULE_ATTRS.items():
      with self.subTest(name=name):
        owner = self._importSubmodule(submodule)
        self.assertTrue(hasattr(owner, name), f"pyagrum.{submodule} has no attribute {name}")
        self.assertIs(getattr(gum, name), getattr(owner, name))

  def testCompositeAliasesResolve(self):
    for submodule in self._submodules():
      self._importSubmodule(submodule)  # skips the test if a submodule is not part of this build
    for name in gum._LAZY_COMPOSITE_TYPE_ALIASES:
      with self.subTest(name=name):
        self.assertIsNotNone(getattr(gum, name))

  def testEveryPublicNameOfASubmoduleIsListed(self):
    core = set(gum.__all__)
    for submodule in self._submodules():
      with self.subTest(submodule=submodule):
        owner = self._importSubmodule(submodule)
        owned = {n for n in dir(owner) if not n.startswith("_") and not inspect.ismodule(getattr(owner, n))} - core
        listed = {n for n, s in gum._LAZY_SUBMODULE_ATTRS.items() if s == submodule}
        missing = owned - listed - self.UNLISTED_ON_PURPOSE.get(submodule, set())
        self.assertEqual(missing, set(), f"public names of pyagrum.{submodule} missing from _LAZY_SUBMODULE_ATTRS")


ts = unittest.TestSuite()
addTests(ts, TestImport)
addTests(ts, TestLazySubmodules)
